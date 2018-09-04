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
    
    /**
     * Sets the current line number in the latest entry in the call stack
     */
    static void setLineNumberAtRuntime(IRGenerationContext& context,
                                       llvm::Value* callStackValue,
                                       llvm::Value* lineNumber);

    /**
     * Increments reference count unsafely for an object that is either controller or a node
     */
    static void incrementReferenceCountUnsafely(IRGenerationContext& context, llvm::Value* object);
    
    /**
     * Decrements reference count unsafely for an object that is either controller or a node
     */
    static void decrementReferenceCountUnsafely(IRGenerationContext& context, llvm::Value* object);
    
    /**
     * Increments reference count safely for an object that is either a model or a thread controller
     */
    static void incrementReferenceCountSafely(IRGenerationContext& context, llvm::Value* object);
    
    /**
     * Decrements reference count safely for an object that is either a model or a thread controller
     */
    static void decrementReferenceCountSafely(IRGenerationContext& context, llvm::Value* object);
 
    /**
     * Increments reference count unsafely for an array
     */
    static void incrementArrayReferenceCountUnsafely(IRGenerationContext& context,
                                                     llvm::Value* array);
    
    /**
     * Decrements reference count unsafely for an array
     */
    static void decrementArrayReferenceCountUnsafely(IRGenerationContext& context,
                                                     llvm::Value* array);

    /**
     * Checks if the value is null and throws npe if so
     */
    static void checkForNull(IRGenerationContext& context, llvm::Value* value, int line);
    
    /**
     * Checks that array index is in range
     */
    static void checkArrayIndex(IRGenerationContext& context,
                                llvm::Value* index,
                                llvm::Value* size,
                                int line);
    
  private:
    
    static bool shouldSkipCallStack(IRGenerationContext& context);
    
    static llvm::StructType* getCCallStackStruct(IRGenerationContext& context);
    
    static void adjustReferenceCountUnsafely(IRGenerationContext& context,
                                             llvm::Value* object,
                                             int adjustmentValue);
    
    static void adjustReferenceCountSafely(IRGenerationContext& context,
                                           llvm::Value* object,
                                           int adjustmentValue);

    static void adjustArrayReferenceCountUnsafely(IRGenerationContext& context,
                                                  llvm::Value* array,
                                                  int adjustmentValue);

  };
  
} /* namespace wisey */

#endif /* Composer_h */

