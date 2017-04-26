//
//  Catch.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 4/26/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/Catch.hpp"
#include "yazyk/IntrinsicFunctions.hpp"
#include "yazyk/LocalStackVariable.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

Model* Catch::getType(IRGenerationContext& context) const {
  IType* argumentType = mTypeSpecifier.getType(context);
  assert(argumentType->getTypeKind() == MODEL_TYPE);
  
  return dynamic_cast<Model*>(argumentType);
}

void Catch::generateIR(IRGenerationContext& context,
                       Value* wrappedException,
                       BasicBlock* catchBlock) const {
  BasicBlock* exceptionContinueBlock = context.getScopes().getExceptionContinueBlock();
  Function* beginCatchFunction = IntrinsicFunctions::getBeginCatchFunction(context);
  Function* endCatchFunction = IntrinsicFunctions::getEndCatchFunction(context);

  Model* exceptionType = getType(context);
  Type* exceptionLLVMType = exceptionType->getLLVMType(context.getLLVMContext());
  Type* exceptionStructLLVMType = ((PointerType*) exceptionLLVMType)->getPointerElementType();
  vector<Value*> arguments;
  arguments.push_back(wrappedException);
  Value* exceptionPointer = CallInst::Create(beginCatchFunction, arguments, "", catchBlock);
  Value* exception = new BitCastInst(exceptionPointer, exceptionLLVMType, "", catchBlock);
  
  context.getScopes().pushScope();
  context.setBasicBlock(catchBlock);
  
  AllocaInst* expectionSpace = new AllocaInst(exceptionStructLLVMType, "", catchBlock);
  Value* exceptionLoaded = new LoadInst(exception, "", catchBlock);
  new StoreInst(exceptionLoaded, expectionSpace, catchBlock);
  AllocaInst* alloca = new AllocaInst(exceptionLLVMType, "", catchBlock);
  new StoreInst(expectionSpace, alloca, catchBlock);
  vector<Value*> endCatchArguments;
  CallInst::Create(endCatchFunction, endCatchArguments, "", catchBlock);
  
  IVariable* exceptionVariable = new LocalStackVariable(mIdentifier, exceptionType, alloca);
  context.getScopes().getScope()->setVariable(mIdentifier, exceptionVariable);
  mStatement.generateIR(context);
  context.getScopes().popScope(context);
  
  BranchInst::Create(exceptionContinueBlock, catchBlock);
}
