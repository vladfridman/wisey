//
//  VoidType.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/4/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef VoidType_h
#define VoidType_h

#include "yazyk/IType.hpp"

namespace yazyk {
  
/**
 * Represents void expression type
 */
class VoidType : public IType {
    
public:
  
  VoidType() { }
  
  ~VoidType() { }
  
  std::string getName() const override { return "void"; }
};
  
} /* namespace yazyk */

#endif /* VoidType_h */
