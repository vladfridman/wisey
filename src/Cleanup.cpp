//
//  Cleanup.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/5/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Block.hpp"
#include "wisey/Cleanup.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/TryCatchInfo.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

void Cleanup::generateCleanupTryCatchInfo(IRGenerationContext& context) {
  Function* function = context.getBasicBlock()->getParent();
  BasicBlock* landingPadBlock = BasicBlock::Create(context.getLLVMContext(),
                                                   "cleanup.landing.pad",
                                                   function);
  Block* emtpyBlock = new Block();
  vector<Catch*> catchList;
  TryCatchInfo* tryCatchInfo = new TryCatchInfo(landingPadBlock, NULL, emtpyBlock, catchList);
  context.getScopes().setTryCatchInfo(tryCatchInfo);
}

void Cleanup::generateCleanupLandingPad(IRGenerationContext& context) {
  TryCatchInfo* tryCatchInfo = context.getScopes().getTryCatchInfo();
  context.setBasicBlock(tryCatchInfo->getLandingPadBlock());
  
  LLVMContext& llvmContext = context.getLLVMContext();
  Function* function = context.getBasicBlock()->getParent();
  if (!function->hasPersonalityFn()) {
    function->setPersonalityFn(IntrinsicFunctions::getPersonalityFunction(context));
  }
  
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
  IRWriter::createResumeInst(context, landingPad);
  
  context.getScopes().clearTryCatchInfo();
  delete tryCatchInfo;
}
