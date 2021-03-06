//
//  IMethodDescriptor.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IMethodDescriptor_h
#define IMethodDescriptor_h

#include <iostream>
#include <string>
#include <vector>

#include <llvm/IR/DerivedTypes.h>

#include "IType.hpp"
#include "IObjectType.hpp"
#include "IPrintable.hpp"
#include "MethodQualifier.hpp"

namespace wisey {
  
  class IRGenerationContext;
  class Argument;
  class Model;
  
  /**
   * Represents an object that describes a method.
   *
   * There are two implementations: Method and MethodSignature
   */
  class IMethodDescriptor : public IType {
    
  public:
    
    virtual ~IMethodDescriptor() { }
    
    /**
     * Returns the method's name
     */
    virtual std::string getName() const = 0;
    
    /**
     * Tells whether the method is publicly accessable
     */
    virtual bool isPublic() const = 0;
    
    /**
     * Returns method's return type
     */
    virtual const IType* getReturnType() const = 0;
    
    /**
     * Returns an array of method arguments
     */
    virtual std::vector<const Argument*> getArguments() const = 0;
    
    /**
     * Returns a list of exceptions this method throws
     */
    virtual std::vector<const Model*> getThrownExceptions() const = 0;
    
    /**
     * Tells whether this method is static
     */
    virtual bool isStatic() const = 0;

    /**
     * Tells whether this method is marked with override qualifier
     */
    virtual bool isOverride() const = 0;

    /**
     * Return corresponding LLVM type
     */
    virtual llvm::FunctionType* getLLVMType(IRGenerationContext& context) const = 0;
    
    /**
     * Returns the object this method belongs to
     */
    virtual const IObjectType* getParentObject() const = 0;
    
    /**
     * Returns the method qualifiers
     */
    virtual MethodQualifiers* getMethodQualifiers() const = 0;
    
    /**
     * Returns function type corresponding to the given method descirptor
     */
    static llvm::FunctionType* getLLVMFunctionType(IRGenerationContext& context,
                                                   const IMethodDescriptor* method,
                                                   const IObjectType* object,
                                                   int line);
    
    /**
     * Print the given method descriptor to the given stream
     */
    static void printDescriptorToStream(const IMethodDescriptor* method, std::iostream& stream);
    
  };
  
} /* namespace wisey */

#endif /* IMethodDescriptor_h */

