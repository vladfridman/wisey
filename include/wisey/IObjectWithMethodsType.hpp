//
//  IObjectWithMethodsType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IObjectWithMethodsType_h
#define IObjectWithMethodsType_h

#include "wisey/IObjectType.hpp"

namespace wisey {
  
class IMethodDescriptor;
  
/**
 * Represents an object that has methods such as a Model or an Interface
 */
class IObjectWithMethodsType : public virtual IObjectType {
  
public:
  
  /**
   * Finds a method with a given name
   */
  virtual IMethodDescriptor* findMethod(std::string methodName) const = 0;
};

} /* namespace wisey */

#endif /* IObjectWithMethodsType_h */
