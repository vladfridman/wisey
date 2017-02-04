//
//  PrimitiveTypes.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/4/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef PrimitiveTypes_h
#define PrimitiveTypes_h

#include "yazyk/CharType.hpp"
#include "yazyk/DoubleType.hpp"
#include "yazyk/FloatType.hpp"
#include "yazyk/IntType.hpp"
#include "yazyk/LongType.hpp"

namespace yazyk {

/**
 * Has constants that stand for primitive types
 */
class PrimitiveTypes {

public:
  
  static CharType* CHAR_TYPE;
  static DoubleType* DOUBLE_TYPE;
  static FloatType* FLOAT_TYPE;
  static IntType* INT_TYPE;
  static LongType* LONG_TYPE;

};
  
} /* namespace yazyk */

#endif /* PrimitiveTypes_h */
