//
//  IThreadTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/2/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef IThreadTypeSpecifier_h
#define IThreadTypeSpecifier_h

#include "wisey/IObjectTypeSpecifier.hpp"
#include "wisey/Thread.hpp"

namespace wisey {
  
  /**
   * Represents model type specifier
   */
  class IThreadTypeSpecifier : public IObjectTypeSpecifier {
    
  public:
    
    virtual Thread* getType(IRGenerationContext& context) const = 0;
    
  };
  
} /* namespace wisey */

#endif /* IThreadTypeSpecifier_h */
