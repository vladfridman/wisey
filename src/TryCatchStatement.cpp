//
//  TryCatchStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/24/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/TypeBuilder.h>

#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalStackVariable.hpp"
#include "wisey/TryCatchStatement.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

void TryCatchStatement::prototype(IRGenerationContext& context) const {
}

Value* TryCatchStatement::generateIR(IRGenerationContext& context) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Function* function = context.getBasicBlock()->getParent();
  if (!function->hasPersonalityFn()) {
    function->setPersonalityFn(IntrinsicFunctions::getPersonalityFunction(context));
  }
  BasicBlock* landingPadBlock = BasicBlock::Create(llvmContext, "eh.landing.pad", function);
  BasicBlock* continueBlock = BasicBlock::Create(llvmContext, "eh.continue", function);
  context.getScopes().setLandingPadBlock(landingPadBlock);
  context.getScopes().setExceptionContinueBlock(continueBlock);
  context.getScopes().setExceptionFinally(mFinally);

  mTryBlock.generateIR(context);
  
  context.getScopes().getExceptionFinally()->generateIR(context);

  IRWriter::createBranch(context, continueBlock);

  tuple<LandingPadInst*, Value*, Value*>
  landingPadIR = generateLandingPad(context, landingPadBlock);
  LandingPadInst* landingPadInst = get<0>(landingPadIR);
  Value* wrappedException = get<1>(landingPadIR);
  Value* exceptionTypeId = get<2>(landingPadIR);
  
  vector<tuple<Catch*, BasicBlock*>>
  catchesAndBlocks = generateSelectCatchByExceptionType(context, landingPadBlock, exceptionTypeId);
  generateResumeAndFail(context, landingPadInst, exceptionTypeId, wrappedException);
  generateCatches(context, wrappedException, catchesAndBlocks);
  
  context.getScopes().setLandingPadBlock(NULL);
  context.getScopes().setExceptionContinueBlock(NULL);
  context.getScopes().setExceptionFinally(NULL);
  context.setBasicBlock(continueBlock);
  
  return NULL;
}

tuple<LandingPadInst*, Value*, Value*>
TryCatchStatement::generateLandingPad(IRGenerationContext& context,
                                      BasicBlock* landingPadBlock) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  context.setBasicBlock(landingPadBlock);
  Type* int8PointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  vector<Type*> landingPadReturnTypes;
  landingPadReturnTypes.push_back(int8PointerType);
  landingPadReturnTypes.push_back(Type::getInt32Ty(llvmContext));
  StructType* landingPadReturnType = StructType::get(llvmContext, landingPadReturnTypes);
  LandingPadInst* landingPad = LandingPadInst::Create(landingPadReturnType,
                                                      (int) mCatchList.size(),
                                                      "",
                                                      landingPadBlock);
  
  for (Catch* catchClause : mCatchList) {
    Model* exceptionType = catchClause->getType(context);
    landingPad->addClause(context.getModule()->
                          getGlobalVariable(exceptionType->getRTTIVariableName()));
    context.getScopes().getScope()->removeException(exceptionType);
  }
  
  if (mCatchList.size() == 0) {
    landingPad->setCleanup(true);
  }
  
  Value* landingPadReturnValueAlloca = IRWriter::newAllocaInst(context, landingPadReturnType, "");
  IRWriter::newStoreInst(context, landingPad, landingPadReturnValueAlloca);
  
  Value* index[2];
  index[0] = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  Value* wrappedExceptionPointer =
  IRWriter::createGetElementPtrInst(context, landingPadReturnValueAlloca, index);
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), 1);
  Value* exceptionTypeIdPointer =
  IRWriter::createGetElementPtrInst(context, landingPadReturnValueAlloca, index);
  Value* wrappedException = IRWriter::newLoadInst(context, wrappedExceptionPointer, "");
  Value* exceptionTypeId = IRWriter::newLoadInst(context, exceptionTypeIdPointer, "");
  
  return make_tuple(landingPad, wrappedException, exceptionTypeId);
}

void TryCatchStatement::generateResumeAndFail(IRGenerationContext& context,
                                              LandingPadInst* landingPadInst,
                                              Value* exceptionTypeId,
                                              Value* wrappedException) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Function* function = context.getBasicBlock()->getParent();
  Constant* zero = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  ICmpInst* compare = IRWriter::newICmpInst(context, ICmpInst::ICMP_SLT, exceptionTypeId, zero, "");
  BasicBlock* unexpectedBlock = BasicBlock::Create(llvmContext, "unexpected", function);
  BasicBlock* resumeBlock = BasicBlock::Create(llvmContext, "resume", function);
  IRWriter::createConditionalBranch(context, unexpectedBlock, resumeBlock, compare);
  
  Function* unexpectedFunction = IntrinsicFunctions::getUnexpectedFunction(context);
  vector<Value*> arguments;
  arguments.push_back(wrappedException);
  context.setBasicBlock(unexpectedBlock);
  context.getScopes().getExceptionFinally()->generateIR(context);
  IRWriter::createCallInst(context, unexpectedFunction, arguments, "");
  new UnreachableInst(llvmContext, unexpectedBlock);
  
  context.setBasicBlock(resumeBlock);
  context.getScopes().getExceptionFinally()->generateIR(context);
  IRWriter::createResumeInst(context, landingPadInst);
}

vector<tuple<Catch*, BasicBlock*>>
TryCatchStatement::generateSelectCatchByExceptionType(IRGenerationContext& context,
                                                      llvm::BasicBlock* landingPadBlock,
                                                      Value* exceptionTypeId) const {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8PointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  Function* typeIdFunction = IntrinsicFunctions::getTypeIdFunction(context);
  Function* function = context.getBasicBlock()->getParent();
  
  vector<tuple<Catch*, BasicBlock*>> catchesAndBlocks;
  BasicBlock* currentBlock = landingPadBlock;
  
  for (Catch* catchClause : mCatchList) {
    context.setBasicBlock(currentBlock);
    Model* exceptionType = catchClause->getType(context);
    Value* rtti = context.getModule()->getGlobalVariable(exceptionType->getRTTIVariableName());
    Value* rttiBitcast = IRWriter::newBitCastInst(context, rtti, int8PointerType);
    vector<Value*> arguments;
    arguments.push_back(rttiBitcast);
    CallInst* typeId = IRWriter::createCallInst(context, typeIdFunction, arguments, "");
    typeId->setTailCall();
    ICmpInst* compare =
      IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, exceptionTypeId, typeId, "");
    string catchBlockName = "catch." + exceptionType->getName();
    BasicBlock* catchBlock = BasicBlock::Create(llvmContext, catchBlockName, function);
    catchesAndBlocks.push_back(make_tuple(catchClause, catchBlock));
    string nextBlockName = "not." + exceptionType->getName();
    BasicBlock* nextBlock = BasicBlock::Create(llvmContext, nextBlockName, function);
    IRWriter::createConditionalBranch(context, catchBlock, nextBlock, compare);
    currentBlock = nextBlock;
  }
  
  context.setBasicBlock(currentBlock);
  
  return catchesAndBlocks;
}

void TryCatchStatement::generateCatches(IRGenerationContext& context,
                                        Value* wrappedException,
                                        vector<tuple<Catch*, BasicBlock*>> catchesAndBlocks) const {
  for (tuple<Catch*, BasicBlock*> catchAndBlock : catchesAndBlocks) {
    Catch* catchClause = get<0>(catchAndBlock);
    BasicBlock* catchBlock = get<1>(catchAndBlock);
    catchClause->generateIR(context, wrappedException, catchBlock);
  }
}

