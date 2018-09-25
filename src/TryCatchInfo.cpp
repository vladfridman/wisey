//
//  TryCatchInfo.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 7/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/Constants.h>

#include "Catch.hpp"
#include "Cleanup.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "IntrinsicFunctions.hpp"
#include "ModelOwner.hpp"
#include "TryCatchInfo.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

TryCatchInfo::TryCatchInfo(std::vector<Catch*> catchList, llvm::BasicBlock* continueBlock) :
mCatchList(catchList),
mContinueBlock(continueBlock) { }

TryCatchInfo::~TryCatchInfo() {
  mComposingCallbacks.clear();
}

vector<Catch*> TryCatchInfo::getCatchList() {
  return mCatchList;
}

tuple<BasicBlock*, Value*> TryCatchInfo::defineLandingPadBlock(IRGenerationContext& context,
                                                               BasicBlock* freeMemoryBlock,
                                                               BasicBlock* freeMemoryEndBlock) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Function* function = context.getBasicBlock()->getParent();
  if (!function->hasPersonalityFn()) {
    function->setPersonalityFn(IntrinsicFunctions::getPersonalityFunction(context));
  }
  
  BasicBlock* landingPadBlock = BasicBlock::Create(llvmContext, "eh.landing.pad", function);
  tuple<LandingPadInst*, Value*, Value*>
  landingPadIR = generateLandingPad(context, landingPadBlock, mCatchList);
  LandingPadInst* landingPadInst = get<0>(landingPadIR);
  Value* wrappedException = get<1>(landingPadIR);
  Value* exceptionTypeId = get<2>(landingPadIR);
  
  mComposingCallbacks.push_back(make_tuple(composeLandingPadBlock,
                                           landingPadBlock,
                                           freeMemoryBlock,
                                           freeMemoryEndBlock,
                                           landingPadInst,
                                           wrappedException,
                                           exceptionTypeId));
  
  return make_tuple(landingPadBlock, wrappedException);
}

bool TryCatchInfo::runComposingCallbacks(IRGenerationContext& context) {
  for (Catch* catchClause : mCatchList) {
    const Model* catchType = catchClause->getType(context)->getReference();
    context.getScopes().getScope()->removeException(catchType);
  }
  
  BasicBlock* lastBasicBlock = context.getBasicBlock();
  
  bool result = true;

  for (tuple<LandingPadComposingFunction, BasicBlock*, BasicBlock*, BasicBlock*, LandingPadInst*,
       Value*, Value*> callbackTuple : mComposingCallbacks) {
    LandingPadComposingFunction function = get<0>(callbackTuple);
    BasicBlock* landingPadBlock = get<1>(callbackTuple);
    BasicBlock* freeMemoryBlock = get<2>(callbackTuple);
    BasicBlock* freeMemoryEndBlock = get<3>(callbackTuple);
    LandingPadInst* landingPadInst = get<4>(callbackTuple);
    Value* wrappedException = get<5>(callbackTuple);
    Value* exceptionTypId = get<6>(callbackTuple);
    
    result &= function(context,
                       landingPadBlock,
                       mCatchList,
                       mContinueBlock,
                       freeMemoryBlock,
                       freeMemoryEndBlock,
                       landingPadInst,
                       wrappedException,
                       exceptionTypId);
  }
  
  context.setBasicBlock(lastBasicBlock);
  return result;
}

bool TryCatchInfo::composeLandingPadBlock(IRGenerationContext& context,
                                          BasicBlock* landingPadBlock,
                                          vector<Catch*> catchList,
                                          BasicBlock* continueBlock,
                                          BasicBlock* freeMemoryBlock,
                                          BasicBlock* freeMemoryEndBlock,
                                          LandingPadInst* landingPadInst,
                                          Value* wrappedException,
                                          Value* exceptionTypeId) {
  context.setBasicBlock(landingPadBlock);
  
  LLVMContext& llvmContext = context.getLLVMContext();
  Function* function = landingPadBlock->getParent();
  BasicBlock* landingPadContinue = BasicBlock::Create(llvmContext, "eh.landing.pad.cont", function);
  IRWriter::createBranch(context, freeMemoryBlock);
  context.setBasicBlock(freeMemoryEndBlock);
  IRWriter::createBranch(context, landingPadContinue);
  
  vector<tuple<Catch*, BasicBlock*>> catchesAndBlocks =
  generateSelectCatchByExceptionType(context, exceptionTypeId, catchList, landingPadContinue);
  generateResumeAndFail(context,
                        landingPadInst,
                        exceptionTypeId,
                        wrappedException);
  return generateCatches(context,
                         wrappedException,
                         catchesAndBlocks,
                         continueBlock);
}

tuple<LandingPadInst*, Value*, Value*>
TryCatchInfo::generateLandingPad(IRGenerationContext& context,
                                 BasicBlock* landingPadBlock,
                                 vector<Catch*> catchList) {
  LLVMContext& llvmContext = context.getLLVMContext();
  context.setBasicBlock(landingPadBlock);
  Type* int8PointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  vector<Type*> landingPadReturnTypes;
  landingPadReturnTypes.push_back(int8PointerType);
  landingPadReturnTypes.push_back(Type::getInt32Ty(llvmContext));
  StructType* landingPadReturnType = StructType::get(llvmContext, landingPadReturnTypes);
  LandingPadInst* landingPad = LandingPadInst::Create(landingPadReturnType,
                                                      (int) catchList.size(),
                                                      "",
                                                      landingPadBlock);
  
  for (Catch* catchClause : catchList) {
    const ModelOwner* exceptionType = catchClause->getType(context);
    landingPad->addClause(context.getModule()->
                          getNamedGlobal(exceptionType->getReference()->getRTTIVariableName()));
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
                                              Value* wrappedException) {
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

  IRWriter::createCallInst(context, unexpectedFunction, arguments, "");
  IRWriter::newUnreachableInst(context);
  
  context.setBasicBlock(resumeBlock);
  IRWriter::createResumeInst(context, landingPadInst);
}

vector<tuple<Catch*, BasicBlock*>>
TryCatchInfo::generateSelectCatchByExceptionType(IRGenerationContext& context,
                                                 Value* exceptionTypeId,
                                                 vector<Catch*> catchList,
                                                 BasicBlock* landingPadBlock) {
  LLVMContext& llvmContext = context.getLLVMContext();
  Type* int8PointerType = Type::getInt8Ty(llvmContext)->getPointerTo();
  Function* typeIdFunction = IntrinsicFunctions::getTypeIdFunction(context);
  Function* function = context.getBasicBlock()->getParent();
  
  vector<tuple<Catch*, BasicBlock*>> catchesAndBlocks;
  BasicBlock* currentBlock = landingPadBlock;
  
  for (Catch* catchClause : catchList) {
    context.setBasicBlock(currentBlock);
    const ModelOwner* exceptionType = catchClause->getType(context);
    Value* rtti = context.getModule()->
    getNamedGlobal(exceptionType->getReference()->getRTTIVariableName());
    Value* rttiBitcast = IRWriter::newBitCastInst(context, rtti, int8PointerType);
    vector<Value*> arguments;
    arguments.push_back(rttiBitcast);
    CallInst* typeId = IRWriter::createCallInst(context, typeIdFunction, arguments, "");
    typeId->setTailCall();
    ICmpInst* compare =
    IRWriter::newICmpInst(context, ICmpInst::ICMP_EQ, exceptionTypeId, typeId, "");
    string catchBlockName = "catch." + exceptionType->getReference()->getTypeName();
    BasicBlock* catchBlock = BasicBlock::Create(llvmContext, catchBlockName, function);
    catchesAndBlocks.push_back(make_tuple(catchClause, catchBlock));
    string nextBlockName = "not." + exceptionType->getReference()->getTypeName();
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
                                   BasicBlock* exceptionContinueBlock) {
  bool doAllCatchesTerminate = true;
  for (tuple<Catch*, BasicBlock*> catchAndBlock : catchesAndBlocks) {
    Catch* catchClause = get<0>(catchAndBlock);
    BasicBlock* catchBlock = get<1>(catchAndBlock);
    doAllCatchesTerminate &= catchClause->generateIR(context,
                                                     wrappedException,
                                                     catchBlock,
                                                     exceptionContinueBlock);
  }
  
  return doAllCatchesTerminate;
}

