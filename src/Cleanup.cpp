//
//  Cleanup.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/5/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Cleanup.hpp"
#include "wisey/FinallyBlock.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/TryCatchInfo.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

BasicBlock* Cleanup::generate(IRGenerationContext& context, FinallyBlock* finallyBlock) {
  BasicBlock* lastBasicBlock = context.getBasicBlock();
  Function* function = lastBasicBlock->getParent();

  BasicBlock* landingPadBlock = BasicBlock::Create(context.getLLVMContext(), "cleanup", function);
  context.setBasicBlock(landingPadBlock);

  LLVMContext& llvmContext = context.getLLVMContext();
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
  if (finallyBlock) {
    vector<Catch*> emptyCatchList;
    context.getScopes().pushScope();
    FinallyBlock subFinallyBlock;
    TryCatchInfo* cleaupTryCatchInfo = new TryCatchInfo(&subFinallyBlock, emptyCatchList, NULL);
    context.getScopes().setTryCatchInfo(cleaupTryCatchInfo);

    finallyBlock->generateIR(context);
    
    context.getScopes().popScope(context);
  }

  context.getScopes().freeOwnedMemory(context);
  
  IRWriter::createResumeInst(context, landingPad);

  return landingPadBlock;
}
