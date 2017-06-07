//
//  Cast.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Cast_h
#define Cast_h

#include <llvm/IR/Instructions.h>

#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IType.hpp"

namespace wisey {
  
/**
 * Methods for cast operations
 */
class Cast {

public:
  
  /**
   * Print an error message when types are incompatable
   */
  static void exitIncopatibleTypes(const IType* fromType, const IType* toType);
  
  /**
   * Widen cast a given value to a given type
   */
  static llvm::Value* widenIntCast(IRGenerationContext& context,
                                   llvm::Value* fromValue,
                                   const IType* toType);
  
  /**
   * Truncate cast a given int type value to a given type
   */
  static llvm::Value* truncIntCast(IRGenerationContext& context,
                                   llvm::Value* fromValue,
                                   const IType* toType);
  
  /**
   * Cast a given int value to a float or double type
   */
  static llvm::Value* intToFloatCast(IRGenerationContext& context,
                                     llvm::Value* fromValue,
                                     const IType* toType);
  
  /**
   * Cast a given float or double value to one of int types
   */
  static llvm::Value* floatToIntCast(IRGenerationContext& context,
                                     llvm::Value* fromValue,
                                     const IType* toType);
  
  /**
   * Truncate cast a given float type value to a given float type
   */
  static llvm::Value* truncFloatCast(IRGenerationContext& context,
                                     llvm::Value* fromValue,
                                     const IType* toType);
  
  /**
   * Cast float to double by widening the float
   */
  static llvm::Value* widenFloatCast(IRGenerationContext& context,
                                     llvm::Value* fromValue,
                                     const IType* toType);
  
};

} /* namespace wisey */

#endif /* Cast_h */
