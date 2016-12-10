//
//  LLVMBridge.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/10/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <llvm/IR/InstrTypes.h>

#include "yazyk/LLVMBridgeImpl.hpp"

using namespace llvm;
using namespace yazyk;

Value * LLVMBridgeImpl::createBinaryOperator(Instruction::BinaryOps operation,
                                             Value * lhsValue,
                                             Value * rhsValue,
                                             const Twine &name,
                                             BasicBlock *basicBlock) const {
  return llvm::BinaryOperator::Create(operation,
                                      lhsValue,
                                      rhsValue,
                                      name,
                                      basicBlock);
};
