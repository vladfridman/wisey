//
//  IBuildableConcreteObjectTypeSpecifier.h
//  Wisey
//
//  Created by Vladimir Fridman on 6/26/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IBuildableConcreteObjectTypeSpecifier_hpp
#define IBuildableConcreteObjectTypeSpecifier_hpp

#include "wisey/IBuildableConcreteObjectType.hpp"
#include "wisey/IObjectTypeSpecifier.hpp"

namespace wisey {
  
/**
 * Reprensents a type specifier for buildable concrete object types such as model and node
 */
class IBuildableConcreteObjectTypeSpecifier : public IObjectTypeSpecifier {
    
public:
  
  virtual const IBuildableConcreteObjectType* getType(IRGenerationContext& context) const = 0;
  
};
  
} /* namespace wisey */

#endif /* IBuildableConcreteObjectTypeSpecifier_hpp */
