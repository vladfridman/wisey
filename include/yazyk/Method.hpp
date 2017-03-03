//
//  Method.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/9/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Method_h
#define Method_h

#include "yazyk/IType.hpp"
#include "yazyk/MethodArgument.hpp"

namespace yazyk {

/**
 * Contains information about a method including its return type and all of its arguments
 */
class Method {
  std::string mName;
  IType* mReturnType;
  std::vector<MethodArgument*> mArguments;
  
public:
  
  Method(std::string name, IType* returnType, std::vector<MethodArgument*> arguments)
    : mName(name), mReturnType(returnType), mArguments(arguments) { }
  
  ~Method() { mArguments.clear(); }
  
  /**
   * Returns the method's name
   */
  std::string getName() const { return mName; }
  
  /**
   * Returns method's return type
   */
  IType* getReturnType() const { return mReturnType; }
  
  /**
   * Returns an array of method arguments
   */
  std::vector<MethodArgument*> getArguments() { return mArguments; }
  
  /**
   * Tells whether the two methods are equal in terms of their name, return type and arguments
   */
  bool equals(Method* method) const;
};

} /* namespace yazyk */

#endif /* Method_h */
