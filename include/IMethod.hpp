//
//  IMethod.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IMethod_h
#define IMethod_h

#include "IMethodDescriptor.hpp"
#include "IObjectElement.hpp"
#include "IObjectType.hpp"

namespace wisey {
  
  /**
   * Represents a method implementation that could either be static or non-static
   */
  class IMethod : public IMethodDescriptor, public IObjectElement {
    
  public:
    
    /**
     * Declares LLVM function corresponding to this method
     */
    virtual llvm::Function* declareFunction(IRGenerationContext& context) const = 0;
    
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
                                   llvm::Value* value,
                                   int line);
    /**
     * Store method argument as a variable that does not adjust its ref count in the current scope
     */
    static void storeStaticArgumentValue(IRGenerationContext& context,
                                         std::string name,
                                         const IObjectType* type,
                                         llvm::Value* value);
    
    /**
     * Check that the method handles or rethrows all the exceptions and print an error if it doesn't
     */
    static void checkForUnhandledExceptions(IRGenerationContext& context,
                                            const IMethod* method,
                                            int line);
    
    /**
     * Add a return void if there is no explicit return is in the wisey code
     */
    static void maybeAddImpliedVoidReturn(IRGenerationContext& context,
                                          const IMethod* method,
                                          int line);
    
    /**
     * Declare llvm function for the given method in the given object
     */
    static llvm::Function* declareFunctionForObject(IRGenerationContext& context,
                                                    const IObjectType* objectType,
                                                    const IMethod* method);
    
    /**
     * Check that the return type is not an inner private object
     */
    static void checkReturnAndArgumentTypes(IRGenerationContext& context,
                                            const IMethod* method,
                                            const IObjectType* object);

  private:
    
    static bool checkTypeIsVisible(const IObjectType* object, const IType*);
    
  };
  
} /* namespace wisey */

#endif /* IMethod_h */

