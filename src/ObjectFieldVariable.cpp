//
//  ObjectFieldVariable.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "yazyk/IExpression.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/Log.hpp"
#include "yazyk/ObjectFieldVariable.hpp"

using namespace std;
using namespace llvm;
using namespace yazyk;

string ObjectFieldVariable::getName() const {
  return mName;
}

IType* ObjectFieldVariable::getType() const {
  return mModel->findField(mName)->getType();
}

Value* ObjectFieldVariable::getValue() const {
  return mValue;
}

Value* ObjectFieldVariable::generateIdentifierIR(IRGenerationContext& context,
                                                 string llvmVariableName) const {
  IVariable* thisVariable = context.getScopes().getVariable("this");
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Value* loadedValue = new LoadInst(thisVariable->getValue(), "this", context.getBasicBlock());
  
  StructType* structType = (StructType*) mModel->getLLVMType(llvmContext)->getPointerElementType();
  
  ModelField* modelField = checkAndFindField(context);
  Value *Idx[2];
  Idx[0] = Constant::getNullValue(Type::getInt32Ty(llvmContext));
  Idx[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), modelField->getIndex());
  
  GetElementPtrInst* fieldPointer = GetElementPtrInst::Create(structType,
                                                              loadedValue,
                                                              Idx,
                                                              "",
                                                              context.getBasicBlock());
  
  return new LoadInst(fieldPointer, "", context.getBasicBlock());
}

Value* ObjectFieldVariable::generateAssignmentIR(IRGenerationContext& context,
                                                 IExpression& assignToExpression) {
  ModelField* modelField = checkAndFindField(context);
  IType* assignToType = assignToExpression.getType(context);
  if (assignToType != modelField->getType()) {
    Log::e("Can not assign to field '" + mName + "' of model '" + mModel->getName() +
           "' because of incompatable types");
    exit(1);
  }
  
  LLVMContext& llvmContext = context.getLLVMContext();
  Value *Idx[2];
  Idx[0] = Constant::getNullValue(Type::getInt32Ty(llvmContext));
  Idx[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), modelField->getIndex());
  Type* structType = mModel->getLLVMType(llvmContext)->getPointerElementType();
  IVariable* thisVariable = context.getScopes().getVariable("this");
  Value* loadedValue = new LoadInst(thisVariable->getValue(), "this", context.getBasicBlock());
  BasicBlock* basicBlock = context.getBasicBlock();
  GetElementPtrInst* fieldPointer =
    GetElementPtrInst::Create(structType, loadedValue, Idx, "", basicBlock);
  Value* assignToValue = assignToExpression.generateIR(context);
  
  return new StoreInst(assignToValue, fieldPointer, basicBlock);
}

void ObjectFieldVariable::free(BasicBlock* basicBlock) const {
  /** Not implmeneted yet */
}

ModelField* ObjectFieldVariable::checkAndFindField(IRGenerationContext& context) const {
  ModelField* modelField = mModel->findField(mName);
  
  if (modelField != NULL) {
    return modelField;
  }
  
  Log::e("Field '" + mName + "' is not found in model '" + mModel->getName() + "'");
  exit(1);
}