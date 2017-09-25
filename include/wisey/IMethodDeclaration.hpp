//
//  IMethodDeclaration.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IMethodDeclaration_h
#define IMethodDeclaration_h

#include "wisey/IMethod.hpp"
#include "wisey/IRGenerationContext.hpp"

namespace wisey {

/**
 * Represents a method declaration that could either be static or non-static
 */
class IMethodDeclaration {
  
public:
  
  virtual ~IMethodDeclaration() { }
  
  /**
   * Returns object representing the method that will be saved in an object
   */
  virtual IMethod* createMethod(IRGenerationContext& context) const = 0;

};
  
} /* namespace wisey */

#endif /* IMethodDeclaration_h */