//
//  TryCatchInfo.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 7/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "wisey/Catch.hpp"
#include "wisey/Cleanup.hpp"
#include "wisey/FinallyBlock.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/ModelOwner.hpp"
#include "wisey/TryCatchInfo.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

TryCatchInfo::~TryCatchInfo() {
  mComposingCallbacks.clear();
}

FinallyBlock* TryCatchInfo::getFinallyBlock() {
  return mFinallyBlock;
}

vector<Catch*> TryCatchInfo::getCatchList() {
  return mCatchList;
}

BasicBlock* TryCatchInfo::defineLandingPadBlock(IRGenerationContext& context) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Function* function = context.getBasicBlock()->getParent();
  if (!function->hasPersonalityFn()) {
    function->setPersonalityFn(IntrinsicFunctions::getPersonalityFunction(context));
  }
  
  vector<Catch*> allCatches = context.getScopes().mergeNestedCatchLists(context, mCatchList);

  for (Catch* catchClause : mCatchList) {
    context.getScopes().getScope()->removeException(catchClause->getType(context)->getObject());
  }

  BasicBlock* landingPadBlock = BasicBlock::Create(llvmContext, "eh.landing.pad", function);
  
  mComposingCallbacks.push_back(make_tuple(composeLandingPadBlock, landingPadBlock));
  
  return landingPadBlock;
}

bool TryCatchInfo::runComposingCallbacks(IRGenerationContext& context) {
  BasicBlock* lastBasicBlock = context.getBasicBlock();
  
  vector<Catch*> allCatches = context.getScopes().mergeNestedCatchLists(context, mCatchList);
  bool result = true;

  for (tuple<LandingPadComposingFunction, BasicBlock*> callbackTuple : mComposingCallbacks) {
    LandingPadComposingFunction function = get<0>(callbackTuple);
    BasicBlock* landingPadBlock = get<1>(callbackTuple);
    
    result &= function(context, landingPadBlock, allCatches, mFinallyBlock, mContinueBlock);
  }
  
  context.setBasicBlock(lastBasicBlock);
  return result;
}

bool TryCatchInfo::composeLandingPadBlock(IRGenerationContext& context,
                                          BasicBlock* landingPadBlock,
                                          vector<Catch*> allCatches,
                                          FinallyBlock* finallyBlock,
                                          BasicBlock* continueBlock) {
  if (allCatches.size() == 0) {
    return composeFinallyOnlyLandingPad(context, landingPadBlock, finallyBlock);
  }
  tuple<LandingPadInst*, Value*, Value*>
  landingPadIR = generateLandingPad(context, landingPadBlock, allCatches);
  LandingPadInst* landingPadInst = get<0>(landingPadIR);
  Value* wrappedException = get<1>(landingPadIR);
  Value* exceptionTypeId = get<2>(landingPadIR);
  
  vector<tuple<Catch*, BasicBlock*>> catchesAndBlocks =
  generateSelectCatchByExceptionType(context, exceptionTypeId, allCatches, landingPadBlock);
  generateResumeAndFail(context,
                        landingPadInst,
                        exceptionTypeId,
                        wrappedException,
                        finallyBlock);
  return generateCatches(context,
                         wrappedException,
                         catchesAndBlocks,
                         continueBlock,
                         finallyBlock);
}

bool TryCatchInfo::composeFinallyOnlyLandingPad(IRGenerationContext& context,
                                                BasicBlock* landingPadBlock,
                                                FinallyBlock* finallyBlock) {
  context.setBasicBlock(landingPadBlock);
  
  LLVMContext& llvmContext = context.getLLVMContext();
  
  Type* int8PointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  vector<Type*> landingPadReturnTypes;
  landingPadReturnTypes.push_back(int8PointerType);
  landingPadReturnTypes.push_back(Type::getInt32Ty(llvmContext));
  StructType* landingPadReturnType = StructType::get(llvmContext, landingPadReturnTypes);
  LandingPadInst* landingPad = LandingPadInst::Create(landingPadReturnType,
                                                      0,
                                                      "",
                                                      context.getBasicBlock());
  
  landingPad->setCleanup(true);
  finallyBlock->generateIR(context);

  context.getScopes().freeOwnedMemory(context);
  
  IRWriter::createResumeInst(context, landingPad);

  return true;
}

tuple<LandingPadInst*, Value*, Value*>
TryCatchInfo::generateLandingPad(IRGenerationContext& context,
                                 BasicBlock* landingPadBlock,
                                 vector<Catch*> allCatches) {
  LLVMContext& llvmContext = context.getLLVMContext();
  context.setBasicBlock(landingPadBlock);
  Type* int8PointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  vector<Type*> landingPadReturnTypes;
  landingPadReturnTypes.push_back(int8PointerType);
  landingPadReturnTypes.push_back(Type::getInt32Ty(llvmContext));
  StructType* landingPadReturnType = StructType::get(llvmContext, landingPadReturnTypes);
  LandingPadInst* landingPad = LandingPadInst::Create(landingPadReturnType,
                                                      (int) allCatches.size(),
                                                      "",
                                                      landingPadBlock);
  
  for (Catch* catchClause : allCatches) {
    ModelOwner* exceptionType = catchClause->getType(context);
    landingPad->addClause(context.getModule()->
                          getNamedGlobal(exceptionType->getObject()->getRTTIVariableName()));
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

void TryCatchInfo::generateResumeAndFail(IRGenerationContext& context,
                                              LandingPadInst* landingPadInst,
                                              Value* exceptionTypeId,
                                              Value* wrappedException,
                                              FinallyBlock* finallyBlock) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Function* function = context.getBasicBlock()->getParent();
  llvm::Constant* zero = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  ICmpInst* compare = IRWriter::newICmpInst(context, ICmpInst::ICMP_SLT, exceptionTypeId, zero, "");
  BasicBlock* unexpectedBlock = BasicBlock::Create(llvmContext, "unexpected", function);
  BasicBlock* resumeBlock = BasicBlock::Create(llvmContext, "resume", function);
  IRWriter::createConditionalBranch(context, unexpectedBlock, resumeBlock, compare);
  
  Function* unexpectedFunction = IntrinsicFunctions::getUnexpectedFunction(context);
  vector<Value*> arguments;
  arguments.push_back(wrappedException);
  context.setBasicBlock(unexpectedBlock);

  finallyBlock->generateIR(context);

  IRWriter::createCallInst(context, unexpectedFunction, arguments, "");
  IRWriter::newUnreachableInst(context);
  
  context.setBasicBlock(resumeBlock);
  finallyBlock->generateIR(context);
  
  IRWriter::createResumeInst(context, landingPadInst);
}

vector<tuple<Catch*, BasicBlock*>>
TryCatchInfo::generateSelectCatchByExceptionType(IRGenerationContext& context,
                                                 Value* exceptionTypeId,
                                                 vector<Catch*> allCatches,
                                                 BasicBlock* landingPadBlock) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8PointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  Function* typeIdFunction = IntrinsicFunctions::getTypeIdFunction(context);
  Function* function = context.getBasicBlock()->getParent();
  
  vector<tuple<Catch*, BasicBlock*>> catchesAndBlocks;
  BasicBlock* currentBlock = landingPadBlock;
  
  for (Catch* catchClause : allCatches) {
    context.setBasicBlock(currentBlock);
    ModelOwner* exceptionType = catchClause->getType(context);
    Value* rtti = context.getModule()->
    getNamedGlobal(exceptionType->getObject()->getRTTIVariableName());
    Value* rttiBitcast = IRWriter::newBitCastInst(context, rtti, int8PointerType);
    vector<Value*> arguments;
    arguments.push_back(rttiBitcast);
    CallInst* typeId = IRWriter::createCallInst(context, typeIdFunction, arguments, "");
    typeId->setTailCall();
    ICmpInst* compare =
    IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, exceptionTypeId, typeId, "");
    string catchBlockName = "catch." + exceptionType->getObject()->getName();
    BasicBlock* catchBlock = BasicBlock::Create(llvmContext, catchBlockName, function);
    catchesAndBlocks.push_back(make_tuple(catchClause, catchBlock));
    string nextBlockName = "not." + exceptionType->getObject()->getName();
    BasicBlock* nextBlock = BasicBlock::Create(llvmContext, nextBlockName, function);
    IRWriter::createConditionalBranch(context, catchBlock, nextBlock, compare);
    currentBlock = nextBlock;
  }
  
  context.setBasicBlock(currentBlock);
  
  return catchesAndBlocks;
}

bool TryCatchInfo::generateCatches(IRGenerationContext& context,
                                   Value* wrappedException,
                                   vector<tuple<Catch*, BasicBlock*>> catchesAndBlocks,
                                   BasicBlock* exceptionContinueBlock,
                                   FinallyBlock* finallyBlock) {
  bool doAllCatchesTerminate = true;
  for (tuple<Catch*, BasicBlock*> catchAndBlock : catchesAndBlocks) {
    context.getScopes().setFinallyBlock(finallyBlock);
    Catch* catchClause = get<0>(catchAndBlock);
    BasicBlock* catchBlock = get<1>(catchAndBlock);
    doAllCatchesTerminate &= catchClause->generateIR(context,
                                                     wrappedException,
                                                     catchBlock,
                                                     exceptionContinueBlock,
                                                     finallyBlock);
  }
  
  return doAllCatchesTerminate;
}

