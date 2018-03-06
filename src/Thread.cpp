//
//  Thread.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/3/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AdjustReferenceCounterForConcreteObjectSafelyFunction.hpp"
#include "wisey/ArrayAllocation.hpp"
#include "wisey/ArraySpecificOwnerType.hpp"
#include "wisey/AutoCast.hpp"
#include "wisey/Cast.hpp"
#include "wisey/Environment.hpp"
#include "wisey/FieldReferenceVariable.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/LocalReferenceVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/Names.hpp"
#include "wisey/ParameterReferenceVariable.hpp"
#include "wisey/Thread.hpp"
#include "wisey/ThreadOwner.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Thread::Thread(AccessLevel accessLevel,
               string name,
               StructType* structType,
               bool isExternal) :
mAccessLevel(accessLevel),
mName(name),
mStructType(structType),
mIsExternal(isExternal),
mIsInner(false) {
  mThreadOwner = new ThreadOwner(this);
}

Thread::~Thread() {
  delete mThreadOwner;
  for(Field* field : mFieldsOrdered) {
    delete field;
  }
  mFieldsOrdered.clear();
  mFieldIndexes.clear();
  mFields.clear();
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
}

Thread* Thread::newThread(AccessLevel accessLevel,
                          string name,
                          StructType* structType) {
  return new Thread(accessLevel , name, structType, false);
}

Thread* Thread::newExternalThread(string name, StructType* structType) {
  return new Thread(AccessLevel::PUBLIC_ACCESS, name, structType, true);
}

AccessLevel Thread::getAccessLevel() const {
  return mAccessLevel;
}

void Thread::setFields(vector<Field*> fields, unsigned long startIndex) {
  unsigned long index = startIndex;
  for (Field* field : fields) {
    mFields[field->getName()] = field;
    mFieldsOrdered.push_back(field);
    mFieldIndexes[field] = index;
    if (field->getFieldKind() != FIXED_FIELD) {
      Log::e("Threads can only have fixed fields");
      exit(1);
    }
    const IType* fieldType = field->getType();
    if (!fieldType->isPrimitive() && !fieldType->isModel() && !fieldType->isInterface()) {
      Log::e("Threads can only have fields of primitive or model type");
      exit(1);
    }
    index++;
  }
}

void Thread::setInterfaces(vector<Interface *> interfaces) {
  mInterfaces = interfaces;
  
  for (Interface* interface : mInterfaces) {
    IConcreteObjectType::addInterfaceAndItsParents(interface, mFlattenedInterfaceHierarchy);
  }
}

void Thread::setMethods(vector<IMethod *> methods) {
  mMethods = methods;
  for (IMethod* method : methods) {
    mNameToMethodMap[method->getName()] = method;
  }
}

void Thread::setStructBodyTypes(vector<Type*> types) {
  mStructType->setBody(types);
}

void Thread::setConstants(vector<Constant*> constants) {
  mConstants = constants;
  for (Constant* constant : constants) {
    mNameToConstantMap[constant->getName()] = constant;
  }
}

vector<wisey::Constant*> Thread::getConstants() const {
  return mConstants;
}

wisey::Constant* Thread::findConstant(string constantName) const {
  if (!mNameToConstantMap.count(constantName)) {
    Log::e("Thread " + mName + " does not have constant named " + constantName);
    exit(1);
  }
  return mNameToConstantMap.at(constantName);
}

Instruction* Thread::inject(IRGenerationContext& context,
                            InjectionArgumentList injectionArgumentList,
                            int line) const {
  const IObjectType* lastObjectType = context.getObjectType();
  context.setObjectType(this);
  
  checkArguments(injectionArgumentList);
  Instruction* malloc = createMalloc(context);
  IntrinsicFunctions::setMemoryToZero(context, malloc, ConstantExpr::getSizeOf(mStructType));
  initializeFiexedFields(context, injectionArgumentList, malloc, line);
  initializeVTable(context, (IConcreteObjectType*) this, malloc);
  
  context.setObjectType(lastObjectType);
  
  return malloc;
}

void Thread::checkArguments(const InjectionArgumentList& received) const {
  checkArgumentsAreWellFormed(received);
  checkAllFieldsAreSet(received);
}

void Thread::checkArgumentsAreWellFormed(const InjectionArgumentList& injectionArgumentList) const {
  bool areArgumentsWellFormed = true;
  
  for (InjectionArgument* argument : injectionArgumentList) {
    areArgumentsWellFormed &= argument->checkArgument(this);
  }
  
  if (!areArgumentsWellFormed) {
    Log::e("Some injection arguments for thread " + mName + " are not well formed");
    exit(1);
  }
}

void Thread::checkAllFieldsAreSet(const InjectionArgumentList& injectionArgumentList) const {
  set<string> allFieldsThatAreSet;
  for (InjectionArgument* argument : injectionArgumentList) {
    allFieldsThatAreSet.insert(argument->deriveFieldName());
  }
  
  vector<string> missingFields = getMissingReceivedFields(allFieldsThatAreSet);
  if (missingFields.size() == 0) {
    return;
  }
  
  for (string missingField : missingFields) {
    Log::e("Fixed field " + missingField + " is not initialized");
  }
  Log::e("Some fixed fields of the thread " + mName + " are not initialized.");
  exit(1);
}

vector<string> Thread::getMissingReceivedFields(set<string> givenFields) const {
  vector<string> missingFields;
  
  for (Field* field : mFieldsOrdered) {
    if (givenFields.find(field->getName()) == givenFields.end()) {
      missingFields.push_back(field->getName());
    }
  }
  
  return missingFields;
}

vector<Interface*> Thread::getInterfaces() const {
  return mInterfaces;
}

vector<Interface*> Thread::getFlattenedInterfaceHierarchy() const {
  return mFlattenedInterfaceHierarchy;
}

string Thread::getTypeTableName() const {
  return mName + ".typetable";
}

string Thread::getVTableName() const {
  return mName + ".vtable";
}

unsigned long Thread::getVTableSize() const {
  return mFlattenedInterfaceHierarchy.size();
}

Instruction* Thread::createMalloc(IRGenerationContext& context) const {
  Type* structType = getLLVMType(context)->getPointerElementType();
  llvm::Constant* allocSize = ConstantExpr::getSizeOf(structType);
  llvm::Constant* one = ConstantInt::get(Type::getInt64Ty(context.getLLVMContext()), 1);
  Instruction* malloc = IRWriter::createMalloc(context, structType, allocSize, one, "injectvar");
  
  return malloc;
}

Field* Thread::findField(string fieldName) const {
  if (!mFields.count(fieldName)) {
    return NULL;
  }
  
  return mFields.at(fieldName);
}

unsigned long Thread::getFieldIndex(Field* field) const {
  return mFieldIndexes.at(field);
}

vector<Field*> Thread::getFields() const {
  return mFieldsOrdered;
}

IMethod* Thread::findMethod(std::string methodName) const {
  if (!mNameToMethodMap.count(methodName)) {
    return NULL;
  }
  
  return mNameToMethodMap.at(methodName);
}

vector<IMethod*> Thread::getMethods() const {
  return mMethods;
}

string Thread::getObjectNameGlobalVariableName() const {
  return mName + ".name";
}

string Thread::getObjectShortNameGlobalVariableName() const {
  return mName + ".shortname";
}

string Thread::getTypeName() const {
  return mName;
}

string Thread::getShortName() const {
  return mName.substr(mName.find_last_of('.') + 1);
}

llvm::PointerType* Thread::getLLVMType(IRGenerationContext& context) const {
  return mStructType->getPointerTo();
}

bool Thread::canCastTo(IRGenerationContext& context, const IType* toType) const {
  return IConcreteObjectType::canCast(this, toType);
}

bool Thread::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return canCastTo(context, toType);
}

Value* Thread::castTo(IRGenerationContext& context,
                      Value* fromValue,
                      const IType* toType,
                      int line) const {
  return IConcreteObjectType::castTo(context, (IConcreteObjectType*) this, fromValue, toType);
}

bool Thread::isPrimitive() const {
  return false;
}

bool Thread::isOwner() const {
  return false;
}

bool Thread::isReference() const {
  return true;
}

bool Thread::isArray() const {
  return false;
}

bool Thread::isFunction() const {
  return false;
}

bool Thread::isPackage() const {
  return false;
}

bool Thread::isController() const {
  return false;
}

bool Thread::isInterface() const {
  return false;
}

bool Thread::isModel() const {
  return false;
}

bool Thread::isNode() const {
  return false;
}

bool Thread::isThread() const {
  return true;
}

const IObjectOwnerType* Thread::getOwner() const {
  return mThreadOwner;
}

bool Thread::isExternal() const {
  return mIsExternal;
}

void Thread::printToStream(IRGenerationContext& context, iostream& stream) const {
  IConcreteObjectType::printObjectToStream(context, this, stream);
}

void Thread::incrementReferenceCount(IRGenerationContext& context, Value* object) const {
  AdjustReferenceCounterForConcreteObjectSafelyFunction::call(context, object, 1);
}

void Thread::decrementReferenceCount(IRGenerationContext& context, Value* object) const {
  AdjustReferenceCounterForConcreteObjectSafelyFunction::call(context, object, -1);
}

Value* Thread::getReferenceCount(IRGenerationContext& context, Value* object) const {
  return getReferenceCountForObject(context, object);
}

void Thread::setImportProfile(ImportProfile* importProfile) {
  mImportProfile = importProfile;
}

ImportProfile* Thread::getImportProfile() const {
  return mImportProfile;
}

void Thread::addInnerObject(const IObjectType* innerObject) {
  mInnerObjects[innerObject->getShortName()] = innerObject;
}

const IObjectType* Thread::getInnerObject(string shortName) const {
  if (mInnerObjects.count(shortName)) {
    return mInnerObjects.at(shortName);
  }
  return NULL;
}

map<string, const IObjectType*> Thread::getInnerObjects() const {
  return mInnerObjects;
}

void Thread::markAsInner() {
  mIsInner = true;
}

bool Thread::isInner() const {
  return mIsInner;
}

Function* Thread::getReferenceAdjustmentFunction(IRGenerationContext& context) const {
  return AdjustReferenceCounterForConcreteObjectSafelyFunction::get(context);
}

void Thread::createLocalVariable(IRGenerationContext& context, string name) const {
  PointerType* llvmType = getLLVMType(context);
  
  Value* alloca = IRWriter::newAllocaInst(context, llvmType, "referenceDeclaration");
  IRWriter::newStoreInst(context, ConstantPointerNull::get(llvmType), alloca);
  
  IVariable* uninitializedVariable = new LocalReferenceVariable(name, this, alloca);
  context.getScopes().setVariable(uninitializedVariable);
}

void Thread::createFieldVariable(IRGenerationContext& context,
                                 string name,
                                 const IConcreteObjectType* object) const {
  IVariable* variable = new FieldReferenceVariable(name, object);
  context.getScopes().setVariable(variable);
}

void Thread::createParameterVariable(IRGenerationContext& context,
                                     string name,
                                     Value* value) const {
  IVariable* variable = new ParameterReferenceVariable(name, this, value);
  incrementReferenceCount(context, value);
  context.getScopes().setVariable(variable);
}

const wisey::ArrayType* Thread::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

const Thread* Thread::getObjectType() const {
  return this;
}

void Thread::initializeFiexedFields(IRGenerationContext& context,
                                    const InjectionArgumentList& controllerInjectorArguments,
                                    Instruction* malloc,
                                    int line) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Value* index[2];
  index[0] = llvm::Constant::getNullValue(Type::getInt32Ty(llvmContext));
  for (InjectionArgument* argument : controllerInjectorArguments) {
    string argumentName = argument->deriveFieldName();
    Field* field = findField(argumentName);
    index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), getFieldIndex(field));
    GetElementPtrInst* fieldPointer = IRWriter::createGetElementPtrInst(context, malloc, index);
    const IType* fieldType = field->getType();
    
    Value* argumentValue = argument->getValue(context, fieldType);
    const IType* argumentType = argument->getType(context);
    if (!argumentType->canAutoCastTo(context, fieldType)) {
      Log::e("Thread injector argumet value for field '" + field->getName() +
             "' does not match its type");
      exit(1);
    }
    Value* castValue = AutoCast::maybeCast(context, argumentType, argumentValue, fieldType, line);
    IRWriter::newStoreInst(context, castValue, fieldPointer);
    if (fieldType->isReference()) {
      ((IObjectType*) fieldType)->incrementReferenceCount(context, castValue);
    }
  }
}
