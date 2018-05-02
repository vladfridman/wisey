//
//  PrimitiveTypes.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/4/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/PrimitiveTypes.hpp"

using namespace wisey;

BooleanType* PrimitiveTypes::BOOLEAN_TYPE = new BooleanType();

CharType* PrimitiveTypes::CHAR_TYPE = new CharType();

DoubleType* PrimitiveTypes::DOUBLE_TYPE = new DoubleType();

FloatType* PrimitiveTypes::FLOAT_TYPE = new FloatType();

IntType* PrimitiveTypes::INT_TYPE = new IntType();

LongType* PrimitiveTypes::LONG_TYPE = new LongType();

StringType* PrimitiveTypes::STRING_TYPE = new StringType();

VoidType* PrimitiveTypes::VOID_TYPE = new VoidType();

StringFormatType* PrimitiveTypes::STRING_FORMAT_TYPE = new StringFormatType();

bool PrimitiveTypes::isFloatType(const IType* type) {
  return type == FLOAT_TYPE || type == DOUBLE_TYPE;
}
