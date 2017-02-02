//
//  ModelBuilder.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <set>
#include <llvm/IR/Constants.h>

#include "yazyk/Log.hpp"
#include "yazyk/Model.hpp"
#include "yazyk/ModelBuilder.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Value* ModelBuilder::generateIR(IRGenerationContext& context) const {
  
  Model* model = context.getModel(mModelTypeSpecifier.getName());
  checkArguments(model);
  
  Type* pointerType = Type::getInt32Ty(context.getLLVMContext());
  Type* structType = mModelTypeSpecifier.getType(context);
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

  LLVMContext& llvmContext = context.getLLVMContext();
  Value *Idx[2];
  Idx[0] = Constant::getNullValue(Type::getInt32Ty(llvmContext));
  for (vector<ModelBuilderArgument*>::iterator iterator = mModelBuilderArgumentList->begin();
       iterator != mModelBuilderArgumentList->end();
       iterator++) {
    ModelBuilderArgument* argument = *iterator;
    string fieldName = argument->deriveFieldName();
    Value* fieldValue = argument->getValue(context);
    ModelField* modelField = model->findField(fieldName);
    Idx[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), modelField->getIndex());
    GetElementPtrInst* fieldPointer =
      GetElementPtrInst::Create(structType, malloc, Idx, "", context.getBasicBlock());
    if (modelField->getType() != fieldValue->getType()) {
      Log::e("Model builder argumet value for field '" + fieldName + "' does not match its type");
      exit(1);
    }
    new StoreInst(fieldValue, fieldPointer, context.getBasicBlock());
  }
  
  context.getScopes().setHeapVariable(malloc->getName(), malloc);

  return malloc;
}

void ModelBuilder::checkArguments(Model* model) const {
  checkArgumentsAreWellFormed(model);
  checkAllFieldsAreSet(model);
}

void ModelBuilder::checkArgumentsAreWellFormed(Model* model) const {
  bool areArgumentsWellFormed = true;
  
  for (vector<ModelBuilderArgument*>::iterator iterator = mModelBuilderArgumentList->begin();
       iterator != mModelBuilderArgumentList->end();
       iterator++) {
    ModelBuilderArgument* argument = *iterator;
    areArgumentsWellFormed &= argument->checkArgument(model);
  }
  
  if (!areArgumentsWellFormed) {
    Log::e("Some arguments for the model '" + model->getStructType()->getName().str() +
           "' builder are not well formed");
    exit(1);
  }
}

void ModelBuilder::checkAllFieldsAreSet(Model* model) const {
  bool areAllFieldsInitialized = true;

  set<string> allFieldsThatAreSet;
  for (vector<ModelBuilderArgument*>::iterator iterator = mModelBuilderArgumentList->begin();
       iterator != mModelBuilderArgumentList->end();
       iterator++) {
    ModelBuilderArgument* argument = *iterator;
    allFieldsThatAreSet.insert(argument->deriveFieldName());
  }
  
  for (map<string, ModelField*>::iterator iterator = model->getFields()->begin();
       iterator != model->getFields()->end();
       iterator++) {
    string modelFieldName = iterator->first;
    if (allFieldsThatAreSet.find(modelFieldName) == allFieldsThatAreSet.end()) {
      Log::e("Field '" + modelFieldName + "' is not initialized");
      areAllFieldsInitialized = false;
    }
  }

  if (!areAllFieldsInitialized) {
    Log::e("Some fields of the model '" + model->getStructType()->getName().str()
           + "' are not initialized.");
    exit(1);
  }
}
