//
//  LongType.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/4/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef LongType_h
#define LongType_h

#include "yazyk/IType.hpp"

namespace yazyk {
  
/**
 * Represents long integer expression type
 */
class LongType : public IType {
  
public:
  
  LongType() { }
  
  ~LongType() { }
  
  std::string getName() const override { return "long"; }
};
  
} /* namespace yazyk */

#endif /* LongType_h */
