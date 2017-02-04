//
//  IntType.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/4/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IntType_h
#define IntType_h

#include "yazyk/IType.hpp"

namespace yazyk {
  
/**
 * Represents integer expression type
 */
class IntType : public IType {
  
public:
  
  IntType() { }
  
  ~IntType() { }
  
  std::string getName() const override { return "int"; }
};
  
} /* namespace yazyk */

#endif /* IntType_h */
