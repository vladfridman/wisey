//
//  IMethod.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IMethod_h
#define IMethod_h

#include "wisey/IMethodDescriptor.hpp"
#include "wisey/IObjectElement.hpp"
#include "wisey/IObjectType.hpp"

namespace wisey {
  
  /**
   * Represents a method implementation that could either be static or non-static
   */
  class IMethod : public IMethodDescriptor, public IObjectElement {
    
  public:
    
    /**
     * Defines LLVM function for this method
     */
    virtual llvm::Function* defineFunction(IRGenerationContext& context) = 0;
    
    /**
     * Generates IR for this method in a given concrete object
     */
    virtual void generateIR(IRGenerationContext& context) const = 0;
    
    /**
     * Store method argument as a variable in the current scope
     */
    static void storeArgumentValue(IRGenerationContext& context,
                                   std::string name,
                                   const IType* type,
                                   llvm::Value* value);
    /**
     * Store method argument as a variable in the current scope
     */
    static void storeSystemArgumentValue(IRGenerationContext& context,
                                         std::string name,
                                         const IObjectType* type,
                                         llvm::Value* value);
    
    /**
     * Check that the method handles or rethrows all the exceptions and print an error if it doesn't
     */
    static void checkForUnhandledExceptions(IRGenerationContext& context, const IMethod* method);
    
    /**
     * Add a return void if there is no explicit return is in the wisey code
     */
    static void maybeAddImpliedVoidReturn(IRGenerationContext& context,
                                          const IMethod* method,
                                          int line);
    
    /**
     * Define llvm function for the given method
     */
    static llvm::Function* defineFunction(IRGenerationContext& context,
                                          const IObjectType* objectType,
                                          const IMethod* method);
    
    /**
     * Get pointer to the method name constant
     */
    static void defineCurrentMethodNameVariable(IRGenerationContext& context,
                                                std::string methodName);
    
  };
  
} /* namespace wisey */

#endif /* IMethod_h */

