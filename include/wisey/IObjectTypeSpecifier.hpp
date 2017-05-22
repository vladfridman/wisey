//
//  IObjectTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IObjectTypeSpecifier_h
#define IObjectTypeSpecifier_h

#include "wisey/ITypeSpecifier.hpp"

namespace wisey {
  
/**
 * Represents an object type specifier that could either be an interface, a model or a controller
 */
class IObjectTypeSpecifier : public ITypeSpecifier {

public:
  
  /**
   * Return the type specifier name
   */
  virtual const std::string getName() const = 0;
};
  
} /* namespace wisey */

#endif /* IObjectTypeSpecifier_h */
