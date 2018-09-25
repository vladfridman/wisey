//
//  Argument.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/9/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Argument_h
#define Argument_h

#include "IType.hpp"

namespace wisey {
  
  /**
   * Contains information about a method argument including its type and name
   */
  class Argument {
    const IType* mType;
    std::string mName;
    
  public:
    
    Argument(const IType* type, std::string name) : mType(type), mName(name) { }
    
    ~Argument() { }
    
    const IType* getType() const { return mType; }
    
    std::string getName() const { return mName; }
  };
  
} /* namespace wisey */

#endif /* Argument_h */

