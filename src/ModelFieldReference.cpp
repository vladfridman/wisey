//
//  ModelFieldReference.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/8/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "yazyk/Log.hpp"
#include "yazyk/ModelFieldReference.hpp"

using namespace std;
using namespace llvm;
using namespace yazyk;

Value* ModelFieldReference::generateIR(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Value* expressionValue = mExpression.generateIR(context);
  IType* expressionType = mExpression.getType(context);
  assert(expressionType->getTypeKind() != PRIMITIVE_TYPE);
  
  Model* model = (Model*) expressionType;
  StructType* structType = (StructType*) model->getLLVMType(llvmContext)->getPointerElementType();
  
  ModelField* modelField = checkAndFindField(context);
  Value *Idx[2];
  Idx[0] = Constant::getNullValue(Type::getInt32Ty(llvmContext));
  Idx[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), modelField->getIndex());
  
  GetElementPtrInst* fieldPointer = GetElementPtrInst::Create(structType,
                                                              expressionValue,
                                                              Idx,
                                                              "",
                                                              context.getBasicBlock());

  return new LoadInst(fieldPointer, "", context.getBasicBlock());
}

IType* ModelFieldReference::getType(IRGenerationContext& context) const {
  return checkAndFindField(context)->getType();
}

ModelField* ModelFieldReference::checkAndFindField(IRGenerationContext& context) const {
  IType* expressionType = mExpression.getType(context);
  assert(expressionType->getTypeKind() != PRIMITIVE_TYPE);
  
  Model* model = (Model*) expressionType;
  ModelField* modelField = model->findField(mIdentifier);

  if (modelField != NULL) {
    return modelField;
  }
  
  Log::e("Field '" + mIdentifier + "' is not found in model '" + model->getName() + "'");
  exit(1);
}
