//
//  Controller.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AdjustReferenceCounterForConcreteObjectUnsafelyFunction.hpp"
#include "wisey/AutoCast.hpp"
#include "wisey/Controller.hpp"
#include "wisey/ControllerOwner.hpp"
#include "wisey/Environment.hpp"
#include "wisey/FieldReferenceVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMFunction.hpp"
#include "wisey/LocalReferenceVariable.hpp"
#include "wisey/LocalSystemReferenceVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/ParameterReferenceVariable.hpp"
#include "wisey/ThreadExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Controller::Controller(AccessLevel accessLevel,
                       string name,
                       StructType* structType,
                       ImportProfile* importProfile,
                       bool isExternal,
                       int line) :
mIsPublic(accessLevel == PUBLIC_ACCESS),
mName(name),
mStructType(structType),
mIsExternal(isExternal),
mIsInner(false),
mControllerOwner(new ControllerOwner(this)),
mImportProfile(importProfile),
mLine(line) {
  assert(importProfile && "Import profile can not be NULL at Controller creation");
}

Controller::~Controller() {
  delete mControllerOwner;
  for(IField* field : mFieldsOrdered) {
    delete field;
  }
  mFieldsOrdered.clear();
  mFieldIndexes.clear();
  mFields.clear();
  mReceivedFields.clear();
  mReceivedFieldIndexes.clear();
  mInjectedFields.clear();
  mStateFields.clear();
  for (IMethod* method : mMethods) {
    delete method;
  }
  mMethods.clear();
  mNameToMethodMap.clear();
  mInterfaces.clear();
  mFlattenedInterfaceHierarchy.clear();
  for (Constant* constant : mConstants) {
    delete constant;
  }
  mConstants.clear();
  mNameToConstantMap.clear();
  mInnerObjects.clear();
  for (LLVMFunction* llvmFunction : mLLVMFunctions) {
    delete llvmFunction;
  }
  mLLVMFunctions.clear();
}

Controller* Controller::newController(AccessLevel accessLevel,
                                      string name,
                                      StructType* structType,
                                      ImportProfile* importProfile,
                                      int line) {
  return new Controller(accessLevel , name, structType, importProfile, false, line);
}

Controller* Controller::newExternalController(string name,
                                              StructType* structType,
                                              ImportProfile* importProfile,
                                              int line) {
  return new Controller(AccessLevel::PUBLIC_ACCESS, name, structType, importProfile, true, line);
}

vector<IField*> Controller::getReceivedFields() const {
  return mReceivedFields;
}

bool Controller::isPublic() const {
  return mIsPublic;
}

void Controller::setFields(IRGenerationContext& context,
                           vector<IField*> fields,
                           unsigned long startIndex) {
  unsigned long index = startIndex;
  for (IField* field : fields) {
    mFields[field->getName()] = field;
    mFieldsOrdered.push_back(field);
    mFieldIndexes[field] = index;
    if (field->isReceived()) {
      mReceivedFieldIndexes[field] = mReceivedFields.size();
      mReceivedFields.push_back(field);
    } else if (field->isState()) {
      mStateFields.push_back(field);
    } else if (field->isInjected()) {
      mInjectedFields.push_back((InjectedField*) field);
    } else {
      context.reportError(field->getLine(),
                          "Controllers can only have received, injected or state fields");
      exit(1);
    }
    index++;
  }
}

void Controller::setInterfaces(vector<Interface *> interfaces) {
  mInterfaces = interfaces;
  
  for (Interface* interface : mInterfaces) {
    IConcreteObjectType::addInterfaceAndItsParents(interface, mFlattenedInterfaceHierarchy);
  }
}

void Controller::setMethods(vector<IMethod *> methods) {
  mMethods = methods;
  for (IMethod* method : methods) {
    mNameToMethodMap[method->getName()] = method;
  }
}

void Controller::setStructBodyTypes(vector<Type*> types) {
  mStructType->setBody(types);
}

void Controller::setConstants(vector<Constant*> constants) {
  mConstants = constants;
  for (Constant* constant : constants) {
    mNameToConstantMap[constant->getName()] = constant;
  }
}

vector<wisey::Constant*> Controller::getConstants() const {
  return mConstants;
}

void Controller::setLLVMFunctions(vector<LLVMFunction*> llvmFunctions) {
  mLLVMFunctions = llvmFunctions;
  for (LLVMFunction* function : llvmFunctions) {
    mLLVMFunctionMap[function->getName()] = function;
  }
}

vector<LLVMFunction*> Controller::getLLVMFunctions() const {
  return mLLVMFunctions;
}

LLVMFunction* Controller::findLLVMFunction(string functionName) const {
  if (mLLVMFunctionMap.count(functionName)) {
    return mLLVMFunctionMap.at(functionName);
  }
  return NULL;
}

wisey::Constant* Controller::findConstant(IRGenerationContext& context,
                                          string constantName,
                                          int line) const {
  if (!mNameToConstantMap.count(constantName)) {
    context.reportError(line, "Controller " + mName +
                        " does not have constant named " + constantName);
    exit(1);
  }
  return mNameToConstantMap.at(constantName);
}

Instruction* Controller::inject(IRGenerationContext& context,
                                InjectionArgumentList injectionArgumentList,
                                int line) const {
  checkArguments(context, injectionArgumentList, line);

  Function* function = context.getModule()->getFunction(getInjectFunctionName());
  assert(function && "Inject function for controller is not defined");

  Value* callArguments[mReceivedFields.size()];
  
  for (InjectionArgument* argument : injectionArgumentList) {
    string argumentName = argument->deriveFieldName();
    IField* field = findField(argumentName);
    const IType* fieldType = field->getType();
    
    Value* argumentValue = argument->getValue(context, fieldType);
    const IType* argumentType = argument->getType(context);
    if (!argumentType->canAutoCastTo(context, fieldType)) {
      context.reportError(line, "Injector argumet value for field '" + field->getName() +
                          "' does not match its type");
      exit(1);
    }
    Value* castValue = AutoCast::maybeCast(context, argumentType, argumentValue, fieldType, line);
    callArguments[mReceivedFieldIndexes.at(field)] = castValue;
  }

  vector<Value*> callArgumentsVector;
  for (Value* callArgument : callArguments) {
    callArgumentsVector.push_back(callArgument);
  }
  return IRWriter::createCallInst(context, function, callArgumentsVector, "");
}

string Controller::getInjectFunctionName() const {
  return getTypeName() + ".inject";
}

Function* Controller::declareInjectFunction(IRGenerationContext& context, int line) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Value* index[2];
  index[0] = llvm::Constant::getNullValue(Type::getInt32Ty(llvmContext));
  vector<Type*> argumentTypes;
  for (IField* receivedField : mReceivedFields) {
    argumentTypes.push_back(receivedField->getType()->getLLVMType(context));
  }
  
  FunctionType* functionType = FunctionType::get(getLLVMType(context), argumentTypes, false);
  return Function::Create(functionType,
                          GlobalValue::ExternalLinkage,
                          getInjectFunctionName(),
                          context.getModule());
}

Function* Controller::createInjectFunction(IRGenerationContext& context, int line) const {
  Function* function = declareInjectFunction(context, line);
  context.addComposingCallback1Objects(composeInjectFunctionBody, function, this);
  return function;
}

void Controller::composeInjectFunctionBody(IRGenerationContext& context,
                                           Function* function,
                                           const void* object) {
  const Controller* controller = (const Controller*) object;
  BasicBlock* entryBlock = BasicBlock::Create(context.getLLVMContext(), "entry", function);
  context.getScopes().pushScope();
  context.setBasicBlock(entryBlock);
  context.setObjectType(controller);
  
  Instruction* malloc = createMallocForObject(context, controller, "injectvar");
  controller->initializeReceivedFields(context, function, malloc);
  initializeVTable(context, controller, malloc);
  IRWriter::createReturnInst(context, malloc);
  
  context.getScopes().popScope(context, 0);
}

vector<string> Controller::getMissingReceivedFields(set<string> givenFields) const {
  vector<string> missingFields;
  
  for (IField* field : mReceivedFields) {
    if (givenFields.find(field->getName()) == givenFields.end()) {
      missingFields.push_back(field->getName());
    }
  }
  
  return missingFields;
}

vector<Interface*> Controller::getInterfaces() const {
  return mInterfaces;
}

vector<Interface*> Controller::getFlattenedInterfaceHierarchy() const {
  return mFlattenedInterfaceHierarchy;
}

string Controller::getTypeTableName() const {
  return mName + ".typetable";
}

string Controller::getVTableName() const {
  return mName + ".vtable";
}

unsigned long Controller::getVTableSize() const {
  return IConcreteObjectType::getVTableSizeForObject(this);
}

IField* Controller::findField(string fieldName) const {
  if (!mFields.count(fieldName)) {
    return NULL;
  }
  
  return mFields.at(fieldName);
}

unsigned long Controller::getFieldIndex(const IField* field) const {
  return mFieldIndexes.at(field);
}

vector<IField*> Controller::getFields() const {
  return mFieldsOrdered;
}

vector<InjectedField*> Controller::getInjectedFields() const {
  return mInjectedFields;
}

IMethod* Controller::findMethod(std::string methodName) const {
  return IConcreteObjectType::findMethodInObject(methodName, this);
}

vector<IMethod*> Controller::getMethods() const {
  return mMethods;
}

map<string, IMethod*> Controller::getNameToMethodMap() const {
  return mNameToMethodMap;
}

string Controller::getObjectNameGlobalVariableName() const {
  return mName + ".name";
}

string Controller::getObjectShortNameGlobalVariableName() const {
  return mName + ".shortname";
}

string Controller::getTypeName() const {
  return mName;
}

string Controller::getShortName() const {
  return mName.substr(mName.find_last_of('.') + 1);
}

llvm::PointerType* Controller::getLLVMType(IRGenerationContext& context) const {
  return mStructType->getPointerTo();
}

bool Controller::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType->isNative() && toType->isReference()) {
    return !toType->isModel() && !toType->isNode();
  }
  return IConcreteObjectType::canCast(this, toType);
}

bool Controller::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return canCastTo(context, toType);
}

Value* Controller::castTo(IRGenerationContext& context,
                          Value* fromValue,
                          const IType* toType,
                          int line) const {
  return IConcreteObjectType::castTo(context, (IConcreteObjectType*) this, fromValue, toType, line);
}

bool Controller::isPrimitive() const {
  return false;
}

bool Controller::isOwner() const {
  return false;
}

bool Controller::isReference() const {
  return true;
}

bool Controller::isArray() const {
  return false;
}

bool Controller::isFunction() const {
  return false;
}

bool Controller::isPackage() const {
  return false;
}

bool Controller::isController() const {
  return true;
}

bool Controller::isInterface() const {
  return false;
}

bool Controller::isModel() const {
  return false;
}

bool Controller::isNode() const {
  return false;
}

bool Controller::isNative() const {
  return false;
}

bool Controller::isPointer() const {
  return false;
}

bool Controller::isImmutable() const {
  return false;
}

const IObjectOwnerType* Controller::getOwner() const {
  return mControllerOwner;
}

bool Controller::isExternal() const {
  return mIsExternal;
}

void Controller::printToStream(IRGenerationContext& context, iostream& stream) const {
  IConcreteObjectType::printObjectToStream(context, this, stream);
}

void Controller::incrementReferenceCount(IRGenerationContext& context, Value* object) const {
  AdjustReferenceCounterForConcreteObjectUnsafelyFunction::call(context, object, 1);
}

void Controller::decrementReferenceCount(IRGenerationContext& context, Value* object) const {
  AdjustReferenceCounterForConcreteObjectUnsafelyFunction::call(context, object, -1);
}

Value* Controller::getReferenceCount(IRGenerationContext& context, Value* object) const {
  return getReferenceCountForObject(context, object);
}

ImportProfile* Controller::getImportProfile() const {
  return mImportProfile;
}

void Controller::addInnerObject(const IObjectType* innerObject) {
  mInnerObjects[innerObject->getShortName()] = innerObject;
}

const IObjectType* Controller::getInnerObject(string shortName) const {
  if (mInnerObjects.count(shortName)) {
    return mInnerObjects.at(shortName);
  }
  return NULL;
}

map<string, const IObjectType*> Controller::getInnerObjects() const {
  return mInnerObjects;
}

void Controller::markAsInner() {
  mIsInner = true;
}

bool Controller::isInner() const {
  return mIsInner;
}

Function* Controller::getReferenceAdjustmentFunction(IRGenerationContext& context) const {
  return AdjustReferenceCounterForConcreteObjectUnsafelyFunction::get(context);
}

void Controller::createLocalVariable(IRGenerationContext& context, string name) const {
  llvm::PointerType* llvmType = getLLVMType(context);
  
  Value* alloca = IRWriter::newAllocaInst(context, llvmType, "referenceDeclaration");
  IRWriter::newStoreInst(context, ConstantPointerNull::get(llvmType), alloca);
  
  IVariable* uninitializedVariable = name == ThreadExpression::CALL_STACK
    ? (IVariable*) new LocalSystemReferenceVariable(name, this, alloca)
    : (IVariable*) new LocalReferenceVariable(name, this, alloca);
  context.getScopes().setVariable(uninitializedVariable);
}

void Controller::createFieldVariable(IRGenerationContext& context,
                                      string name,
                                      const IConcreteObjectType* object) const {
  IVariable* variable = new FieldReferenceVariable(name, object);
  context.getScopes().setVariable(variable);
}

void Controller::createParameterVariable(IRGenerationContext& context,
                                         string name,
                                         Value* value) const {
  IVariable* variable = new ParameterReferenceVariable(name, this, value);
  incrementReferenceCount(context, value);
  context.getScopes().setVariable(variable);
}

const wisey::ArrayType* Controller::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

int Controller::getLine() const {
  return mLine;
}

void Controller::checkArguments(IRGenerationContext& context,
                                const InjectionArgumentList& received,
                                int line) const {
  checkArgumentsAreWellFormed(context, received, line);
  checkAllFieldsAreSet(context, received, line);
}

void Controller::checkArgumentsAreWellFormed(IRGenerationContext& context,
                                             const InjectionArgumentList& injectionArgumentList,
                                             int line) const {
  bool areArgumentsWellFormed = true;
  
  for (InjectionArgument* argument : injectionArgumentList) {
    areArgumentsWellFormed &= argument->checkArgument(this);
  }
  
  if (!areArgumentsWellFormed) {
    context.reportError(line, "Some injection arguments for injected object " + getTypeName() +
                        " are not well formed");
    exit(1);
  }
}

void Controller:: checkAllFieldsAreSet(IRGenerationContext& context,
                                       const InjectionArgumentList& injectionArgumentList,
                                       int line) const {
  set<string> allFieldsThatAreSet;
  for (InjectionArgument* argument : injectionArgumentList) {
    allFieldsThatAreSet.insert(argument->deriveFieldName());
  }
  
  vector<string> missingFields = getMissingReceivedFields(allFieldsThatAreSet);
  if (missingFields.size() == 0) {
    return;
  }
  
  for (string missingField : missingFields) {
    context.reportError(line, "Received field " + missingField + " is not initialized");
  }
  context.reportError(line, "Some received fields of the controller " + getTypeName() +
                      " are not initialized.");
  exit(1);
}

void Controller::initializeReceivedFields(IRGenerationContext& context,
                                          llvm::Function* function,
                                          Instruction* malloc) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Function::arg_iterator functionArguments = function->arg_begin();

  Value* index[2];
  index[0] = llvm::Constant::getNullValue(Type::getInt32Ty(llvmContext));
  for (IField* field : mReceivedFields) {
    Value* functionArgument = &*functionArguments;
    functionArgument->setName(field->getName());
    index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), getFieldIndex(field));
    GetElementPtrInst* fieldPointer = IRWriter::createGetElementPtrInst(context, malloc, index);
    IRWriter::newStoreInst(context, functionArgument, fieldPointer);
    const IType* fieldType = field->getType();
    if (fieldType->isReference()) {
      ((const IReferenceType*) fieldType)->incrementReferenceCount(context, functionArgument);
    }
    functionArguments++;
  }
}

void Controller::defineFieldInjectorFunctions(IRGenerationContext& context, int line) const {
  for (InjectedField* field : mInjectedFields) {
    field->defineInjectionFunction(context, this);
  }
}

void Controller::declareFieldInjectionFunctions(IRGenerationContext& context, int line) const {
  for (InjectedField* field : mInjectedFields) {
    field->declareInjectionFunction(context, this);
  }
}
