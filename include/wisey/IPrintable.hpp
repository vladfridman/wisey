//
//  IPrintable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IPrintable_h
#define IPrintable_h

#include <iostream>

namespace wisey {

/**
 * Indicates that the class can be printed into a stream
 */
class IPrintable {

public:
  
  virtual ~IPrintable() { }
  
  /**
   * Converts the class instance to string form and prints it to the given stream
   */
  virtual void printToStream(std::iostream& stream) const = 0;

};

} /* namespace wisey */
  
#endif /* IPrintable_h */
