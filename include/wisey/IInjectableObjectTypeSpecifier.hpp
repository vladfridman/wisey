//
//  IInjectableObjectTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/14/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IInjectableObjectTypeSpecifier_h
#define IInjectableObjectTypeSpecifier_h

#include "wisey/IObjectTypeSpecifier.hpp"

namespace wisey {
  
/**
 * Reprensents a type specifier for injectable object types such as controller and interface
 */
class IInjectableObjectTypeSpecifier : public IObjectTypeSpecifier {
    
public:
  
  virtual const IObjectType* getType(IRGenerationContext& context) const = 0;
  
};

} /* namespace wisey */

#endif /* IInjectableObjectTypeSpecifier_h */
