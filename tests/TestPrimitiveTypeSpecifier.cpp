//
//  TestPrimitiveTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/18/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link PrimitiveTypeSpecifier}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct PrimitiveTypeSpecifierTest : public Test {
  IRGenerationContext mContext;

  PrimitiveTypeSpecifierTest() { }
};

TEST_F(PrimitiveTypeSpecifierTest, creationTest) {
  PrimitiveTypeSpecifier intTypeSpecifier(PrimitiveTypes::INT_TYPE);

  EXPECT_EQ(intTypeSpecifier.getType(mContext), PrimitiveTypes::INT_TYPE);
}

TEST_F(PrimitiveTypeSpecifierTest, printToStreamTest) {
  PrimitiveTypeSpecifier booleanTypeSpecifier(PrimitiveTypes::BOOLEAN_TYPE);
  stringstream booleanStringStream;
  booleanTypeSpecifier.printToStream(booleanStringStream);
  EXPECT_STREQ("boolean", booleanStringStream.str().c_str());
  
  PrimitiveTypeSpecifier charTypeSpecifier(PrimitiveTypes::CHAR_TYPE);
  stringstream charStringStream;
  charTypeSpecifier.printToStream(charStringStream);
  EXPECT_STREQ("char", charStringStream.str().c_str());
  
  PrimitiveTypeSpecifier doubleTypeSpecifier(PrimitiveTypes::DOUBLE_TYPE);
  stringstream doubleStringStream;
  doubleTypeSpecifier.printToStream(doubleStringStream);
  EXPECT_STREQ("double", doubleStringStream.str().c_str());
  
  PrimitiveTypeSpecifier floatTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
  stringstream floatStringStream;
  floatTypeSpecifier.printToStream(floatStringStream);
  EXPECT_STREQ("float", floatStringStream.str().c_str());
  
  PrimitiveTypeSpecifier intTypeSpecifier(PrimitiveTypes::INT_TYPE);
  stringstream intStringStream;
  intTypeSpecifier.printToStream(intStringStream);
  EXPECT_STREQ("int", intStringStream.str().c_str());

  PrimitiveTypeSpecifier longTypeSpecifier(PrimitiveTypes::LONG_TYPE);
  stringstream longStringStream;
  longTypeSpecifier.printToStream(longStringStream);
  EXPECT_STREQ("long", longStringStream.str().c_str());

  PrimitiveTypeSpecifier stringTypeSpecifier(PrimitiveTypes::STRING_TYPE);
  stringstream stringStringStream;
  stringTypeSpecifier.printToStream(stringStringStream);
  EXPECT_STREQ("string", stringStringStream.str().c_str());

  PrimitiveTypeSpecifier voidTypeSpecifier(PrimitiveTypes::VOID_TYPE);
  stringstream voidStringStream;
  voidTypeSpecifier.printToStream(voidStringStream);
  EXPECT_STREQ("void", voidStringStream.str().c_str());
}
