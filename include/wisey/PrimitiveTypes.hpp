//
//  PrimitiveTypes.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/4/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef PrimitiveTypes_h
#define PrimitiveTypes_h

#include "wisey/BooleanType.hpp"
#include "wisey/CharType.hpp"
#include "wisey/DoubleType.hpp"
#include "wisey/FloatType.hpp"
#include "wisey/IntType.hpp"
#include "wisey/LongType.hpp"
#include "wisey/StringType.hpp"
#include "wisey/VoidType.hpp"

namespace wisey {

/**
 * Has constants that stand for primitive types
 */
class PrimitiveTypes {

public:
  
  static BooleanType* BOOLEAN_TYPE;
  static CharType* CHAR_TYPE;
  static DoubleType* DOUBLE_TYPE;
  static FloatType* FLOAT_TYPE;
  static IntType* INT_TYPE;
  static LongType* LONG_TYPE;
  static StringType* STRING_TYPE;
  static VoidType* VOID_TYPE;

  /**
   * Tells whether the given type is one of float types such as float or double
   */
  static bool isFloatType(const IType* type);

};
  
} /* namespace wisey */

#endif /* PrimitiveTypes_h */
