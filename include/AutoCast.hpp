//
//  AutoCast.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/14/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef AutoCast_h
#define AutoCast_h

#include <llvm/IR/Instructions.h>

#include "IExpression.hpp"
#include "IRGenerationContext.hpp"
#include "IType.hpp"

namespace wisey {
  
  /**
   * Methods for automatic casting of expressions
   */
  class AutoCast {
    
  public:
    
    /**
     * Try to cast expression's value to a given type
     */
    static llvm::Value* maybeCast(IRGenerationContext& context,
                                  const IType* fromType,
                                  llvm::Value* fromValue,
                                  const IType* toType,
                                  int line);
    
  };
  
} /* namespace wisey */

#endif /* AutoCast_h */

