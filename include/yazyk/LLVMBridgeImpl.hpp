//
//  LLVMBridgeImpl.h
//  Yazyk
//
//  Created by Vladimir Fridman on 12/10/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMBridgeImpl_h
#define LLVMBridgeImpl_h

#include <llvm/ADT/Twine.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Value.h>
#include <llvm/ADT/Optional.h>
#include <llvm/ADT/Twine.h>
#include <llvm/IR/Attributes.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/OperandTraits.h>

#include "ILLVMBridge.hpp"

namespace yazyk {
  
/**
 * Bridge for calling static LLVM methods
 */
class LLVMBridgeImpl : public ILLVMBridge {
public:
  
  /**
   * Routes the call to llvm::BinaryOperator::Create()
   */
  llvm::Value * createBinaryOperator(llvm::Instruction::BinaryOps operation,
                                     llvm::Value * lhsValue,
                                     llvm::Value * rhsValue,
                                     const llvm::Twine &name,
                                     llvm::BasicBlock *basicBlock) const;
};

}



#endif /* LLVMBridgeImpl_h */
