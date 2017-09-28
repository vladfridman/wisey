//
//  IConcreteObjectDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IConcreteObjectDefinition_h
#define IConcreteObjectDefinition_h

#include "wisey/IObjectDefinition.hpp"

namespace wisey {
  
/**
 * Represents a concrete object definition such as controller, model or node definitions
 */
class IConcreteObjectDefinition : public IObjectDefinition {
};

} /* namespace wisey */

#endif /* IConcreteObjectDefinition_h */
