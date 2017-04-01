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
class AccessSpecifier {

public:
  
  static AccessSpecifier PRIVATE_ACCESS;
  static AccessSpecifier PUBLIC_ACCESS;
};
  
} /* namespace yazyk */

#endif /* AccessSpecifier_h */
