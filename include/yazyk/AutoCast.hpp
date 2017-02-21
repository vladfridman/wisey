//
//  AutoCast.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/14/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef AutoCast_h
#define AutoCast_h

#include <llvm/IR/Instructions.h>

#include "yazyk/IExpression.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/IType.hpp"

namespace yazyk {

/**
 * Methods for automatic casting of expressions
 */
class AutoCast {
  
public:
  
  /**
   * Try to cast expression's value to a given type
   */
  static llvm::Value* maybeCast(IRGenerationContext& context,
                                IType* fromType,
                                llvm::Value* fromValue,
                                IType* toType);
  /**
   * Print an error message when types are incompatable
   */
  static void exitIncopatibleTypes(const IType* fromType, const IType* toType);

  /**
   * Widen cast a given value to a given type
   */
  static llvm::Value* widenIntCast(IRGenerationContext& context,
                                   llvm::Value* fromValue,
                                   IType* toType);

  /**
   * Truncate cast a given int type value to a given type
   */
  static llvm::Value* truncIntCast(IRGenerationContext& context,
                                   llvm::Value* fromValue,
                                   IType* toType);
  
  /**
   * Cast a given int value to a float or double type
   */
  static llvm::Value* intToFloatCast(IRGenerationContext& context,
                                     llvm::Value* fromValue,
                                     IType* toType);
  
  /**
   * Cast a given float or double value to one of int types
   */
  static llvm::Value* floatToIntCast(IRGenerationContext& context,
                                     llvm::Value* fromValue,
                                     IType* toType);

  /**
   * Truncate cast a given float type value to a given float type
   */
  static llvm::Value* truncFloatCast(IRGenerationContext& context,
                                     llvm::Value* fromValue,
                                     IType* toType);
  
  /**
   * Cast float to double by widening the float
   */
  static llvm::Value* widenFloatCast(IRGenerationContext& context,
                                     llvm::Value* fromValue,
                                     IType* toType);
private:

  static void exitNeedExplicitCast(IType* fromType, IType* toType);
  
  
};

} /* namespace yazyk */

#endif /* AutoCast_h */
