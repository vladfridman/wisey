//
//  Controller.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AutoCast.hpp"
#include "wisey/Composer.hpp"
#include "wisey/Controller.hpp"
#include "wisey/ControllerOwner.hpp"
#include "wisey/ControllerTypeSpecifierFull.hpp"
#include "wisey/Environment.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/FakeExpressionWithCleanup.hpp"
#include "wisey/FieldReferenceVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/IdentifierChain.hpp"
#include "wisey/LLVMFunction.hpp"
#include "wisey/LocalReferenceVariable.hpp"
#include "wisey/LocalSystemReferenceVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/MethodCall.hpp"
#include "wisey/Names.hpp"
#include "wisey/ObjectKindGlobal.hpp"
#include "wisey/ParameterReferenceVariable.hpp"
#include "wisey/ParameterReferenceVariableStatic.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ThreadExpression.hpp"
#include "wisey/VariableDeclaration.hpp"
#include "wisey/WiseyObjectType.hpp"

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
mScopeType(NULL),
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
      throw 1;
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
    throw 1;
  }
  return mNameToConstantMap.at(constantName);
}

Instruction* Controller::inject(IRGenerationContext& context,
                                InjectionArgumentList injectionArgumentList,
                                int line) const {
  checkInjectionArguments(context, injectionArgumentList, line);

  Function* function = context.getModule()->getFunction(getInjectFunctionName());
  assert(function && "Inject function for controller is not defined");

  Value* callArguments[mReceivedFields.size()];
  
  for (InjectionArgument* argument : injectionArgumentList) {
    string argumentName = argument->deriveFieldName();
    IField* field = findField(argumentName);
    assert(field->isReceived() && "Trying to initialize a field that is not of receive type");
    const IType* fieldType = field->getType();
    
    Value* argumentValue = argument->getValue(context, fieldType);
    const IType* argumentType = argument->getType(context);
    assert(argumentType->canAutoCastTo(context, fieldType) &&
           "Init value is not castable for a received field");
    Value* castValue = AutoCast::maybeCast(context, argumentType, argumentValue, fieldType, line);
    callArguments[mReceivedFieldIndexes.at(field)] = castValue;
  }

  IVariable* threadVariable = context.getScopes().getVariable(ThreadExpression::THREAD);
  Value* threadObject = threadVariable->generateIdentifierIR(context, line);
  IVariable* callstackVariable = context.getScopes().getVariable(ThreadExpression::CALL_STACK);
  Value* callstackObject = callstackVariable->generateIdentifierIR(context, line);

  vector<Value*> callArgumentsVector;
  callArgumentsVector.push_back(threadObject);
  callArgumentsVector.push_back(callstackObject);
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
  Interface* threadInterface = context.getInterface(Names::getThreadInterfaceFullName(), line);
  argumentTypes.push_back(threadInterface->getLLVMType(context));
  Controller* calstackController =
    context.getController(Names::getCallStackControllerFullName(), line);
  argumentTypes.push_back(calstackController->getLLVMType(context));
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
  if (isScopeInjected(context) && mReceivedFields.size()) {
    context.reportError(mReceivedFields.front()->getLine(),
                        "Scope injected controllers can not have received fields");
    throw 1;
  }
  Function* function = declareInjectFunction(context, line);
  ComposingFunction1Objects callback = mScopeType
  ? composeContextInjectFunctionBody
  : composeInjectFunctionBody;
  
  context.addComposingCallback1Objects(callback, function, this);
  return function;
}

void Controller::composeInjectFunctionBody(IRGenerationContext& context,
                                           Function* function,
                                           const void* objectType) {
  LLVMContext& llvmContext = context.getLLVMContext();
  const Controller* controller = (const Controller*) objectType;
  BasicBlock* declareBlock = BasicBlock::Create(llvmContext, "declare", function);
  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", function);
  context.getScopes().pushScope();
  context.setBasicBlock(entryBlock);
  context.setDeclarationsBlock(declareBlock);
  context.setObjectType(controller);
    
  Function::arg_iterator llvmArguments = function->arg_begin();
  llvm::Argument* thread = &*llvmArguments;
  thread->setName(ThreadExpression::THREAD);
  llvmArguments++;
  llvm::Argument* callstack = &*llvmArguments;
  callstack->setName(ThreadExpression::CALL_STACK);
  
  Interface* threadInterface = context.getInterface(Names::getThreadInterfaceFullName(), 0);
  IVariable* threadVariable = new ParameterReferenceVariableStatic(ThreadExpression::THREAD,
                                                                   threadInterface,
                                                                   thread,
                                                                   0);
  context.getScopes().setVariable(context, threadVariable);
  Controller* callstackController =
  context.getController(Names::getCallStackControllerFullName(), 0);
  IVariable* callstackVariable = new ParameterReferenceVariableStatic(ThreadExpression::CALL_STACK,
                                                                      callstackController,
                                                                      callstack,
                                                                      0);
  context.getScopes().setVariable(context, callstackVariable);

  Instruction* malloc = createMallocForObject(context, controller, "injectvar");
  Value* index[2];
  index[0] = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), 1);
  Instruction* objectStart = IRWriter::createGetElementPtrInst(context, malloc, index);
  
  controller->initializeReceivedFields(context, function, objectStart);
  controller->injectInjectedFields(context, objectStart);
  initializeVTable(context, controller, objectStart);
  
  IRWriter::createReturnInst(context, objectStart);

  context.setBasicBlock(declareBlock);
  IRWriter::createBranch(context, entryBlock);

  context.getScopes().popScope(context, 0);
}

void Controller::composeContextInjectFunctionBody(IRGenerationContext& context,
                                                  Function* function,
                                                  const void* objectType) {
  LLVMContext& llvmContext = context.getLLVMContext();
  const Controller* controller = (const Controller*) objectType;
  BasicBlock* entryBlock = BasicBlock::Create(llvmContext, "entry", function);
  context.getScopes().pushScope();
  context.setBasicBlock(entryBlock);
  context.setObjectType(controller);

  Function::arg_iterator llvmArguments = function->arg_begin();
  llvm::Argument* thread = &*llvmArguments;
  thread->setName(ThreadExpression::THREAD);
  llvmArguments++;
  llvm::Argument* callstack = &*llvmArguments;
  callstack->setName(ThreadExpression::CALL_STACK);

  Interface* threadInterface = context.getInterface(Names::getThreadInterfaceFullName(), 0);
  IVariable* threadVariable = new ParameterReferenceVariableStatic(ThreadExpression::THREAD,
                                                                   threadInterface,
                                                                   thread,
                                                                   0);
  context.getScopes().setVariable(context, threadVariable);
  Controller* callstackController =
    context.getController(Names::getCallStackControllerFullName(), 0);
  IVariable* callstackVariable = new ParameterReferenceVariableStatic(ThreadExpression::CALL_STACK,
                                                                      callstackController,
                                                                      callstack,
                                                                      0);
  context.getScopes().setVariable(context, callstackVariable);
  
  Identifier* identfier = new Identifier(ThreadExpression::THREAD, 0);
  IdentifierChain* methodIdentifier = new IdentifierChain(identfier,
                                                          Names::getGetContextManagerMethodName(),
                                                          0);
  ExpressionList callArguments;
  MethodCall* getContextManager = MethodCall::create(methodIdentifier, callArguments, 0);
  Value* contextManagerPointer = getContextManager->generateIR(context, PrimitiveTypes::VOID);
  
  string contextManagerVariableName = "contextManager";
  Controller* contextManagerController =
    context.getController(Names::getContextManagerFullName(), 0);
  IVariable* contextManagerVariable =
  new ParameterReferenceVariableStatic(contextManagerVariableName,
                                       contextManagerController,
                                       contextManagerPointer,
                                       0);
  context.getScopes().setVariable(context, contextManagerVariable);
  
  methodIdentifier = new IdentifierChain(new Identifier(contextManagerVariableName, 0),
                                         Names::getGetInstanceMethodName(),
                                         0);
  Value* contextObjectName = IObjectType::getObjectNamePointer(controller->mScopeType, context);
  FakeExpression* contextName = new FakeExpression(contextObjectName, PrimitiveTypes::STRING);
  Value* objectNamePointer = IObjectType::getObjectNamePointer(controller, context);
  FakeExpression* objectName = new FakeExpression(objectNamePointer, PrimitiveTypes::STRING);
  
  callArguments.clear();
  callArguments.push_back(contextName);
  callArguments.push_back(objectName);
  MethodCall* getInstance = MethodCall::create(methodIdentifier, callArguments, 0);
  Value* object = getInstance->generateIR(context, PrimitiveTypes::VOID);
  Value* instance = WiseyObjectType::WISEY_OBJECT_TYPE->castTo(context, object, controller, 0);

  BasicBlock* ifNullBlock = BasicBlock::Create(llvmContext, "if.null", function);
  BasicBlock* ifNotNullBlock = BasicBlock::Create(llvmContext, "if.not.null", function);
  
  ConstantPointerNull* null = ConstantPointerNull::get(controller->getLLVMType(context));
  Value* condition = IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, instance, null, "");
  IRWriter::createConditionalBranch(context, ifNullBlock, ifNotNullBlock, condition);
  
  context.setBasicBlock(ifNotNullBlock);
  
  IRWriter::createReturnInst(context, instance);
  
  context.setBasicBlock(ifNullBlock);

  Instruction* malloc = createMallocForObject(context, controller, "injectvar");
  Value* index[2];
  index[0] = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), 1);
  Instruction* objectStart = IRWriter::createGetElementPtrInst(context, malloc, index);

  controller->initializeReceivedFields(context, function, objectStart);
  controller->injectInjectedFields(context, objectStart);
  initializeVTable(context, controller, objectStart);
  
  methodIdentifier = new IdentifierChain(new Identifier(contextManagerVariableName, 0),
                                         Names::getSetInstanceMethodName(),
                                         0);
  contextName = new FakeExpression(contextObjectName, PrimitiveTypes::STRING);
  objectName = new FakeExpression(objectNamePointer, PrimitiveTypes::STRING);
  FakeExpression* instanceExpression = new FakeExpression(objectStart, controller->getOwner());

  callArguments.clear();
  callArguments.push_back(contextName);
  callArguments.push_back(objectName);
  callArguments.push_back(instanceExpression);
  MethodCall* setInstance = MethodCall::create(methodIdentifier, callArguments, 0);
  setInstance->generateIR(context, PrimitiveTypes::VOID);

  IRWriter::createReturnInst(context, objectStart);
  
  context.getScopes().popScope(context, 0);
  
  delete getContextManager;
  delete getInstance;
  delete setInstance;
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

const IMethod* Controller::findMethod(std::string methodName) const {
  return IConcreteObjectType::findMethodInObject(methodName, this);
}

vector<IMethod*> Controller::getMethods() const {
  return mMethods;
}

map<string, IMethod*> Controller::getNameToMethodMap() const {
  return mNameToMethodMap;
}

string Controller::getObjectNameGlobalVariableName() const {
  return mName + ".typename";
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
  if (isThread(context)) {
    Composer::incrementReferenceCountSafely(context, object);
  } else {
    Composer::incrementReferenceCountUnsafely(context, object);
  }
}

void Controller::decrementReferenceCount(IRGenerationContext& context, Value* object) const {
  if (isThread(context)) {
    Composer::decrementReferenceCountSafely(context, object);
  } else {
    Composer::decrementReferenceCountUnsafely(context, object);
  }
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

void Controller::createLocalVariable(IRGenerationContext& context, string name, int line) const {
  llvm::PointerType* llvmType = getLLVMType(context);
  
  Value* alloca = IRWriter::newAllocaInst(context, llvmType, name);
  IRWriter::newStoreInst(context, ConstantPointerNull::get(llvmType), alloca);
  
  IVariable* uninitializedVariable = name == ThreadExpression::CALL_STACK
    ? (IVariable*) new LocalSystemReferenceVariable(name, this, alloca, line)
    : (IVariable*) new LocalReferenceVariable(name, this, alloca, line);
  context.getScopes().setVariable(context, uninitializedVariable);
}

void Controller::createFieldVariable(IRGenerationContext& context,
                                     string name,
                                     const IConcreteObjectType* object,
                                     int line) const {
  IVariable* variable = new FieldReferenceVariable(name, object, line);
  context.getScopes().setVariable(context, variable);
}

void Controller::createParameterVariable(IRGenerationContext& context,
                                         string name,
                                         Value* value,
                                         int line) const {
  IVariable* variable = new ParameterReferenceVariable(name, this, value, line);
  incrementReferenceCount(context, value);
  context.getScopes().setVariable(context, variable);
}

const wisey::ArrayType* Controller::getArrayType(IRGenerationContext& context, int line) const {
  reportNonArrayType(context, line);
  throw 1;
}

int Controller::getLine() const {
  return mLine;
}

void Controller::checkInjectedFields(IRGenerationContext& context) const {
  for (InjectedField* injectedField : mInjectedFields) {
    injectedField->checkInjectionArguments(context);
  }
}

void Controller::setScopeType(const IObjectType* objectType) {
  mScopeType = objectType;
}

bool Controller::isScopeInjected(IRGenerationContext& context) const {
  return mScopeType != NULL;
}

void Controller::checkInjectionArguments(IRGenerationContext& context,
                                         const InjectionArgumentList received,
                                         int line) const {
  checkArgumentsAreWellFormed(context, received, line);
  checkAllFieldsAreSet(context, received, line);
  checkReceivedValuesAreForReceivedFields(context, received, line);
}

void Controller::checkArgumentsAreWellFormed(IRGenerationContext& context,
                                             const InjectionArgumentList injectionArgumentList,
                                             int line) const {
  bool areArgumentsWellFormed = true;
  
  for (InjectionArgument* argument : injectionArgumentList) {
    areArgumentsWellFormed &= argument->checkArgument(context, this, line);
  }
  
  if (!areArgumentsWellFormed) {
    throw 1;
  }
}

void Controller::checkAllFieldsAreSet(IRGenerationContext& context,
                                      const InjectionArgumentList injectionArgumentList,
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
  throw 1;
}

void Controller:: checkReceivedValuesAreForReceivedFields(IRGenerationContext& context,
                                                          const InjectionArgumentList&
                                                          injectionArgumentList,
                                                          int line) const {
  for (InjectionArgument* argument : injectionArgumentList) {
    IField* field = findField(argument->deriveFieldName());
    if (!field->isReceived()) {
      context.reportError(line, "Use receive type for fileds that are initialized at injection time");
      throw 1;
    }
    const IType* fieldType = field->getType();
    
    const IType* argumentType = argument->getType(context);
    if (!argumentType->canAutoCastTo(context, fieldType)) {
      context.reportError(line, "Injector argumet value for field '" + field->getName() +
                          "' does not match its type");
      throw 1;
    }
  }
}

void Controller::initializeReceivedFields(IRGenerationContext& context,
                                          llvm::Function* function,
                                          Instruction* objectStart) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Function::arg_iterator functionArguments = function->arg_begin();
  functionArguments++;
  functionArguments++;

  Value* index[2];
  index[0] = llvm::Constant::getNullValue(Type::getInt32Ty(llvmContext));
  for (IField* field : mReceivedFields) {
    Value* functionArgument = &*functionArguments;
    functionArgument->setName(field->getName());
    index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), getFieldIndex(field));
    GetElementPtrInst* fieldPointer = IRWriter::createGetElementPtrInst(context,
                                                                        objectStart,
                                                                        index);
    IRWriter::newStoreInst(context, functionArgument, fieldPointer);
    const IType* fieldType = field->getType();
    if (fieldType->isReference()) {
      ((const IReferenceType*) fieldType)->incrementReferenceCount(context, functionArgument);
    }
    functionArguments++;
  }
}

void Controller::injectInjectedFields(IRGenerationContext& context,
                                      Instruction* objectStart) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  if (!mInjectedFields.size()) {
    return;
  }
  
  for (IField* field : mReceivedFields) {
    field->getType()->createFieldVariable(context, field->getName(), this, field->getLine());
  }
  IVariable* thisVariable = new ParameterReferenceVariableStatic(IObjectType::THIS,
                                                                 this,
                                                                 objectStart,
                                                                 mLine);
  context.getScopes().setVariable(context, thisVariable);

  Value* index[2];
  index[0] = llvm::Constant::getNullValue(Type::getInt32Ty(llvmContext));
  for (InjectedField* field : mInjectedFields) {
    if (!field->isImmediate()) {
      continue;
    }
    index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), getFieldIndex(field));
    GetElementPtrInst* fieldPointer = IRWriter::createGetElementPtrInst(context,
                                                                        objectStart,
                                                                        index);
    Value* injectedValue = field->inject(context);
    IRWriter::newStoreInst(context, injectedValue, fieldPointer);
    const IType* fieldType = field->getType();
    if (fieldType->isReference()) {
      ((const IReferenceType*) fieldType)->incrementReferenceCount(context, injectedValue);
    }
  }
}

void Controller::defineFieldInjectorFunctions(IRGenerationContext& context, int line) const {
  for (InjectedField* field : mInjectedFields) {
    if (field->isImmediate()) {
      continue;
    }
    field->defineInjectionFunction(context, this);
  }
}

void Controller::declareFieldInjectionFunctions(IRGenerationContext& context, int line) const {
  for (InjectedField* field : mInjectedFields) {
    if (field->isImmediate()) {
      continue;
    }
    field->declareInjectionFunction(context, this);
  }
}

llvm::Constant* Controller::getObjectTypeNameGlobal(IRGenerationContext& context) const {
  if (isThread(context)) {
    return ObjectKindGlobal::getThread(context);
  }
  return ObjectKindGlobal::getController(context);
}

bool Controller::isThread(IRGenerationContext& context) const {
  Interface* threadInterface = context.getInterface(Names::getThreadInterfaceFullName(), 0);
  for (Interface* interface : mFlattenedInterfaceHierarchy) {
    if (interface == threadInterface) {
      return true;
    }
  }
  return false;
}
