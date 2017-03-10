//
//  IMethodDescriptor.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IMethodDescriptor_h
#define IMethodDescriptor_h

#include <string>
#include <vector>

#include <llvm/IR/DerivedTypes.h>

namespace yazyk {
  
class ICallableObjectType;
class IRGenerationContext;
class IType;
class MethodArgument;

  /**
 * Represents an object that describes a method. 
 *
 * There are two implementations: Method and MethodSignature
 */
class IMethodDescriptor {

public:
  
  virtual ~IMethodDescriptor() { }
  
  /**
   * Returns the method's name
   */
  virtual std::string getName() const = 0;
  
  /**
   * Returns method's return type
   */
  virtual IType* getReturnType() const = 0;
  
  /**
   * Returns an array of method arguments
   */
  virtual std::vector<MethodArgument*> getArguments() const = 0;
  
  /**
   * Tells index of this method in the container model or interface
   */
  virtual unsigned long getIndex() const = 0;
  
  /**
   * Returns function type corresponding to this method
   */
  virtual llvm::FunctionType* getLLVMFunctionType(IRGenerationContext& context,
                                                  ICallableObjectType* callableObject) const = 0;
  
  /**
   * Returns two if two method descriptors are the same in terms of their name, return type
   * and argument types
   */
  static bool compare(IMethodDescriptor* method1, IMethodDescriptor* method2);
};
  
} /* namespace yazyk */

#endif /* IMethodDescriptor_h */
