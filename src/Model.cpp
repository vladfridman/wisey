//
//  Model.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/2/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Cast.hpp"
#include "wisey/Environment.hpp"
#include "wisey/Log.hpp"
#include "wisey/Model.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Model::~Model() {
  mFields.clear();
  mMethods.clear();
}

void Model::setFields(map<string, Field *> fields) {
  mFields = fields;
}

void Model::setInterfaces(vector<Interface*> interfaces) {
  mInterfaces = interfaces;
  for (Interface* interface : mInterfaces) {
    addInterfaceAndItsParents(mFlattenedInterfaceHierarchy, interface);
  }
}

void Model::setMethods(vector<Method*> methods) {
  mMethods = methods;
  for (Method* method : methods) {
    mNameToMethodMap[method->getName()] = method;
  }
}

void Model::setStructBodyTypes(vector<Type *> types) {
  mStructType->setBody(types);
}

Value* Model::getSize(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Value* nullPointer = ConstantPointerNull::get(mStructType->getPointerTo());
  Value* index[1];
  index[0] = ConstantInt::get(Type::getInt32Ty(llvmContext), 1);
  Value* sizePointer = IRWriter::createGetElementPtrInst(context, nullPointer, index);
  
  return IRWriter::newPtrToIntInst(context, sizePointer, Type::getInt64Ty(llvmContext), "");
}

Field* Model::findField(string fieldName) const {
  if (!mFields.count(fieldName)) {
    return NULL;
  }
  
  return mFields.at(fieldName);
}

vector<string> Model::getMissingFields(set<string> givenFields) const {
  vector<string> missingFields;
  
  for (map<string, Field*>::const_iterator iterator = mFields.begin();
       iterator != mFields.end();
       iterator++) {
    string fieldName = iterator->first;
    if (givenFields.find(fieldName) == givenFields.end()) {
      missingFields.push_back(fieldName);
    }
  }
  
  return missingFields;
}

Method* Model::findMethod(std::string methodName) const {
  if (!mNameToMethodMap.count(methodName)) {
    return NULL;
  }
  
  return mNameToMethodMap.at(methodName);
}

vector<Method*> Model::getMethods() const {
  return mMethods;
}

string Model::getVTableName() const {
  return getName() + ".vtable";
}

string Model::getObjectNameGlobalVariableName() const {
  return getName() + ".name";
}

string Model::getTypeTableName() const {
  return getName() + ".typetable";
}

vector<Interface*> Model::getInterfaces() const {
  return mInterfaces;
}

vector<Interface*> Model::getFlattenedInterfaceHierarchy() const {
  return mFlattenedInterfaceHierarchy;
}

bool Model::doesImplmentInterface(Interface* interface) const {
  // TODO: optimize this
  return getInterfaceIndex(interface) >= 0;
}

void Model::addInterfaceAndItsParents(vector<Interface*>& result, Interface* interface) const {
  result.push_back(interface);
  vector<Interface*> parentInterfaces = interface->getParentInterfaces();
  for (Interface* interface : parentInterfaces) {
    addInterfaceAndItsParents(result, interface);
  }
}

string Model::getName() const {
  return mName;
}

string Model::getShortName() const {
  return mName.substr(mName.find_last_of('.') + 1);
}

llvm::Type* Model::getLLVMType(LLVMContext& llvmContext) const {
  return mStructType->getPointerTo();
}

TypeKind Model::getTypeKind() const {
  return MODEL_TYPE;
}

bool Model::canCastTo(IType* toType) const {
  if (toType == this) {
    return true;
  }
  if (toType->getTypeKind() == INTERFACE_TYPE && getInterfaceIndex((Interface*) toType) >= 0) {
    return true;
  }
  return false;
}

bool Model::canAutoCastTo(IType* toType) const {
  return canCastTo(toType);
}

Value* Model::castTo(IRGenerationContext& context, Value* fromValue, IType* toType) const {
  if (toType == this) {
    return fromValue;
  }
  if (!canCastTo(toType)) {
    Cast::exitIncopatibleTypes(this, toType);
    return NULL;
  }
  LLVMContext& llvmContext = context.getLLVMContext();
  Interface* interface = (Interface*) toType;
  int interfaceIndex = getInterfaceIndex(interface);
  if (interfaceIndex == 0) {
    return IRWriter::newBitCastInst(context, fromValue, interface->getLLVMType(llvmContext));
  }
  
  Type* int8Type = Type::getInt8Ty(llvmContext);
  BitCastInst* bitcast = IRWriter::newBitCastInst(context, fromValue, int8Type->getPointerTo());
  Value* index[1];
  unsigned int thunkBy = interfaceIndex * Environment::getAddressSizeInBytes();
  index[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), thunkBy);
  Value* thunk = IRWriter::createGetElementPtrInst(context, bitcast, index);
  return IRWriter::newBitCastInst(context, thunk, interface->getLLVMType(llvmContext));
}

int Model::getInterfaceIndex(Interface* interface) const {
  int index = 0;
  for (Interface* implementedInterface : mFlattenedInterfaceHierarchy) {
    if (implementedInterface == interface) {
      return index;
    }
    index++;
  }
  return -1;
}

Instruction* Model::build(IRGenerationContext& context,
                          ModelBuilderArgumentList* modelBuilderArgumentList) const {
  checkArguments(modelBuilderArgumentList);
  Instruction* malloc = createMalloc(context);
  initializeFields(context, modelBuilderArgumentList, malloc);
  initializeVTable(context, modelBuilderArgumentList, malloc);
  
  return malloc;
}

void Model::createRTTI(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8PointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
 
  Constant* modelNamePointer = IObjectType::getObjectNamePointer(this, context);

  Constant* cxxabiv117ClassType =
    context.getModule()->getOrInsertGlobal("_ZTVN10__cxxabiv117__class_type_infoE",
                                           int8PointerType);
  Value* Idx[1];
  Idx[0] = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 2);
  Type* elementType = cxxabiv117ClassType->getType()->getPointerElementType();
  Constant* cxxabiv117ClassTypeElement =
    ConstantExpr::getGetElementPtr(elementType, cxxabiv117ClassType, Idx);
  Constant* cxxabiv117ClassTypeElementBitcast =
    ConstantExpr::getTruncOrBitCast(cxxabiv117ClassTypeElement, int8PointerType);
  
  vector<Constant*> rttiArray;
  vector<Type*> types;
  rttiArray.push_back(cxxabiv117ClassTypeElementBitcast);
  rttiArray.push_back(modelNamePointer);
  types.push_back(int8PointerType);
  types.push_back(int8PointerType);
  
  StructType* rttiGlobalType = StructType::get(llvmContext, types);
  Constant* rttiGlobalConstantStruct = ConstantStruct::get(rttiGlobalType, rttiArray);

  new GlobalVariable(*context.getModule(),
                     rttiGlobalType,
                     true,
                     GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                     rttiGlobalConstantStruct,
                     getRTTIVariableName());
}

void Model::checkArguments(ModelBuilderArgumentList* modelBuilderArgumentList) const {
  checkArgumentsAreWellFormed(modelBuilderArgumentList);
  checkAllFieldsAreSet(modelBuilderArgumentList);
}

void Model::checkArgumentsAreWellFormed(ModelBuilderArgumentList* modelBuilderArgumentList) const {
  bool areArgumentsWellFormed = true;
  
  for (ModelBuilderArgument* argument : *modelBuilderArgumentList) {
    areArgumentsWellFormed &= argument->checkArgument(this);
  }
  
  if (!areArgumentsWellFormed) {
    Log::e("Some arguments for the model " + mName + " builder are not well formed");
    exit(1);
  }
}

void Model::checkAllFieldsAreSet(ModelBuilderArgumentList* modelBuilderArgumentList) const {
  set<string> allFieldsThatAreSet;
  for (ModelBuilderArgument* argument : *modelBuilderArgumentList) {
    allFieldsThatAreSet.insert(argument->deriveFieldName());
  }
  
  vector<string> missingFields = getMissingFields(allFieldsThatAreSet);
  if (missingFields.size() == 0) {
    return;
  }
  
  for (string missingField : missingFields) {
    Log::e("Field " + missingField + " is not initialized");
  }
  Log::e("Some fields of the model " + mName + " are not initialized.");
  exit(1);
}

Instruction* Model::createMalloc(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Type* structType = getLLVMType(llvmContext)->getPointerElementType();
  Constant* allocSize = ConstantExpr::getSizeOf(structType);
  Instruction* malloc = IRWriter::createMalloc(context, structType, allocSize, "buildervar");
  
  return malloc;
}

void Model::initializeFields(IRGenerationContext& context,
                             ModelBuilderArgumentList* modelBuilderArgumentList,
                             Instruction* malloc) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Value* index[2];
  index[0] = Constant::getNullValue(Type::getInt32Ty(llvmContext));
  for (ModelBuilderArgument* argument : *modelBuilderArgumentList) {
    string argumentName = argument->deriveFieldName();
    Value* argumentValue = argument->getValue(context);
    IType* argumentType = argument->getType(context);
    Field* field = findField(argumentName);
    index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), field->getIndex());
    GetElementPtrInst* fieldPointer = IRWriter::createGetElementPtrInst(context, malloc, index);
    if (field->getType() != argumentType) {
      Log::e("Model builder argument value for field " + argumentName +
             " does not match its type");
      exit(1);
    }
    IRWriter::newStoreInst(context, argumentValue, fieldPointer);
  }
}

void Model::initializeVTable(IRGenerationContext& context,
                             ModelBuilderArgumentList* modelBuilderArgumentList,
                             Instruction* malloc) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  GlobalVariable* vTableGlobal = context.getModule()->getGlobalVariable(getVTableName());
  
  Type* genericPointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  Type* functionType = FunctionType::get(Type::getInt32Ty(llvmContext), true);
  Type* vTableType = functionType->getPointerTo()->getPointerTo();
  
  vector<Interface*> interfaces = getFlattenedInterfaceHierarchy();
  for (unsigned int interfaceIndex = 0; interfaceIndex < interfaces.size(); interfaceIndex++) {
    Value* vTableStart;
    if (interfaceIndex == 0) {
      vTableStart = malloc;
    } else {
      Value* vTableStartCalculation = IRWriter::newBitCastInst(context, malloc, genericPointerType);
      Value* index[1];
      unsigned int thunkBy = interfaceIndex * Environment::getAddressSizeInBytes();
      index[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), thunkBy);
      vTableStart = IRWriter::createGetElementPtrInst(context, vTableStartCalculation, index);
    }
    
    Value* vTablePointer =
    IRWriter::newBitCastInst(context, vTableStart, vTableType->getPointerTo());
    Value* index[3];
    index[0] = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
    index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), interfaceIndex);
    index[2] = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
    Value* initializerStart = IRWriter::createGetElementPtrInst(context, vTableGlobal, index);
    BitCastInst* bitcast = IRWriter::newBitCastInst(context, initializerStart, vTableType);
    IRWriter::newStoreInst(context, bitcast, vTablePointer);
  }
}

string Model::getRTTIVariableName() const {
  return getName() + ".rtti";
}
