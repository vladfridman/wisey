//
//  LLVMBridgeImpl.h
//  Yazyk
//
//  Created by Vladimir Fridman on 12/10/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMBridge_h
#define LLVMBridge_h

#include <llvm/ADT/Twine.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Value.h>

namespace yazyk {

/**
 * Interface for calling static LLVM methods.
 *
 * LLVMBridgeImpl is the implementation. This interface is needed for mocking purposes in tests
 */
class ILLVMBridge {
public:
  virtual ~ILLVMBridge() { }
  
  /**
   * Create binary instruction such as addition or multiplication of two statements
   */
  virtual llvm::Value * createBinaryOperator(llvm::Instruction::BinaryOps operation,
                                             llvm::Value * lhsValue,
                                             llvm::Value * rhsValue,
                                             const llvm::Twine &name,
                                             llvm::BasicBlock *basicBlock) const = 0;
};
  
}

#endif /* LLVMBridge_h */
