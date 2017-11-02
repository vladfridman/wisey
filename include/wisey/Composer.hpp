//
//  Composer.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 7/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Composer_h
#define Composer_h

#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"

namespace wisey {

/**
 * A collection of static methods used to add wisey language code snippets
 */
class Composer {

public:
  
  /**
   * Check if a given expession is null and throw an NPE if it is
   */
  static void checkNullAndThrowNPE(IRGenerationContext& context,
                                   llvm::Value* value,
                                   llvm::Value* threadObject,
                                   int line);

  /**
   * Push new entry on call stack
   */
  static void pushCallStack(IRGenerationContext& context, llvm::Value* threadObject, int line);

  /**
   * Pop an entry from call stack
   */
  static void popCallStack(IRGenerationContext& context, llvm::Value* threadObject);

};

} /* namespace wisey */

#endif /* Composer_h */
