//
//  PrimitiveTypes.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/4/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "yazyk/PrimitiveTypes.hpp"

using namespace yazyk;

BooleanType* PrimitiveTypes::BOOLEAN_TYPE = new BooleanType();

CharType* PrimitiveTypes::CHAR_TYPE = new CharType();

DoubleType* PrimitiveTypes::DOUBLE_TYPE = new DoubleType();

FloatType* PrimitiveTypes::FLOAT_TYPE = new FloatType();

IntType* PrimitiveTypes::INT_TYPE = new IntType();

LongType* PrimitiveTypes::LONG_TYPE = new LongType();

VoidType* PrimitiveTypes::VOID_TYPE = new VoidType();
