//
//  IRWriter.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 4/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IRWriter_h
#define IRWriter_h

#include <llvm/IR/Instructions.h>

#include "yazyk/IRGenerationContext.hpp"

namespace yazyk {

/**
 * Class resposible for adding LLVM Intermediate Representation code.
 *
 * It mainly serves the purpose of guarding against writing new instructions after a terminator.
 * It always creates instructions in the context's current basic block
 */
class IRWriter {
  
public:
  
  /**
   * Create an instance of a ReturnInst and check that the block does not have a terminator already
   */
  static llvm::ReturnInst* createReturnInst(IRGenerationContext& context, llvm::Value* returnValue);
  
  /**
   * Create a conditional branch if the current basic block does not already have a terminator
   */
  static llvm::BranchInst* createBranch(IRGenerationContext& context,
                                        llvm::BasicBlock* toBlock);
  
  /**
   * Create a conditional branch if the current basic block does not already have a terminator
   */
  static llvm::BranchInst* createConditionalBranch(IRGenerationContext& context,
                                                   llvm::BasicBlock* ifTrueBlock,
                                                   llvm::BasicBlock* ifFalseBlock,
                                                   llvm::Value* condition);
  
  /**
   * Create binary operator which could be an addition, multiplications, etc.
   */
  static llvm::BinaryOperator* createBinaryOperator(IRGenerationContext& context,
                                                    llvm::Instruction::BinaryOps instruction,
                                                    llvm::Value* leftValue,
                                                    llvm::Value* rightValue,
                                                    std::string llvmVariableName);

  /**
   * Create a call to a given function with supplied arguments
   */
  static llvm::CallInst* createCallInst(IRGenerationContext& context,
                                        llvm::Function* function,
                                        std::vector<llvm::Value*> arguments,
                                        std::string resultName);

  /**
   * Create a call to malloc function that allocates memory on heap
   */
  static llvm::Instruction* createMalloc(IRGenerationContext& context,
                                         llvm::Type* structType,
                                         llvm::Value* allocSize,
                                         std::string variableName);
  
  /**
   * Create a call to free function that frees heep allocated memory
   */
  static llvm::Instruction* createFree(IRGenerationContext& context, llvm::Value* value);

  /**
   * Create a GetElementPtrInst instruction for a given array or struct with given indexes
   */
  static llvm::GetElementPtrInst* createGetElementPtrInst(IRGenerationContext& context,
                                                          llvm::Value* value,
                                                          llvm::ArrayRef<llvm::Value *> index);

  /**
   * Add a BitCastInst to the current basic block casting value to the given type
   */
  static llvm::BitCastInst* newBitCastInst(IRGenerationContext& context,
                                           llvm::Value* value,
                                           llvm::Type* type);

  /**
   * Add StoreInst instruction to the current basic block storing the value in the memory pointed to
   * by the given pointer
   */
  static llvm::StoreInst* newStoreInst(IRGenerationContext& context,
                                       llvm::Value* value,
                                       llvm::Value* pointer);

  /**
   * Add AllocaInst instruction to the the current basic block allocating on stack memory
   * for a given type
   */
  static llvm::AllocaInst* newAllocaInst(IRGenerationContext& context,
                                         llvm::Type* type,
                                         std::string variableName);

  
  /**
   * Add LoadInst instruction to the the current basic block loading a variable value from pointer
   */
  static llvm::LoadInst* newLoadInst(IRGenerationContext& context,
                                     llvm::Value* pointer,
                                     std::string variableName);
  
  /**
   * Add a CastInst that zero-extends an int value to a longer int type
   */
  static llvm::CastInst* createZExtOrBitCast(IRGenerationContext& context,
                                             llvm::Value* fromValue,
                                             llvm::Type* toLLVMType);

  /**
   * Add a TruncInst that truncates an int value to a given int type
   */
  static llvm::TruncInst* newTruncInst(IRGenerationContext& context,
                                       llvm::Value* fromValue,
                                       llvm::Type* toLLVMType);
  /**
   * Add a FPTruncInst that truncates a float value to a given float type
   */
  static llvm::FPTruncInst* newFPTruncInst(IRGenerationContext& context,
                                           llvm::Value* fromValue,
                                           llvm::Type* toLLVMType);

  /**
   * Add a FPExtInst that widens a float value to a given float type
   */
  static llvm::FPExtInst* newFPExtInst(IRGenerationContext& context,
                                       llvm::Value* fromValue,
                                       llvm::Type* toLLVMType);

  /**
   * Add a SIToFPInst that casts int type to float
   */
  static llvm::SIToFPInst* newSIToFPInst(IRGenerationContext& context,
                                         llvm::Value* fromValue,
                                         llvm::Type* toLLVMType);
};
  
} /* namespace yazyk */

#endif /* IRWriter_h */
