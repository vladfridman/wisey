//
//  IInterfaceTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IInterfaceTypeSpecifier_h
#define IInterfaceTypeSpecifier_h

#include "wisey/IObjectTypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents interface type specifier
   */
  class IInterfaceTypeSpecifier : public IObjectTypeSpecifier {
    
  public:
    
    virtual Interface* getType(IRGenerationContext& context) const = 0;
    
  };
  
} /* namespace wisey */

#endif /* IInterfaceTypeSpecifier_h */

