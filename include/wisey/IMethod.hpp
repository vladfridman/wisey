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
#include "wisey/IObjectType.hpp"

namespace wisey {

/**
 * Represents a method implementation that could either be static or non-static
 */
class IMethod : public IMethodDescriptor {

public:

  /**
   * Defines LLVM function for this method
   */
  virtual llvm::Function* defineFunction(IRGenerationContext& context, IObjectType* objectType) = 0;
  
  /**
   * Generates IR for this method in a given concrete object
   */
  virtual void generateIR(IRGenerationContext& context, IObjectType* objectType) const = 0;

};
  
} /* namespace wisey */

#endif /* IMethod_h */
