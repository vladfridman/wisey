//
//  Model.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/2/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "yazyk/Cast.hpp"
#include "yazyk/Environment.hpp"
#include "yazyk/Log.hpp"
#include "yazyk/Model.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/IRWriter.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Model::~Model() {
  mFields.clear();
  mMethods.clear();
}

Model::Model(string name,
             StructType* structType,
             map<string, Field*> fields,
             vector<Method*> methods,
             vector<Interface*> interfaces) {
  mName = name;
  mStructType = structType;
  mFields = fields;
  mMethods = methods;
  mInterfaces = interfaces;
  
  for (Method* method : methods) {
    mNameToMethodMap[method->getName()] = method;
  }
  mFlattenedInterfaceHierarchy = createFlattenedInterfaceHierarchy();
}

Value* Model::getSize(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  BasicBlock* basicBlock = context.getBasicBlock();
  Value* nullPointer = ConstantPointerNull::get(mStructType->getPointerTo());
  Value *Idx[1];
  Idx[0] = ConstantInt::get(Type::getInt32Ty(llvmContext), 1);
  Value* sizePointer = GetElementPtrInst::Create(mStructType, nullPointer, Idx, "", basicBlock);
  
  return new PtrToIntInst(sizePointer, Type::getInt64Ty(llvmContext), "", basicBlock);
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

string Model::getVTableName() const {
  return "model." + getName() + ".vtable";
}

string Model::getObjectNameGlobalVariableName() const {
  return "model." + getName() + ".name";
}

string Model::getTypeTableName() const {
  return "model." + getName() + ".typetable";
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

vector<Interface*> Model::createFlattenedInterfaceHierarchy() const {
  vector<Interface*> result;
  for (Interface* interface : mInterfaces) {
    addInterfaceAndItsParents(result, interface);
  }
  return result;
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
  if (toType->getTypeKind() == MODEL_TYPE) {
    return false;
  }
  if (toType->getTypeKind() == INTERFACE_TYPE &&
      getInterfaceIndex(dynamic_cast<Interface*>(toType)) >= 0) {
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
  BasicBlock* basicBlock = context.getBasicBlock();
  Interface* interface = dynamic_cast<Interface*>(toType);
  int interfaceIndex = getInterfaceIndex(interface);
  if (interfaceIndex == 0) {
    return new BitCastInst(fromValue, interface->getLLVMType(llvmContext), "", basicBlock);
  }
  
  Type* int8Type = Type::getInt8Ty(llvmContext);
  BitCastInst* bitcast = new BitCastInst(fromValue, int8Type->getPointerTo(), "", basicBlock);
  Value *Idx[1];
  unsigned int thunkBy = interfaceIndex * Environment::getAddressSizeInBytes();
  Idx[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), thunkBy);
  Value* thunk = GetElementPtrInst::Create(int8Type, bitcast, Idx, "add.ptr", basicBlock);
  return new BitCastInst(thunk, interface->getLLVMType(llvmContext), "", basicBlock);
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
 
  Constant* modelNamePointer =
    IObjectWithMethodsType::getObjectNamePointer(this, context);

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
    Log::e("Some arguments for the model '" + mName + "' builder are not well formed");
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
    Log::e("Field '" + missingField + "' is not initialized");
  }
  Log::e("Some fields of the model '" + mName + "' are not initialized.");
  exit(1);
}

Instruction* Model::createMalloc(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Type* structType = getLLVMType(llvmContext)->getPointerElementType();
  Constant* allocSize = ConstantExpr::getSizeOf(structType);
  allocSize = ConstantExpr::getTruncOrBitCast(allocSize, Type::getInt32Ty(llvmContext));
  Instruction* malloc = IRWriter::createMalloc(context, structType, allocSize, "buildervar");
  
  return malloc;
}

void Model::initializeFields(IRGenerationContext& context,
                             ModelBuilderArgumentList* modelBuilderArgumentList,
                             Instruction* malloc) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* structType = getLLVMType(llvmContext)->getPointerElementType();
  
  Value *Idx[2];
  Idx[0] = Constant::getNullValue(Type::getInt32Ty(llvmContext));
  for (ModelBuilderArgument* argument : *modelBuilderArgumentList) {
    string argumentName = argument->deriveFieldName();
    Value* argumentValue = argument->getValue(context);
    IType* argumentType = argument->getType(context);
    Field* field = findField(argumentName);
    Idx[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), field->getIndex());
    GetElementPtrInst* fieldPointer =
    GetElementPtrInst::Create(structType, malloc, Idx, "", context.getBasicBlock());
    if (field->getType() != argumentType) {
      Log::e("Model builder argument value for field '" + argumentName +
             "' does not match its type");
      exit(1);
    }
    new StoreInst(argumentValue, fieldPointer, context.getBasicBlock());
  }
}

void Model::initializeVTable(IRGenerationContext& context,
                             ModelBuilderArgumentList* modelBuilderArgumentList,
                             Instruction* malloc) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  BasicBlock* basicBlock = context.getBasicBlock();
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
      Value* vTableStartCalculation = new BitCastInst(malloc, genericPointerType, "", basicBlock);
      Value *Idx[1];
      unsigned int thunkBy = interfaceIndex * Environment::getAddressSizeInBytes();
      Idx[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), thunkBy);
      vTableStart = GetElementPtrInst::Create(genericPointerType->getPointerElementType(),
                                              vTableStartCalculation,
                                              Idx,
                                              "",
                                              basicBlock);
    }
    
    Value* vTablePointer = new BitCastInst(vTableStart, vTableType->getPointerTo(), "", basicBlock);
    Value *Idx[3];
    Idx[0] = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
    Idx[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), interfaceIndex);
    Idx[2] = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
    Value* initializerStart = GetElementPtrInst::Create(vTableGlobal->getType()->
                                                        getPointerElementType(),
                                                        vTableGlobal,
                                                        Idx,
                                                        "",
                                                        basicBlock);
    BitCastInst* bitcast = new BitCastInst(initializerStart, vTableType, "", basicBlock);
    new StoreInst(bitcast, vTablePointer, basicBlock);
  }
}

string Model::getRTTIVariableName() const {
  return "model." + getName() + ".rtti";
}
