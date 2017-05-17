//
//  Environment.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Environment_h
#define Environment_h

namespace wisey {
  
/**
 * Contains information abut the environment on which the wisey code will be run
 */
class Environment {

public:
  
  /**
   * Returns the size of memory address in bytes.
   *
   * It should return 8 for 64 bit memory and 4 for 32 bit memory
   */
  static unsigned short getAddressSizeInBytes();
  
  /**
   * Default memory allignment
   *
   * https://www.ibm.com/developerworks/library/pa-dalign/
   */
  static unsigned int getDefaultMemoryAllignment();
  
};
  
} /* namespace wisey */

#endif /* Environment_h */
