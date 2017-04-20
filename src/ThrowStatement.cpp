//
//  ThrowStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 4/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/TypeBuilder.h>

#include "yazyk/Log.hpp"
#include "yazyk/ThrowStatement.hpp"

using namespace std;
using namespace llvm;
using namespace yazyk;

Value* ThrowStatement::generateIR(IRGenerationContext& context) const {
  IType* expressionType = mExpression.getType(context);
  if (expressionType->getTypeKind() != MODEL_TYPE) {
    Log::e("Thrown object can only be a model");
    exit(1);
  }

  LLVMContext& llvmContext = context.getLLVMContext();
  
  Model* model = dynamic_cast<Model*>(expressionType);
  GlobalVariable* rtti = model->getOrCreateRTTI(context);
  
  PointerType* int8PointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  Value* expressionValue = mExpression.generateIR(context);
  BitCastInst* expressionValueBitcast = new BitCastInst(expressionValue,
                                                        int8PointerType,
                                                        "",
                                                        context.getBasicBlock());
  BitCastInst* rttiBitcast = new BitCastInst(rtti, int8PointerType, "", context.getBasicBlock());
  
  vector<Value*> arguments;
  arguments.push_back(expressionValueBitcast);
  arguments.push_back(rttiBitcast);
  arguments.push_back(ConstantPointerNull::get(int8PointerType));
  ExpressionList::const_iterator it;
  
  Function* throwFunction = getThrowFunction(context);
  CallInst* callInst = CallInst::Create(throwFunction, arguments, "", context.getBasicBlock());
  
  new UnreachableInst(llvmContext, context.getBasicBlock());
  
  return callInst;
}

Function* ThrowStatement::getThrowFunction(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  FunctionType *cxxThrowType = TypeBuilder<void (types::i<8>*, types::i<8>*, types::i<8>*), false>
  ::get(llvmContext);
  
  AttributeSet attributeSet = AttributeSet().addAttribute(llvmContext, 1U, Attribute::NoAlias);
  return cast<Function>(context.getModule()->getOrInsertFunction("__cxa_throw",
                                                                 cxxThrowType,
                                                                 attributeSet));
}
