//
//  BooleanType.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/5/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef BooleanType_h
#define BooleanType_h

#include "yazyk/IType.hpp"

namespace yazyk {
  
/**
 * Represents char expression type
 */
class BooleanType : public IType {
    
public:
  
  BooleanType() { }
  
  ~BooleanType() { }
  
  std::string getName() const override { return "boolean"; }
};

} /* namespace yazyk */

#endif /* BooleanType_h */
