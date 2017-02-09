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
  
  std::string getName() const { return mName; }
  
  IType* getReturnType() const { return mReturnType; }
  
  std::vector<MethodArgument*> getArguments() { return mArguments; }
};

} /* namespace yazyk */

#endif /* Method_h */
