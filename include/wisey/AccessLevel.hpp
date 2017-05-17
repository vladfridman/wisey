//
//  AccessLevel.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/8/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef AccessLevel_h
#define AccessLevel_h

namespace wisey {

/**
 * Represents public and private access levels to model methods
 */
typedef enum AccessLevelEnum {
  PRIVATE_ACCESS,
  PUBLIC_ACCESS,
} AccessLevel;
  
} /* namespace wisey */

#endif /* AccessLevel_h */
