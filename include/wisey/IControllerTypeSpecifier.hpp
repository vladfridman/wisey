//
//  IControllerTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IControllerTypeSpecifier_h
#define IControllerTypeSpecifier_h

#include "wisey/IObjectTypeSpecifier.hpp"

namespace wisey {
  
/**
 * Represents controller type specifier
 */
class IControllerTypeSpecifier : public IObjectTypeSpecifier {

public:
  
  virtual const Controller* getType(IRGenerationContext& context) const = 0;

};
  
} /* namespace wisey */

#endif /* IControllerTypeSpecifier_h */
