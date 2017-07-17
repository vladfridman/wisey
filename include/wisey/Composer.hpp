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
  static void checkNullAndThrowNPE(IRGenerationContext& context, llvm::Value* value);
  
  /**
   * Free memory if the pointer is not null
   */
  static void freeIfNotNull(IRGenerationContext& context, llvm::Value* pointer);

};

} /* namespace wisey */

#endif /* Composer_h */
