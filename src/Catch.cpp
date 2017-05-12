//
//  Catch.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 4/26/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "yazyk/Catch.hpp"
#include "yazyk/Environment.hpp"
#include "yazyk/IntrinsicFunctions.hpp"
#include "yazyk/IRWriter.hpp"
#include "yazyk/LocalHeapVariable.hpp"

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
  LLVMContext& llvmContext = context.getLLVMContext();
  
  BasicBlock* exceptionContinueBlock = context.getScopes().getExceptionContinueBlock();
  Function* beginCatchFunction = IntrinsicFunctions::getBeginCatchFunction(context);
  Function* endCatchFunction = IntrinsicFunctions::getEndCatchFunction(context);

  context.getScopes().pushScope();
  context.setBasicBlock(catchBlock);
  
  Model* exceptionType = getType(context);
  Type* exceptionLLVMType = exceptionType->getLLVMType(llvmContext);
  Type* exceptionStructLLVMType = ((PointerType*) exceptionLLVMType)->getPointerElementType();
  vector<Value*> arguments;
  arguments.push_back(wrappedException);
  CallInst* exceptionPointer = IRWriter::createCallInst(context, beginCatchFunction, arguments, "");
  
  Constant* allocSize = ConstantExpr::getSizeOf(exceptionStructLLVMType);
  Instruction* malloc = IRWriter::createMalloc(context, exceptionStructLLVMType, allocSize, "");
  Type* int8PointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  BitCastInst* mallocBitcast = IRWriter::newBitCastInst(context, malloc, int8PointerType);
  
  vector<Value*> memCopyArguments;
  unsigned int memoryAlignment = Environment::getDefaultMemoryAllignment();
  memCopyArguments.push_back(mallocBitcast);
  memCopyArguments.push_back(exceptionPointer);
  memCopyArguments.push_back(allocSize);
  memCopyArguments.push_back(ConstantInt::get(Type::getInt32Ty(llvmContext), memoryAlignment));
  memCopyArguments.push_back(ConstantInt::get(Type::getInt1Ty(llvmContext), 0));
  Function* memCopyFunction = IntrinsicFunctions::getMemCopyFunction(context);
  IRWriter::createCallInst(context, memCopyFunction, memCopyArguments, "");

  vector<Value*> endCatchArguments;
  IRWriter::createCallInst(context, endCatchFunction, endCatchArguments, "");
  
  IVariable* exceptionVariable = new LocalHeapVariable(mIdentifier, exceptionType, malloc);
  context.getScopes().getScope()->setVariable(mIdentifier, exceptionVariable);
  mStatement.generateIR(context);
  context.getScopes().popScope(context);
  
  IRWriter::createBranch(context, exceptionContinueBlock);
}
