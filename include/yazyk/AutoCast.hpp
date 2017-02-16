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
   * Tells whether one type can be cast into another without a loss
   */
  static bool canCastLosslessFromTo(IType* fromType, IType* toType);

  /**
   * Tells whether one type can be cast into another at all
   */
  static bool canCast(IType* fromType, IType* toType);

  /**
   * Try to cast expression's value to a given type
   */
  static llvm::Value* maybeCast(IRGenerationContext& context,
                                IExpression& expression,
                                IType* toType);
private:

  static bool canCastLosslessPrimitiveTypeFromTo(IType* fromType, IType* toType);

  static llvm::Value* maybeCastPrimitiveTypes(IRGenerationContext& context,
                                              IExpression& expression,
                                              IType* toType);

  static llvm::Value* maybeCastModelTypes(IRGenerationContext& context,
                                          IExpression& expression,
                                          IType* toType);
  
  static void exitIncopatibleTypes(IType* fromType, IType* toType);

  static void exitNeedExplicitCast(IType* fromType, IType* toType);

  static llvm::Value* widenIntCast(IRGenerationContext& context,
                                   IExpression& expression,
                                   IType* toType);
  
  static llvm::Value* widenFloatCast(IRGenerationContext& context,
                                     IExpression& expression,
                                     IType* toType);
  
  static llvm::Value* intToFloatCast(IRGenerationContext& context,
                                     IExpression& expression,
                                     IType* toType);
};

} /* namespace yazyk */

#endif /* AutoCast_h */
