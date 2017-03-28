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

namespace yazyk {
  
class IMethodDescriptor;
  
/**
 * Represents an IType of a callable object such as a Model or an Interface
 */
class ICallableObjectType : public virtual IType {
  
public:
  
  /**
   * Finds a method with a given name
   */
  virtual IMethodDescriptor* findMethod(std::string methodName) const = 0;

  /**
   * Returns name of the global variable containing name of this Callable Object in string format
   */
  virtual std::string getObjectNameGlobalVariableName() const = 0;
};

} /* namespace yazyk */

#endif /* ICallableObjectType_h */
