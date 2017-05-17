//
//  IntrinsicFunctions.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/26/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IntrinsicFunctions_h
#define IntrinsicFunctions_h

#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"

namespace wisey {
  
/**
 * A collection of static methods responsible for creation intrinsic functions declarations
 */
class IntrinsicFunctions {

public:
  
  /**
   * Declares __cxa_throw function that is used to throw exceptions
   */
  static llvm::Function* getThrowFunction(IRGenerationContext& context);
  
  /**
   * Declares __cxa_allocate_exception function that is used to allocate an exception before throw
   */
  static llvm::Function* getAllocateExceptionFunction(IRGenerationContext& context);
  
  /**
   * Declares llvm.memcpy.p0i8.p0i8.i64 function that is used to copy memory
   */
  static llvm::Function* getMemCopyFunction(IRGenerationContext& context);

  /**
   * Declares __gxx_personality_v0 function that is used orchestrate exception handling
   */
  static llvm::Function* getPersonalityFunction(IRGenerationContext& context);
  
  /**
   * Declares llvm.eh.typeid.for function that identifies exception type assigned
   * by personality function
   */
  static llvm::Function* getTypeIdFunction(IRGenerationContext& context);
  
  /**
   * Declares __cxa_call_unexpected function that is used to notify of unexpected conditions
   * during exception handling
   */
  static llvm::Function* getUnexpectedFunction(IRGenerationContext& context);
  
  /**
   * Declares __cxa_begin_catch function that extracts the exception object
   */
  static llvm::Function* getBeginCatchFunction(IRGenerationContext& context);
  
  /**
   * Declares __cxa_end_catch function that completes a catch block
   */
  static llvm::Function* getEndCatchFunction(IRGenerationContext& context);
  
  /**
   * Declares printf function that is used to print things to screen
   */
  static llvm::Function* getPrintfFunction(IRGenerationContext& context);
};
  
} /* namespace wisey */

#endif /* IntrinsicFunctions_h */
