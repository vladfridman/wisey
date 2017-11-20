//
//  InstanceOf.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef InstanceOf_h
#define InstanceOf_h

#include <llvm/IR/Instructions.h>

#include "wisey/Interface.hpp"

namespace wisey {

/**
 * Class for creating instanceof function.
 *
 * The function is used to determine whether an interface reference is castable to another 
 * interface or a concrete object. The function returns -1 if the interface is not castable
 * otherwise returns an index of the interface in concrete object's flattened interface list
 */
class InstanceOf {
  
public:
  
  /**
   * Call instanceof function and check whether interfaceObject is of type callableObjectType
   */
  static llvm::Value* call(IRGenerationContext& context,
                           const Interface* interface,
                           llvm::Value* interfaceObject,
                           const IObjectType* object);

  /**
   * Creates instanceof function for the given interface
   */
  static llvm::Function* getOrCreateFunction(IRGenerationContext& context,
                                             const Interface* interface);
  
private:
  
  static std::string getFunctionName(const Interface* interface);
  
  static llvm::Function* compose(IRGenerationContext& context,
                                 llvm::Function* function,
                                 std::vector<const IObjectType*> objectTypes);
  
  static llvm::Function* createFunction(IRGenerationContext& context, const Interface* interface);
  
  static llvm::BitCastInst* composeEntryBlock(IRGenerationContext& context,
                                              llvm::BasicBlock* entryBlock,
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
  
#endif /* InstanceOf_h */
