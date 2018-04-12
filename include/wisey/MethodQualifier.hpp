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
   * Represents method qualifiers such as override and reveal
   */
  typedef enum MethodQualifierEnum {
    CONCEAL,
    OVERRIDE,
    REVEAL,
  } MethodQualifier;

  /**
   * Represents a set of method qualifiers
   */
  class MethodQualifiers {
    
    std::set<MethodQualifier> mMethodQualifierSet;
    int mLine;
    
  public:
    
    MethodQualifiers(int line) :
    mLine(line) {
    }
    
    ~MethodQualifiers() {
    }
    
    std::set<MethodQualifier>& getMethodQualifierSet() {
      return mMethodQualifierSet;
    }
    
    int getLine() {
      return mLine;
    }
  };
  
} /* namespace wisey */

#endif /* MethodQualifier_h */
