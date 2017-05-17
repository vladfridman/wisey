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

#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IType.hpp"

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
  
};

} /* namespace yazyk */

#endif /* AutoCast_h */
