//
//  PrimitiveTypes.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/4/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/PrimitiveTypes.hpp"

using namespace wisey;

BooleanType* PrimitiveTypes::BOOLEAN = new BooleanType();

CharType* PrimitiveTypes::CHAR = new CharType();

DoubleType* PrimitiveTypes::DOUBLE = new DoubleType();

FloatType* PrimitiveTypes::FLOAT = new FloatType();

ByteType* PrimitiveTypes::BYTE = new ByteType();

IntType* PrimitiveTypes::INT = new IntType();

LongType* PrimitiveTypes::LONG = new LongType();

StringType* PrimitiveTypes::STRING = new StringType();

VoidType* PrimitiveTypes::VOID = new VoidType();

StringFormatType* PrimitiveTypes::STRING_FORMAT = new StringFormatType();

bool PrimitiveTypes::isFloatType(const IType* type) {
  return type == FLOAT || type == DOUBLE;
}
