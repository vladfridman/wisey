//
//  IObjectElementDeclaration.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IObjectElementDeclaration_h
#define IObjectElementDeclaration_h

#include "wisey/IObjectElement.hpp"

namespace wisey {
  
class IRGenerationContext;

/**
 * Represents a declaration of an object element such as a method or a field
 */
class IObjectElementDeclaration {
    
public:
  
  virtual ~IObjectElementDeclaration() { }
  
  /**
   * Returns a represention of an object element: a method or a field
   */
  virtual IObjectElement* declare(IRGenerationContext& context) const = 0;

};

} /* namespace wisey */


#endif /* IObjectElementDeclaration_h */
