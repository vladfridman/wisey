//
//  IObjectType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IObjectType_h
#define IObjectType_h

#include "wisey/IType.hpp"

namespace wisey {
  
/**
 * Represents an IType that is either model, controller or interface
 */
class IObjectType : public IType {
    
public:
  
  /**
   * Returns short name of the object without the package name.
   */
  virtual std::string getShortName() const = 0;
};
  
} /* namespace wisey */

#endif /* IObjectType_h */
