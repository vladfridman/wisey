//
//  IObjectTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/9/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IObjectTypeSpecifier_h
#define IObjectTypeSpecifier_h

#include "wisey/ITypeSpecifier.hpp"

namespace wisey {

/**
 * Reprensents a type specifier for object types such as controller, model or interface
 */
class IObjectTypeSpecifier : public ITypeSpecifier {
  
public:
  
  virtual bool isOwner() const = 0;
};
  
} /* namespace wisey */

#endif /* IObjectTypeSpecifier_h */
