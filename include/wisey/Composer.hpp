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
     * Push new entry on call stack
     */
    static void pushCallStack(IRGenerationContext& context, int line);
    
    /**
     * Pop an entry from call stack
     */
    static void popCallStack(IRGenerationContext& context);
    
    /**
     * Sets the current line number in the latest entry in the call stack
     */
    static void setLineNumber(IRGenerationContext& context, int line);
    
  private:
    
    static bool shouldSkip(IRGenerationContext& context);
    
    static llvm::StructType* getCCallStackStruct(IRGenerationContext& context);
    
  };
  
} /* namespace wisey */

#endif /* Composer_h */

