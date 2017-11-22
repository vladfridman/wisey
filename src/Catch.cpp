//
//  Catch.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/26/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Catch.hpp"
#include "wisey/Environment.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/ModelOwner.hpp"
#include "wisey/ParameterOwnerVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

Catch::~Catch() {
  delete mModelTypeSpecifier;
  delete mStatement;
}

ModelOwner* Catch::getType(IRGenerationContext& context) const {
  Model* model = mModelTypeSpecifier->getType(context);
  return (ModelOwner*) model->getOwner();
}

bool Catch::generateIR(IRGenerationContext& context,
                       Value* wrappedException,
                       BasicBlock* catchBlock,
                       BasicBlock* exceptionContinueBlock,
                       FinallyBlock* finallyBlock) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Function* beginCatchFunction = IntrinsicFunctions::getBeginCatchFunction(context);
  Function* endCatchFunction = IntrinsicFunctions::getEndCatchFunction(context);

  context.getScopes().pushScope();
  context.setBasicBlock(catchBlock);
  
  IObjectOwnerType* exceptionType = getType(context);
  PointerType* exceptionLLVMType = exceptionType->getLLVMType(llvmContext);
  Type* exceptionStructLLVMType = exceptionLLVMType->getPointerElementType();
  vector<Value*> arguments;
  arguments.push_back(wrappedException);
  CallInst* exceptionPointer = IRWriter::createCallInst(context, beginCatchFunction, arguments, "");
  
  llvm::Constant* allocSize = ConstantExpr::getSizeOf(exceptionStructLLVMType);
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
  
  Value* pointer = IRWriter::newAllocaInst(context, malloc->getType(), "exceptionPointer");
  IRWriter::newStoreInst(context, malloc, pointer);
  
  IVariable* exceptionVariable = new ParameterOwnerVariable(mIdentifier, exceptionType, pointer);
  context.getScopes().getScope()->setVariable(mIdentifier, exceptionVariable);
  mStatement->generateIR(context);
  context.getScopes().popScope(context);
  
  vector<Catch*> emptyCatchList;
  context.getScopes().pushScope();
  FinallyBlock subFinallyBlock;
  TryCatchInfo* cleaupTryCatchInfo = new TryCatchInfo(&subFinallyBlock, emptyCatchList, NULL);
  context.getScopes().beginTryCatch(cleaupTryCatchInfo);

  finallyBlock->generateIR(context);
  
  bool hasTerminator = context.getBasicBlock()->getTerminator() != NULL;

  context.getScopes().popScope(context);

  IRWriter::createBranch(context, exceptionContinueBlock);
  
  return hasTerminator;
}
