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
  PrimitiveTypeSpecifier booleanTypeSpecifier(PrimitiveTypes::BOOLEAN_TYPE, 1);
  PrimitiveTypeSpecifier charTypeSpecifier(PrimitiveTypes::CHAR_TYPE, 2);
  PrimitiveTypeSpecifier doubleTypeSpecifier(PrimitiveTypes::DOUBLE_TYPE, 3);
  PrimitiveTypeSpecifier floatTypeSpecifier(PrimitiveTypes::FLOAT_TYPE, 4);
  PrimitiveTypeSpecifier intTypeSpecifier(PrimitiveTypes::INT_TYPE, 5);
  PrimitiveTypeSpecifier longTypeSpecifier(PrimitiveTypes::LONG_TYPE, 6);
  PrimitiveTypeSpecifier stringTypeSpecifier(PrimitiveTypes::STRING_TYPE, 7);
  PrimitiveTypeSpecifier voidTypeSpecifier(PrimitiveTypes::VOID_TYPE, 8);

  EXPECT_EQ(PrimitiveTypes::BOOLEAN_TYPE, booleanTypeSpecifier.getType(mContext));
  EXPECT_EQ(PrimitiveTypes::CHAR_TYPE, charTypeSpecifier.getType(mContext));
  EXPECT_EQ(PrimitiveTypes::DOUBLE_TYPE, doubleTypeSpecifier.getType(mContext));
  EXPECT_EQ(PrimitiveTypes::FLOAT_TYPE, floatTypeSpecifier.getType(mContext));
  EXPECT_EQ(PrimitiveTypes::INT_TYPE, intTypeSpecifier.getType(mContext));
  EXPECT_EQ(PrimitiveTypes::LONG_TYPE, longTypeSpecifier.getType(mContext));
  EXPECT_EQ(PrimitiveTypes::STRING_TYPE, stringTypeSpecifier.getType(mContext));
  EXPECT_EQ(PrimitiveTypes::VOID_TYPE, voidTypeSpecifier.getType(mContext));

  EXPECT_EQ(1, booleanTypeSpecifier.getLine());
  EXPECT_EQ(2, charTypeSpecifier.getLine());
  EXPECT_EQ(3, doubleTypeSpecifier.getLine());
  EXPECT_EQ(4, floatTypeSpecifier.getLine());
  EXPECT_EQ(5, intTypeSpecifier.getLine());
  EXPECT_EQ(6, longTypeSpecifier.getLine());
  EXPECT_EQ(7, stringTypeSpecifier.getLine());
  EXPECT_EQ(8, voidTypeSpecifier.getLine());
}

TEST_F(PrimitiveTypeSpecifierTest, printToStreamTest) {
  PrimitiveTypeSpecifier booleanTypeSpecifier(PrimitiveTypes::BOOLEAN_TYPE, 0);
  stringstream booleanStringStream;
  booleanTypeSpecifier.printToStream(mContext, booleanStringStream);
  EXPECT_STREQ("boolean", booleanStringStream.str().c_str());
  
  PrimitiveTypeSpecifier charTypeSpecifier(PrimitiveTypes::CHAR_TYPE, 0);
  stringstream charStringStream;
  charTypeSpecifier.printToStream(mContext, charStringStream);
  EXPECT_STREQ("char", charStringStream.str().c_str());
  
  PrimitiveTypeSpecifier doubleTypeSpecifier(PrimitiveTypes::DOUBLE_TYPE, 0);
  stringstream doubleStringStream;
  doubleTypeSpecifier.printToStream(mContext, doubleStringStream);
  EXPECT_STREQ("double", doubleStringStream.str().c_str());
  
  PrimitiveTypeSpecifier floatTypeSpecifier(PrimitiveTypes::FLOAT_TYPE, 0);
  stringstream floatStringStream;
  floatTypeSpecifier.printToStream(mContext, floatStringStream);
  EXPECT_STREQ("float", floatStringStream.str().c_str());
  
  PrimitiveTypeSpecifier intTypeSpecifier(PrimitiveTypes::INT_TYPE, 0);
  stringstream intStringStream;
  intTypeSpecifier.printToStream(mContext, intStringStream);
  EXPECT_STREQ("int", intStringStream.str().c_str());

  PrimitiveTypeSpecifier longTypeSpecifier(PrimitiveTypes::LONG_TYPE, 0);
  stringstream longStringStream;
  longTypeSpecifier.printToStream(mContext, longStringStream);
  EXPECT_STREQ("long", longStringStream.str().c_str());

  PrimitiveTypeSpecifier stringTypeSpecifier(PrimitiveTypes::STRING_TYPE, 0);
  stringstream stringStringStream;
  stringTypeSpecifier.printToStream(mContext, stringStringStream);
  EXPECT_STREQ("string", stringStringStream.str().c_str());

  PrimitiveTypeSpecifier voidTypeSpecifier(PrimitiveTypes::VOID_TYPE, 0);
  stringstream voidStringStream;
  voidTypeSpecifier.printToStream(mContext, voidStringStream);
  EXPECT_STREQ("void", voidStringStream.str().c_str());
}
