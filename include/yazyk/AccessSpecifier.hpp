//
//  AccessSpecifier.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/8/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef AccessSpecifier_h
#define AccessSpecifier_h

namespace yazyk {

/**
 * Represents public and private access levels to model methods
 */
typedef enum AccessSpecifierEnum {
  PRIVATE_ACCESS,
  PUBLIC_ACCESS,
} AccessSpecifier;
  
} /* namespace yazyk */

#endif /* AccessSpecifier_h */
