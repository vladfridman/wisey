//
//  ModelBuilder.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <set>
#include <llvm/IR/Constants.h>

#include "yazyk/LocalHeapVariable.hpp"
#include "yazyk/Log.hpp"
#include "yazyk/Model.hpp"
#include "yazyk/ModelBuilder.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Value* ModelBuilder::generateIR(IRGenerationContext& context) const {
  
  Model* model = context.getModel(mModelTypeSpecifier.getName());
  checkArguments(model);
  Instruction* malloc = createMalloc(context, model);
  initializeFields(context, model, malloc);
  initializeVTable(context, model, malloc);
  
  return malloc;
}

Instruction* ModelBuilder::createMalloc(IRGenerationContext& context, Model* model) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Type* pointerType = Type::getInt32Ty(llvmContext);
  IType* yazykType = mModelTypeSpecifier.getType(context);
  Type* structType = yazykType->getLLVMType(llvmContext)->getPointerElementType();
  Constant* allocSize = ConstantExpr::getSizeOf(structType);
  allocSize = ConstantExpr::getTruncOrBitCast(allocSize, pointerType);
  Instruction* malloc = CallInst::CreateMalloc(context.getBasicBlock(),
                                               pointerType,
                                               structType,
                                               allocSize,
                                               nullptr,
                                               nullptr,
                                               "buildervar");
  context.getBasicBlock()->getInstList().push_back(malloc);

  return malloc;
}

void ModelBuilder::initializeVTable(IRGenerationContext& context,
                                    Model* model,
                                    Instruction* malloc) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  BasicBlock* basicBlock = context.getBasicBlock();
  GlobalVariable* vTableGlobal = context.getModule()->getGlobalVariable(model->getVTableName());

  Type* genericPointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  Type* functionType = FunctionType::get(Type::getInt32Ty(llvmContext), true);
  Type* vTableType = functionType->getPointerTo()->getPointerTo();
  
  vector<Interface*> interfaces = model->getInterfaces();
  for (unsigned int interfaceIndex = 0; interfaceIndex < interfaces.size(); interfaceIndex++) {
    Value* vTableStart;
    if (interfaceIndex == 0) {
      vTableStart = malloc;
    } else {
      Value* vTableStartCalculation = new BitCastInst(malloc, genericPointerType, "", basicBlock);
      Value *Idx[1];
      Idx[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), 8 * interfaceIndex);
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
    interfaceIndex++;
  }
}

void ModelBuilder::initializeFields(IRGenerationContext& context,
                                    Model* model,
                                    Instruction* malloc) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  IType* yazykType = mModelTypeSpecifier.getType(context);
  Type* structType = yazykType->getLLVMType(llvmContext)->getPointerElementType();

  Value *Idx[2];
  Idx[0] = Constant::getNullValue(Type::getInt32Ty(llvmContext));
  for (ModelBuilderArgument* argument : *mModelBuilderArgumentList) {
    string fieldName = argument->deriveFieldName();
    Value* fieldValue = argument->getValue(context);
    ModelField* modelField = model->findField(fieldName);
    Idx[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), modelField->getIndex());
    GetElementPtrInst* fieldPointer =
    GetElementPtrInst::Create(structType, malloc, Idx, "", context.getBasicBlock());
    if (modelField->getType()->getLLVMType(llvmContext) != fieldValue->getType()) {
      Log::e("Model builder argumet value for field '" + fieldName + "' does not match its type");
      exit(1);
    }
    new StoreInst(fieldValue, fieldPointer, context.getBasicBlock());
  }
  
  LocalHeapVariable* heapVariable = new LocalHeapVariable(malloc->getName(), model, malloc);
  context.getScopes().setVariable(heapVariable);
}

void ModelBuilder::checkArguments(Model* model) const {
  checkArgumentsAreWellFormed(model);
  checkAllFieldsAreSet(model);
}

void ModelBuilder::checkArgumentsAreWellFormed(Model* model) const {
  bool areArgumentsWellFormed = true;
  
  for (ModelBuilderArgument* argument : *mModelBuilderArgumentList) {
    areArgumentsWellFormed &= argument->checkArgument(model);
  }
  
  if (!areArgumentsWellFormed) {
    Log::e("Some arguments for the model '" + model->getName() + "' builder are not well formed");
    exit(1);
  }
}

void ModelBuilder::checkAllFieldsAreSet(Model* model) const {
  set<string> allFieldsThatAreSet;
  for (ModelBuilderArgument* argument : *mModelBuilderArgumentList) {
    allFieldsThatAreSet.insert(argument->deriveFieldName());
  }
  
  vector<string> missingFields = model->getMissingFields(allFieldsThatAreSet);
  if (missingFields.size() == 0) {
    return;
  }
  
  for (string missingField : missingFields) {
    Log::e("Field '" + missingField + "' is not initialized");
  }
  Log::e("Some fields of the model '" + model->getName() + "' are not initialized.");
  exit(1);
}

IType* ModelBuilder::getType(IRGenerationContext& context) const {
  return mModelTypeSpecifier.getType(context);
}
