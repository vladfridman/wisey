//
//  Model.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/2/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/AdjustReferenceCounterForConcreteObjectSafelyFunction.hpp"
#include "wisey/AutoCast.hpp"
#include "wisey/Cast.hpp"
#include "wisey/Environment.hpp"
#include "wisey/FieldReferenceVariable.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/LLVMFunction.hpp"
#include "wisey/LocalReferenceVariable.hpp"
#include "wisey/Log.hpp"
#include "wisey/Model.hpp"
#include "wisey/ModelOwner.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/ParameterReferenceVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Model::Model(AccessLevel accessLevel,
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
  mModelOwner = new ModelOwner(this);
}

Model::~Model() {
  delete mModelOwner;
  for(IField* field : mFieldsOrdered) {
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
  for (LLVMFunction* llvmFunction : mLLVMFunctions) {
    delete llvmFunction;
  }
  mLLVMFunctions.clear();
}

Model* Model::newModel(AccessLevel accessLevel, string name, StructType* structType, int line) {
  return new Model(accessLevel, name, structType, false, line);
}

Model* Model::newExternalModel(string name, StructType* structType, int line) {
  return new Model(AccessLevel::PUBLIC_ACCESS, name, structType, true, line);
}

bool Model::isPublic() const {
  return mIsPublic;
}

void Model::setFields(vector<IField*> fields, unsigned long startIndex) {
  mFieldsOrdered = fields;
  unsigned long index = startIndex;
  for (IField* field : fields) {
    mFields[field->getName()] = field;
    mFieldIndexes[field] = index;
    if (!field->isFixed()) {
      Log::e_deprecated("Models can only have fixed fields");
      exit(1);
    }
    const IType* fieldType = field->getType();
    if (!fieldType->isPrimitive() && !fieldType->isModel() && !fieldType->isInterface()) {
      Log::e_deprecated("Models can only have fields of primitive or model type");
      exit(1);
    }

    index++;
  }
}

void Model::setInterfaces(vector<Interface*> interfaces) {
  mInterfaces = interfaces;
  for (Interface* interface : mInterfaces) {
    IConcreteObjectType::addInterfaceAndItsParents(interface, mFlattenedInterfaceHierarchy);
  }
}

void Model::setMethods(vector<IMethod*> methods) {
  mMethods = methods;
  for (IMethod* method : methods) {
    mNameToMethodMap[method->getName()] = method;
  }
}

void Model::setStructBodyTypes(vector<Type *> types) {
  mStructType->setBody(types);
}

void Model::setConstants(vector<Constant*> constants) {
  mConstants = constants;
  for (Constant* constant : constants) {
    mNameToConstantMap[constant->getName()] = constant;
  }
}

wisey::Constant* Model::findConstant(string constantName) const {
  if (!mNameToConstantMap.count(constantName)) {
    Log::e_deprecated("Model " + mName + " does not have constant named " + constantName);
    exit(1);
  }
  return mNameToConstantMap.at(constantName);
}

vector<wisey::Constant*> Model::getConstants() const {
  return mConstants;
}

void Model::setLLVMFunctions(vector<LLVMFunction*> llvmFunctions) {
  mLLVMFunctions = llvmFunctions;
  for (LLVMFunction* function : llvmFunctions) {
    mLLVMFunctionMap[function->getName()] = function;
  }
}

vector<LLVMFunction*> Model::getLLVMFunctions() const {
  return mLLVMFunctions;
}

LLVMFunction* Model::findLLVMFunction(string functionName) const {
  if (!mLLVMFunctionMap.count(functionName)) {
    Log::e_deprecated("LLVM function " + functionName + " not found in object " + getTypeName());
    exit(1);
  }
  return mLLVMFunctionMap.at(functionName);
}

IField* Model::findField(string fieldName) const {
  if (!mFields.count(fieldName)) {
    return NULL;
  }
  
  return mFields.at(fieldName);
}

unsigned long Model::getFieldIndex(IField* field) const {
  return mFieldIndexes.at(field);
}

vector<IField*> Model::getFields() const {
  return mFieldsOrdered;
}

vector<string> Model::getMissingFields(set<string> givenFields) const {
  vector<string> missingFields;
  
  for (map<string, IField*>::const_iterator iterator = mFields.begin();
       iterator != mFields.end();
       iterator++) {
    string fieldName = iterator->first;
    if (givenFields.find(fieldName) == givenFields.end()) {
      missingFields.push_back(fieldName);
    }
  }
  
  return missingFields;
}

IMethod* Model::findMethod(std::string methodName) const {
  return IConcreteObjectType::findMethodInObject(methodName, this);
}

vector<IMethod*> Model::getMethods() const {
  return mMethods;
}

map<string, IMethod*> Model::getNameToMethodMap() const {
  return mNameToMethodMap;
}

string Model::getVTableName() const {
  return getTypeName() + ".vtable";
}

unsigned long Model::getVTableSize() const {
  return IConcreteObjectType::getVTableSizeForObject(this);
}

string Model::getObjectNameGlobalVariableName() const {
  return getTypeName() + ".name";
}

string Model::getObjectShortNameGlobalVariableName() const {
  return getTypeName() + ".shortname";
}

string Model::getTypeTableName() const {
  return getTypeName() + ".typetable";
}

vector<Interface*> Model::getInterfaces() const {
  return mInterfaces;
}

vector<Interface*> Model::getFlattenedInterfaceHierarchy() const {
  return mFlattenedInterfaceHierarchy;
}

bool Model::doesImplmentInterface(Interface* interface) const {
  // TODO: optimize this
  return getInterfaceIndex((IConcreteObjectType*) this, interface) >= 0;
}

string Model::getTypeName() const {
  return mName;
}

string Model::getShortName() const {
  return mName.substr(mName.find_last_of('.') + 1);
}

llvm::PointerType* Model::getLLVMType(IRGenerationContext& context) const {
  return mStructType->getPointerTo();
}

bool Model::canCastTo(IRGenerationContext& context, const IType* toType) const {
  if (toType->isNative() && toType->isReference()) {
    return !toType->isController() && !toType->isNode() && !toType->isThread();
  }
  return IConcreteObjectType::canCast(this, toType);
}

bool Model::canAutoCastTo(IRGenerationContext& context, const IType* toType) const {
  return canCastTo(context, toType);
}

Value* Model::castTo(IRGenerationContext& context,
                     Value* fromValue,
                     const IType* toType,
                     int line) const {
  return IConcreteObjectType::castTo(context, (IConcreteObjectType*) this, fromValue, toType, line);
}

bool Model::isPrimitive() const {
  return false;
}

bool Model::isOwner() const {
  return false;
}

bool Model::isReference() const {
  return true;
}

bool Model::isArray() const {
  return false;
}

bool Model::isFunction() const {
  return false;
}

bool Model::isPackage() const {
  return false;
}

bool Model::isController() const {
  return false;
}

bool Model::isInterface() const {
  return false;
}

bool Model::isModel() const {
  return true;
}

bool Model::isNode() const {
  return false;
}

bool Model::isThread() const {
  return false;
}

bool Model::isNative() const {
  return false;
}

bool Model::isPointer() const {
  return false;
}

Instruction* Model::build(IRGenerationContext& context,
                          const ObjectBuilderArgumentList& objectBuilderArgumentList,
                          int line) const {
  checkArguments(objectBuilderArgumentList);
  Instruction* malloc = IConcreteObjectType::createMallocForObject(context, this, "buildervar");
  initializeFields(context, objectBuilderArgumentList, malloc, line);
  initializeVTable(context, (IConcreteObjectType*) this, malloc);
  
  return malloc;
}

void Model::createRTTI(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8PointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
 
  llvm::Constant* modelNamePointer = IObjectType::getObjectNamePointer(this, context);

  llvm::Constant* cxxabiv117ClassType =
    context.getModule()->getOrInsertGlobal("_ZTVN10__cxxabiv117__class_type_infoE",
                                           int8PointerType);
  Value* Idx[1];
  Idx[0] = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 2);
  Type* elementType = cxxabiv117ClassType->getType()->getPointerElementType();
  llvm::Constant* cxxabiv117ClassTypeElement =
    ConstantExpr::getGetElementPtr(elementType, cxxabiv117ClassType, Idx);
  llvm::Constant* cxxabiv117ClassTypeElementBitcast =
    ConstantExpr::getTruncOrBitCast(cxxabiv117ClassTypeElement, int8PointerType);
  
  vector<llvm::Constant*> rttiArray;
  vector<Type*> types;
  rttiArray.push_back(cxxabiv117ClassTypeElementBitcast);
  rttiArray.push_back(modelNamePointer);
  types.push_back(int8PointerType);
  types.push_back(int8PointerType);
  
  StructType* rttiGlobalType = StructType::get(llvmContext, types);
  llvm::Constant* rttiGlobalConstantStruct = ConstantStruct::get(rttiGlobalType, rttiArray);

  new GlobalVariable(*context.getModule(),
                     rttiGlobalType,
                     true,
                     GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                     rttiGlobalConstantStruct,
                     getRTTIVariableName());
}

void Model::checkArguments(const ObjectBuilderArgumentList& objectBuilderArgumentList) const {
  checkArgumentsAreWellFormed(objectBuilderArgumentList);
  checkAllFieldsAreSet(objectBuilderArgumentList);
}

void Model::checkArgumentsAreWellFormed(const ObjectBuilderArgumentList&
                                        objectBuilderArgumentList) const {
  bool areArgumentsWellFormed = true;
  
  for (ObjectBuilderArgument* argument : objectBuilderArgumentList) {
    areArgumentsWellFormed &= argument->checkArgument(this);
  }
  
  if (!areArgumentsWellFormed) {
    Log::e_deprecated("Some arguments for the model " + mName + " builder are not well formed");
    exit(1);
  }
}

void Model::checkAllFieldsAreSet(const ObjectBuilderArgumentList& objectBuilderArgumentList) const {
  set<string> allFieldsThatAreSet;
  for (ObjectBuilderArgument* argument : objectBuilderArgumentList) {
    allFieldsThatAreSet.insert(argument->deriveFieldName());
  }
  
  vector<string> missingFields = getMissingFields(allFieldsThatAreSet);
  if (missingFields.size() == 0) {
    return;
  }
  
  for (string missingField : missingFields) {
    Log::e_deprecated("Field " + missingField + " is not initialized");
  }
  Log::e_deprecated("Some fields of the model " + mName + " are not initialized.");
  exit(1);
}

void Model::initializeFields(IRGenerationContext& context,
                             const ObjectBuilderArgumentList& objectBuilderArgumentList,
                             Instruction* malloc,
                             int line) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Value* index[2];
  index[0] = llvm::Constant::getNullValue(Type::getInt32Ty(llvmContext));
  for (ObjectBuilderArgument* argument : objectBuilderArgumentList) {
    string argumentName = argument->deriveFieldName();
    IField* field = findField(argumentName);
    index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), getFieldIndex(field));
    GetElementPtrInst* fieldPointer = IRWriter::createGetElementPtrInst(context, malloc, index);
    const IType* fieldType = field->getType();
    
    Value* argumentValue = argument->getValue(context, fieldType);
    const IType* argumentType = argument->getType(context);
    if (!argumentType->canAutoCastTo(context, fieldType)) {
      Log::e_deprecated("Model builder argument value for field " + argumentName +
             " does not match its type");
      exit(1);
    }
    Value* castValue = AutoCast::maybeCast(context, argumentType, argumentValue, fieldType, line);
    IRWriter::newStoreInst(context, castValue, fieldPointer);
    if (fieldType->isReference()) {
      ((const IReferenceType*) fieldType)->incrementReferenceCount(context, castValue);
    }
  }
}

string Model::getRTTIVariableName() const {
  return getTypeName() + ".rtti";
}

const IObjectOwnerType* Model::getOwner() const {
  return mModelOwner;
}

bool Model::isExternal() const {
  return mIsExternal;
}

void Model::printToStream(IRGenerationContext& context, iostream& stream) const {
	IConcreteObjectType::printObjectToStream(context, this, stream);
}

void Model::incrementReferenceCount(IRGenerationContext& context, Value* object) const {
  AdjustReferenceCounterForConcreteObjectSafelyFunction::call(context, object, 1);
}

void Model::decrementReferenceCount(IRGenerationContext& context, Value* object) const {
  AdjustReferenceCounterForConcreteObjectSafelyFunction::call(context, object, -1);
}

Value* Model::getReferenceCount(IRGenerationContext& context, Value* object) const {
  return getReferenceCountForObject(context, object);
}

void Model::setImportProfile(ImportProfile* importProfile) {
  mImportProfile = importProfile;
}

ImportProfile* Model::getImportProfile() const {
  return mImportProfile;
}

void Model::addInnerObject(const IObjectType* innerObject) {
  mInnerObjects[innerObject->getShortName()] = innerObject;
}

const IObjectType* Model::getInnerObject(string shortName) const {
  if (mInnerObjects.count(shortName)) {
    return mInnerObjects.at(shortName);
  }
  return NULL;
}

map<string, const IObjectType*> Model::getInnerObjects() const {
  return mInnerObjects;
}

void Model::markAsInner() {
  mIsInner = true;
}

bool Model::isInner() const {
  return mIsInner;
}

Function* Model::getReferenceAdjustmentFunction(IRGenerationContext& context) const {
  return AdjustReferenceCounterForConcreteObjectSafelyFunction::get(context);
}

void Model::createLocalVariable(IRGenerationContext& context, string name) const {
  llvm::PointerType* llvmType = getLLVMType(context);
  
  Value* alloca = IRWriter::newAllocaInst(context, llvmType, "referenceDeclaration");
  IRWriter::newStoreInst(context, ConstantPointerNull::get(llvmType), alloca);
  
  IVariable* uninitializedVariable = new LocalReferenceVariable(name, this, alloca);
  context.getScopes().setVariable(uninitializedVariable);
}

void Model::createFieldVariable(IRGenerationContext& context,
                                string name,
                                const IConcreteObjectType* object) const {
  IVariable* variable = new FieldReferenceVariable(name, object);
  context.getScopes().setVariable(variable);
}

void Model::createParameterVariable(IRGenerationContext& context,
                                    string name,
                                    Value* value) const {
  IVariable* variable = new ParameterReferenceVariable(name, this, value);
  incrementReferenceCount(context, value);
  context.getScopes().setVariable(variable);
}

const wisey::ArrayType* Model::getArrayType(IRGenerationContext& context) const {
  ArrayType::reportNonArrayType();
  exit(1);
}

Instruction* Model::inject(IRGenerationContext& context,
                           const InjectionArgumentList injectionArgumentList,
                           int line) const {
  repotNonInjectableType(context, this, line);
  exit(1);
}

int Model::getLine() const {
  return mLine;
}
