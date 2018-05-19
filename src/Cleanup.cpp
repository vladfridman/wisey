//
//  Cleanup.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/5/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/Cleanup.hpp"
#include "wisey/Environment.hpp"
#include "wisey/IntrinsicFunctions.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/TryCatchInfo.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

BasicBlock* Cleanup::generate(IRGenerationContext& context, int line) {
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

  Value* landingPadReturnValueAlloca = IRWriter::newAllocaInst(context, landingPadReturnType, "");
  IRWriter::newStoreInst(context, landingPad, landingPadReturnValueAlloca);
  
  Value* index[2];
  index[0] = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  index[1] = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
  Value* wrappedExceptionPointer =
  IRWriter::createGetElementPtrInst(context, landingPadReturnValueAlloca, index);
  Value* wrappedException = IRWriter::newLoadInst(context, wrappedExceptionPointer, "");
  Function* getException = IntrinsicFunctions::getExceptionPointerFunction(context);
  vector<Value*> arguments;
  arguments.push_back(wrappedException);
  Value* exceptionShell = IRWriter::createCallInst(context, getException, arguments, "");
  Value* idx[1];
  idx[0] = ConstantInt::get(Type::getInt64Ty(llvmContext), Environment::getAddressSizeInBytes());
  Value* exception = IRWriter::createGetElementPtrInst(context, exceptionShell, idx);

  landingPad->setCleanup(true);

  context.getScopes().freeOwnedMemory(context, exception, line);
  
  IRWriter::createResumeInst(context, landingPad);

  return landingPadBlock;
}
