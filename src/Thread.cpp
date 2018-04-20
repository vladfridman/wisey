//
//  Thread.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/3/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AdjustReferenceCounterForConcreteObjectSafelyFunction.hpp"
#include "wisey/FieldReferenceVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMFunction.hpp"
#include "wisey/LocalReferenceVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/ParameterReferenceVariable.hpp"
#include "wisey/Thread.hpp"
#include "wisey/ThreadOwner.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Thread::Thread(AccessLevel accessLevel,
               string name,
               StructType* structType,
               bool isExternal,
               int line) :
mIsPublic(accessLevel == PUBLIC_ACCESS),
mName(name),
mStructType(structType),
mIsExternal(isExternal),
mIsInner(false),
mLine(line) {
  mThreadOwner = new ThreadOwner(this);
}

Thread::~Thread() {
  delete mThreadOwner;
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

Thread* Thread::newThread(AccessLevel accessLevel,
                          string name,
                          StructType* structType,
                          int line) {
  return new Thread(accessLevel , name, structType, false, line);
}

Thread* Thread::newExternalThread(string name, StructType* structType, int line) {
  return new Thread(AccessLevel::PUBLIC_ACCESS, name, structType, true, line);
}

bool Thread::isPublic() const {
  return mIsPublic;
}

void Thread::setFields(IRGenerationContext& context,
                       vector<IField*> fields,
                       unsigned long startIndex) {
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
      Log::e_deprecated("Threads can only have received, injected or state fields");
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
    Log::e_deprecated("Thread " + mName + " does not have constant named " + constantName);
    exit(1);
  }
  return mNameToConstantMap.at(constantName);
}

void Thread::setLLVMFunctions(vector<LLVMFunction*> llvmFunctions) {
  mLLVMFunctions = llvmFunctions;
  for (LLVMFunction* function : llvmFunctions) {
    mLLVMFunctionMap[function->getName()] = function;
  }
}

vector<LLVMFunction*> Thread::getLLVMFunctions() const {
  return mLLVMFunctions;
}

LLVMFunction* Thread::findLLVMFunction(string functionName) const {
  if (mLLVMFunctionMap.count(functionName)) {
    return mLLVMFunctionMap.at(functionName);
  }
  return NULL;
}

Instruction* Thread::inject(IRGenerationContext& context,
                            InjectionArgumentList injectionArgumentList,
                            int line) const {
  return IInjectableConcreteObjectType::injectObject(context, this, injectionArgumentList, line);
}

vector<string> Thread::getMissingReceivedFields(set<string> givenFields) const {
  vector<string> missingFields;
  
  for (IField* field : mReceivedFields) {
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
  return IConcreteObjectType::getVTableSizeForObject(this);
}

IField* Thread::findField(string fieldName) const {
  if (!mFields.count(fieldName)) {
    return NULL;
  }
  
  return mFields.at(fieldName);
}

unsigned long Thread::getFieldIndex(IField* field) const {
  return mFieldIndexes.at(field);
}

vector<IField*> Thread::getFields() const {
  return mFieldsOrdered;
}

vector<InjectedField*> Thread::getInjectedFields() const {
  return mInjectedFields;
}

IMethod* Thread::findMethod(std::string methodName) const {
  return IConcreteObjectType::findMethodInObject(methodName, this);
}

map<string, IMethod*> Thread::getNameToMethodMap() const {
  return mNameToMethodMap;
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
  if (toType->isNative() && toType->isReference()) {
    return !toType->isController() && !toType->isModel() && !toType->isNode();
  }
  return IConcreteObjectType::canCast(this, toType);
}

bool Thread::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return canCastTo(context, toType);
}

Value* Thread::castTo(IRGenerationContext& context,
                      Value* fromValue,
                      const IType* toType,
                      int line) const {
  return IConcreteObjectType::castTo(context, (IConcreteObjectType*) this, fromValue, toType, line);
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

bool Thread::isNative() const {
  return false;
}

bool Thread::isPointer() const {
  return false;
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
  llvm::PointerType* llvmType = getLLVMType(context);
  
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

int Thread::getLine() const {
  return mLine;
}
