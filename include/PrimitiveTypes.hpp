//
//  PrimitiveTypes.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/4/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef PrimitiveTypes_h
#define PrimitiveTypes_h

#include "BooleanType.hpp"
#include "ByteType.hpp"
#include "CharType.hpp"
#include "DoubleType.hpp"
#include "FloatType.hpp"
#include "IntType.hpp"
#include "LongType.hpp"
#include "StringType.hpp"
#include "StringFormatType.hpp"
#include "VoidType.hpp"

namespace wisey {
  
  /**
   * Has constants that stand for primitive types
   */
  class PrimitiveTypes {
    
  public:
    
    static BooleanType* BOOLEAN;
    static CharType* CHAR;
    static DoubleType* DOUBLE;
    static FloatType* FLOAT;
    static ByteType* BYTE;
    static IntType* INT;
    static LongType* LONG;
    static StringType* STRING;
    static VoidType* VOID;
    static StringFormatType* STRING_FORMAT;
    
    /**
     * Tells whether the given type is one of float types such as float or double
     */
    static bool isFloatType(const IType* type);
    
    /**
     * Tells whether the given type is a number type
     */
    static bool isNumberType(const IType* type);

  };
  
} /* namespace wisey */

#endif /* PrimitiveTypes_h */

