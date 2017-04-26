//
//  ThrowStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 4/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "yazyk/IntrinsicFunctions.hpp"
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
  BasicBlock* basicBlock = context.getBasicBlock();
  context.getScopes().getScope()->addException(expressionType);

  Model* model = dynamic_cast<Model*>(expressionType);
  GlobalVariable* rtti = context.getModule()->getGlobalVariable(model->getRTTIVariableName());

  PointerType* int8PointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  Value* expressionValue = mExpression.generateIR(context);
  BitCastInst* expressionValueBitcast = new BitCastInst(expressionValue,
                                                        int8PointerType,
                                                        "",
                                                        basicBlock);
  BitCastInst* rttiBitcast = new BitCastInst(rtti, int8PointerType, "", basicBlock);

  Value* modelSize = model->getSize(context);
  vector<Value*> allocateExceptionArguments;
  allocateExceptionArguments.push_back(modelSize);
  Function* allocateExceptionFunction = IntrinsicFunctions::getAllocateExceptionFunction(context);
  Value* exceptionAlloca = CallInst::Create(allocateExceptionFunction,
                                            allocateExceptionArguments,
                                            "",
                                            basicBlock);

  vector<Value*> memCopyArguments;
  memCopyArguments.push_back(exceptionAlloca);
  memCopyArguments.push_back(expressionValueBitcast);
  memCopyArguments.push_back(modelSize);
  memCopyArguments.push_back(ConstantInt::get(Type::getInt32Ty(llvmContext), 8));
  memCopyArguments.push_back(ConstantInt::get(Type::getInt1Ty(llvmContext), 0));
  Function* memCopyFunction = IntrinsicFunctions::getMemCopyFunction(context);
  CallInst::Create(memCopyFunction, memCopyArguments, "", basicBlock);

  vector<Value*> throwArguments;
  throwArguments.push_back(exceptionAlloca);
  throwArguments.push_back(rttiBitcast);
  throwArguments.push_back(ConstantPointerNull::get(int8PointerType));
  ExpressionList::const_iterator it;

  Function* throwFunction = IntrinsicFunctions::getThrowFunction(context);

  CallInst* callInst = CallInst::Create(throwFunction, throwArguments, "", basicBlock);

  new UnreachableInst(llvmContext, basicBlock);

  return callInst;
}
