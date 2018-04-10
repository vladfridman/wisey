//
//  MethodQualifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/10/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef MethodQualifier_h
#define MethodQualifier_h

#include <set>

namespace wisey {
  
  /**
   * Represents method qualifiers such as override and exposed
   */
  typedef enum MethodQualifierEnum {
    OVERRIDE,
    EXPOSED,
  } MethodQualifier;

  /**
   * Represents a set of method qualifiers
   */
  typedef std::set<MethodQualifier> MethodQualifierSet;
  
} /* namespace wisey */

#endif /* MethodQualifier_h */
