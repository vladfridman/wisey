//
//  MethodArgument.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/9/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MethodArgument_h
#define MethodArgument_h

#include "wisey/IType.hpp"

namespace wisey {
  
  /**
   * Contains information about a method argument including its type and name
   */
  class MethodArgument {
    const IType* mType;
    std::string mName;
    
  public:
    
    MethodArgument(const IType* type, std::string name) : mType(type), mName(name) { }
    
    ~MethodArgument() { }
    
    const IType* getType() { return mType; }
    
    std::string getName() { return mName; }
  };
  
} /* namespace wisey */

#endif /* MethodArgument_h */

