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
  
};
  
} /* namespace yazyk */

#endif /* IRWriter_h */
