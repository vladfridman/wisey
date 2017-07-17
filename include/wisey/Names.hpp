//
//  Names.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 7/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Names_h
#define Names_h

#include <string>

namespace wisey {

/**
 * Contains common names of classes and methods used through out the wisey compiler
 */
class Names {
  
public:
  
  /**
   * Returns the name of the function that checks whether the given parameter is null
   * and throws an null pointer exception
   */
  static std::string getNPECheckFunction();
  
};

} /* namespace wisey */

#endif /* Names_h */
