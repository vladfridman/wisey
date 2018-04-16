//
//  Controller.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AdjustReferenceCounterForConcreteObjectUnsafelyFunction.hpp"
#include "wisey/ArrayAllocation.hpp"
#include "wisey/ArraySpecificOwnerType.hpp"
#include "wisey/AutoCast.hpp"
#include "wisey/Cast.hpp"
#include "wisey/Controller.hpp"
#include "wisey/ControllerOwner.hpp"
#include "wisey/Environment.hpp"
#include "wisey/FieldReferenceVariable.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/InterfaceOwner.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/LLVMFunction.hpp"
#include "wisey/LocalReferenceVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/Names.hpp"
#include "wisey/ParameterReferenceVariable.hpp"
#include "wisey/ThreadOwner.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Controller::Controller(AccessLevel accessLevel,
                       string name,
                       StructType* structType,
                       bool isExternal) :
mAccessLevel(accessLevel),
mName(name),
mStructType(structType),
mIsExternal(isExternal),
mIsInner(false) {
  mControllerOwner = new ControllerOwner(this);
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
                                      StructType* structType) {
  return new Controller(accessLevel , name, structType, false);
}

Controller* Controller::newExternalController(string name, StructType* structType) {
  return new Controller(AccessLevel::PUBLIC_ACCESS, name, structType, true);
}

AccessLevel Controller::getAccessLevel() const {
  return mAccessLevel;
}

void Controller::setFields(vector<IField*> fields, unsigned long startIndex) {
  unsigned long index = startIndex;
  for (IField* field : fields) {
    mFields[field->getName()] = field;
    mFieldsOrdered.push_back(field);
    mFieldIndexes[field] = index;
    if (field->isReceived()) {
      mReceivedFields.push_back(field);
    } else if (field->isState()) {
      mStateFields.push_back(field);
    } else if (field->isInjected()) {
      mInjectedFields.push_back((InjectedField*) field);
    } else {
      Log::e_deprecated("Controllers can only have received, injected or state fields");
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
  if (!mLLVMFunctionMap.count(functionName)) {
    Log::e_deprecated("LLVM function " + functionName + " not found in object " + getTypeName());
    exit(1);
  }
  return mLLVMFunctionMap.at(functionName);
}

wisey::Constant* Controller::findConstant(string constantName) const {
  if (!mNameToConstantMap.count(constantName)) {
    Log::e_deprecated("Controller " + mName + " does not have constant named " + constantName);
    exit(1);
  }
  return mNameToConstantMap.at(constantName);
}

Instruction* Controller::inject(IRGenerationContext& context,
                                InjectionArgumentList injectionArgumentList,
                                int line) const {
  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(this);
  
  checkArguments(injectionArgumentList);
  Instruction* malloc = IConcreteObjectType::createMallocForObject(context, this, "injectvar");
  initializeReceivedFields(context, injectionArgumentList, malloc, line);
  initializeInjectedFields(context, malloc);
  initializeVTable(context, (IConcreteObjectType*) this, malloc);
  
  context.setObjectType(lastObjectType);
  
  return malloc;
}

void Controller::checkArguments(const InjectionArgumentList& received) const {
  checkArgumentsAreWellFormed(received);
  checkAllFieldsAreSet(received);
}

void Controller::checkArgumentsAreWellFormed(const InjectionArgumentList&
                                             injectionArgumentList) const {
  bool areArgumentsWellFormed = true;
  
  for (InjectionArgument* argument : injectionArgumentList) {
    areArgumentsWellFormed &= argument->checkArgument(this);
  }
  
  if (!areArgumentsWellFormed) {
    Log::e_deprecated("Some injection arguments for controller " + mName + " are not well formed");
    exit(1);
  }
}

void Controller::checkAllFieldsAreSet(const InjectionArgumentList& injectionArgumentList) const {
  set<string> allFieldsThatAreSet;
  for (InjectionArgument* argument : injectionArgumentList) {
    allFieldsThatAreSet.insert(argument->deriveFieldName());
  }
  
  vector<string> missingFields = getMissingReceivedFields(allFieldsThatAreSet);
  if (missingFields.size() == 0) {
    return;
  }
  
  for (string missingField : missingFields) {
    Log::e_deprecated("Received field " + missingField + " is not initialized");
  }
  Log::e_deprecated("Some received fields of the controller " + mName + " are not initialized.");
  exit(1);
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

unsigned long Controller::getFieldIndex(IField* field) const {
  return mFieldIndexes.at(field);
}

vector<IField*> Controller::getFields() const {
  return mFieldsOrdered;
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
    return !toType->isModel() && !toType->isNode() && !toType->isThread();
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

bool Controller::isThread() const {
  return false;
}

bool Controller::isNative() const {
  return false;
}

bool Controller::isPointer() const {
  return false;
}

void Controller::initializeReceivedFields(IRGenerationContext& context,
                                          const InjectionArgumentList& controllerInjectorArguments,
                                          Instruction* malloc,
                                          int line) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Value* index[2];
  index[0] = llvm::Constant::getNullValue(Type::getInt32Ty(llvmContext));
  for (InjectionArgument* argument : controllerInjectorArguments) {
    string argumentName = argument->deriveFieldName();
    IField* field = findField(argumentName);
    index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), getFieldIndex(field));
    GetElementPtrInst* fieldPointer = IRWriter::createGetElementPtrInst(context, malloc, index);
    const IType* fieldType = field->getType();
    
    Value* argumentValue = argument->getValue(context, fieldType);
    const IType* argumentType = argument->getType(context);
    if (!argumentType->canAutoCastTo(context, fieldType)) {
      Log::e_deprecated("Controller injector argumet value for field '" + field->getName() +
             "' does not match its type");
      exit(1);
    }
    Value* castValue = AutoCast::maybeCast(context, argumentType, argumentValue, fieldType, line);
    IRWriter::newStoreInst(context, castValue, fieldPointer);
    if (fieldType->isReference()) {
      ((const IReferenceType*) fieldType)->incrementReferenceCount(context, castValue);
    }
  }
}

void Controller::initializeInjectedFields(IRGenerationContext& context, Instruction* malloc) const {
  LLVMContext& llvmContext = context.getLLVMContext();

  Value *index[2];
  index[0] = llvm::Constant::getNullValue(Type::getInt32Ty(llvmContext));
  for (InjectedField* field : mInjectedFields) {
    const IType* fieldType = field->getType();
    Value* fieldValue = NULL;
    if (fieldType->isReference()) {
      Log::e_deprecated("Injected fields must have owner type denoted by '*'");
      exit(1);
    } else if (fieldType->isArray()) {
      const ArraySpecificOwnerType* arraySpecificOwnerType =
      (const ArraySpecificOwnerType*) field->getInjectedType();
      const ArraySpecificType* arraySpecificType = arraySpecificOwnerType->getArraySpecificType();
      Value* arrayPointer = ArrayAllocation::allocateArray(context, arraySpecificType);
      fieldValue = IRWriter::newBitCastInst(context, arrayPointer, arraySpecificType->
                                            getArrayType(context)->getLLVMType(context));
    } else if (fieldType->isController()) {
      const ControllerOwner* controllerOwner = (const ControllerOwner*) fieldType;
      fieldValue = controllerOwner->getReference()->inject(context,
                                                           field->getInjectionArguments(),
                                                           field->getLine());
    } else if (fieldType->isThread()) {
      const ThreadOwner* threadOwner = (const ThreadOwner*) fieldType;
      fieldValue = threadOwner->getReference()->inject(context,
                                                       field->getInjectionArguments(),
                                                       field->getLine());
    } else if (fieldType->isInterface()) {
      const InterfaceOwner* interfaceOwner = (const InterfaceOwner*) fieldType;
      fieldValue = interfaceOwner->getReference()->inject(context,
                                                          field->getInjectionArguments(),
                                                          field->getLine());
    } else {
      Log::e_deprecated("Attempt to inject a variable that is not of injectable type");
      exit(1);
    }
    index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), getFieldIndex(field));
    GetElementPtrInst* fieldPointer = IRWriter::createGetElementPtrInst(context, malloc, index);
    IRWriter::newStoreInst(context, fieldValue, fieldPointer);
  }
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

void Controller::setImportProfile(ImportProfile* importProfile) {
  mImportProfile = importProfile;
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
  
  IVariable* uninitializedVariable = new LocalReferenceVariable(name, this, alloca);
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
