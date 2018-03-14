//
//  INodeTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 11/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef INodeTypeSpecifier_h
#define INodeTypeSpecifier_h

#include "wisey/IObjectTypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents node type specifier
   */
  class INodeTypeSpecifier : public IObjectTypeSpecifier {
    
  public:
    
    virtual const Node* getType(IRGenerationContext& context) const = 0;
    
  };
  
} /* namespace wisey */

#endif /* INodeTypeSpecifier_h */

