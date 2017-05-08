//
//  Catch.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 4/26/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/Catch.hpp"
#include "yazyk/IntrinsicFunctions.hpp"
#include "yazyk/IRWriter.hpp"
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

  context.getScopes().pushScope();
  context.setBasicBlock(catchBlock);
  
  Model* exceptionType = getType(context);
  Type* exceptionLLVMType = exceptionType->getLLVMType(context.getLLVMContext());
  Type* exceptionStructLLVMType = ((PointerType*) exceptionLLVMType)->getPointerElementType();
  vector<Value*> arguments;
  arguments.push_back(wrappedException);
  CallInst* exceptionPointer = IRWriter::createCallInst(context, beginCatchFunction, arguments, "");
  Value* exception = IRWriter::newBitCastInst(context, exceptionPointer, exceptionLLVMType);
  
  AllocaInst* expectionSpace = IRWriter::newAllocaInst(context, exceptionStructLLVMType, "");
  Value* exceptionLoaded = new LoadInst(exception, "", catchBlock);
  IRWriter::newStoreInst(context, exceptionLoaded, expectionSpace);
  AllocaInst* alloca = IRWriter::newAllocaInst(context, exceptionLLVMType, "");
  IRWriter::newStoreInst(context, expectionSpace, alloca);
  vector<Value*> endCatchArguments;
  IRWriter::createCallInst(context, endCatchFunction, endCatchArguments, "");
  
  IVariable* exceptionVariable = new LocalStackVariable(mIdentifier, exceptionType, alloca);
  context.getScopes().getScope()->setVariable(mIdentifier, exceptionVariable);
  mStatement.generateIR(context);
  context.getScopes().popScope(context);
  
  IRWriter::createBranch(context, exceptionContinueBlock);
}
