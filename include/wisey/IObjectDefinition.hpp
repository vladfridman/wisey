//
//  IObjectDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IObjectDefinition_h
#define IObjectDefinition_h

#include "wisey/IGlobalStatement.hpp"

namespace wisey {
  
/**
 * Represents an object definition such as controller, model or interface definitions
 */
class IObjectDefinition : public IGlobalStatement {
};
  
} /* namespace wisey */

#endif /* IObjectDefinition_h */
