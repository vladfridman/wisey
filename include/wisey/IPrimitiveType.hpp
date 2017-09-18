//
//  IPrimitiveType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IPrimitiveType_h
#define IPrimitiveType_h

#include "wisey/IType.hpp"

namespace wisey {

/**
 * Represents a primitive type such as int, boolean, char, string
 */
class IPrimitiveType : public IType {

public:
  
  /**
   * Returns format string used in printf to print this type
   */
  virtual std::string getFormat() const = 0;

};
  
} /* namespace wisey */

#endif /* IPrimitiveType_h */
