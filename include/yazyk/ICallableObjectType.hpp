//
//  ICallableObjectType.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/9/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ICallableObjectType_h
#define ICallableObjectType_h

#include "yazyk/IType.hpp"
#include "yazyk/Method.hpp"

namespace yazyk {
  
/**
 * Represents an IType of a callable object such as a Model or an Interface
 */
class ICallableObjectType : public IType {
  
public:
  
  /**
   * Finds a method with a given name
   */
  virtual Method* findMethod(std::string methodName) const = 0;

};

} /* namespace yazyk */

#endif /* ICallableObjectType_h */
