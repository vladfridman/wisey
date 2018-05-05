//
//  InstanceOfFunction.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef InstanceOfFunction_h
#define InstanceOfFunction_h

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"

namespace wisey {
  
  /**
   * A global function for determining whether an object is an instance of some other object type
   *
   * The function is used to determine whether an interface reference is castable to another
   * interface or a concrete object. The function returns -1 if the interface is not castable
   * otherwise returns an index of the interface in concrete object's flattened interface list
   */
  class InstanceOfFunction {
    
  public:
    
    /**
     * Returns the instanceOf function
     */
    static llvm::Function* get(IRGenerationContext& context);
    
    /**
     * Calls instanceOf function to get the index of the given interface in a given object
     */
    static llvm::Value* call(IRGenerationContext& context,
                             llvm::Value* haystack,
                             llvm::Value* needle);
    
  private:
    
    static std::string getName();
    
    static llvm::Function* define(IRGenerationContext& context);
    
    static void compose(IRGenerationContext& context, llvm::Function* function);
    
    static LLVMFunctionType* getLLVMFunctionType(IRGenerationContext& context);
    
    static llvm::BitCastInst* composeEntryBlock(IRGenerationContext& context,
                                                llvm::BasicBlock* entryBlock,
                                                llvm::BasicBlock* returnNotFound,
                                                llvm::BasicBlock* whileCond,
                                                llvm::Function* function);
    
    static llvm::LoadInst* composeWhileConditionBlock(IRGenerationContext& context,
                                                      llvm::BasicBlock* whileCond,
                                                      llvm::BasicBlock* whileBody,
                                                      llvm::BasicBlock* returnFalse,
                                                      llvm::Value* iterator,
                                                      llvm::Value* arrayOfStrings);
    
    static void composeWhileBodyBlock(IRGenerationContext& context,
                                      llvm::BasicBlock* whileBody,
                                      llvm::BasicBlock* whileCond,
                                      llvm::BasicBlock* returnTrue,
                                      llvm::Value* iterator,
                                      llvm::Value* stringPointer,
                                      llvm::Function* function);
    
    static void composeReturnFound(IRGenerationContext& context,
                                   llvm::BasicBlock* returnFound,
                                   llvm::Value* iterator);
    
    static void composeReturnNotFound(IRGenerationContext& context,
                                      llvm::BasicBlock* returnNotFound);

  };
  
} /* namespace wisey */

#endif /* InstanceOfFunction_h */
