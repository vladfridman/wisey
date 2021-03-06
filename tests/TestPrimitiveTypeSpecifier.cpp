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

#include "PrimitiveTypes.hpp"
#include "PrimitiveTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct PrimitiveTypeSpecifierTest : public Test {
  IRGenerationContext mContext;

  PrimitiveTypeSpecifierTest() { }
};

TEST_F(PrimitiveTypeSpecifierTest, creationTest) {
  PrimitiveTypeSpecifier booleanTypeSpecifier(PrimitiveTypes::BOOLEAN, 1);
  PrimitiveTypeSpecifier charTypeSpecifier(PrimitiveTypes::CHAR, 2);
  PrimitiveTypeSpecifier byteTypeSpecifier(PrimitiveTypes::BYTE, 3);
  PrimitiveTypeSpecifier doubleTypeSpecifier(PrimitiveTypes::DOUBLE, 4);
  PrimitiveTypeSpecifier floatTypeSpecifier(PrimitiveTypes::FLOAT, 5);
  PrimitiveTypeSpecifier intTypeSpecifier(PrimitiveTypes::INT, 6);
  PrimitiveTypeSpecifier longTypeSpecifier(PrimitiveTypes::LONG, 7);
  PrimitiveTypeSpecifier stringTypeSpecifier(PrimitiveTypes::STRING, 8);
  PrimitiveTypeSpecifier voidTypeSpecifier(PrimitiveTypes::VOID, 9);

  EXPECT_EQ(PrimitiveTypes::BOOLEAN, booleanTypeSpecifier.getType(mContext));
  EXPECT_EQ(PrimitiveTypes::CHAR, charTypeSpecifier.getType(mContext));
  EXPECT_EQ(PrimitiveTypes::BYTE, byteTypeSpecifier.getType(mContext));
  EXPECT_EQ(PrimitiveTypes::DOUBLE, doubleTypeSpecifier.getType(mContext));
  EXPECT_EQ(PrimitiveTypes::FLOAT, floatTypeSpecifier.getType(mContext));
  EXPECT_EQ(PrimitiveTypes::INT, intTypeSpecifier.getType(mContext));
  EXPECT_EQ(PrimitiveTypes::LONG, longTypeSpecifier.getType(mContext));
  EXPECT_EQ(PrimitiveTypes::STRING, stringTypeSpecifier.getType(mContext));
  EXPECT_EQ(PrimitiveTypes::VOID, voidTypeSpecifier.getType(mContext));

  EXPECT_EQ(1, booleanTypeSpecifier.getLine());
  EXPECT_EQ(2, charTypeSpecifier.getLine());
  EXPECT_EQ(3, byteTypeSpecifier.getLine());
  EXPECT_EQ(4, doubleTypeSpecifier.getLine());
  EXPECT_EQ(5, floatTypeSpecifier.getLine());
  EXPECT_EQ(6, intTypeSpecifier.getLine());
  EXPECT_EQ(7, longTypeSpecifier.getLine());
  EXPECT_EQ(8, stringTypeSpecifier.getLine());
  EXPECT_EQ(9, voidTypeSpecifier.getLine());
}

TEST_F(PrimitiveTypeSpecifierTest, printToStreamTest) {
  PrimitiveTypeSpecifier booleanTypeSpecifier(PrimitiveTypes::BOOLEAN, 0);
  stringstream booleanStringStream;
  booleanTypeSpecifier.printToStream(mContext, booleanStringStream);
  EXPECT_STREQ("boolean", booleanStringStream.str().c_str());
  
  PrimitiveTypeSpecifier charTypeSpecifier(PrimitiveTypes::CHAR, 0);
  stringstream charStringStream;
  charTypeSpecifier.printToStream(mContext, charStringStream);
  EXPECT_STREQ("char", charStringStream.str().c_str());
  
  PrimitiveTypeSpecifier byteTypeSpecifier(PrimitiveTypes::BYTE, 0);
  stringstream byteStringStream;
  byteTypeSpecifier.printToStream(mContext, byteStringStream);
  EXPECT_STREQ("byte", byteStringStream.str().c_str());

  PrimitiveTypeSpecifier doubleTypeSpecifier(PrimitiveTypes::DOUBLE, 0);
  stringstream doubleStringStream;
  doubleTypeSpecifier.printToStream(mContext, doubleStringStream);
  EXPECT_STREQ("double", doubleStringStream.str().c_str());
  
  PrimitiveTypeSpecifier floatTypeSpecifier(PrimitiveTypes::FLOAT, 0);
  stringstream floatStringStream;
  floatTypeSpecifier.printToStream(mContext, floatStringStream);
  EXPECT_STREQ("float", floatStringStream.str().c_str());
  
  PrimitiveTypeSpecifier intTypeSpecifier(PrimitiveTypes::INT, 0);
  stringstream intStringStream;
  intTypeSpecifier.printToStream(mContext, intStringStream);
  EXPECT_STREQ("int", intStringStream.str().c_str());

  PrimitiveTypeSpecifier longTypeSpecifier(PrimitiveTypes::LONG, 0);
  stringstream longStringStream;
  longTypeSpecifier.printToStream(mContext, longStringStream);
  EXPECT_STREQ("long", longStringStream.str().c_str());

  PrimitiveTypeSpecifier stringTypeSpecifier(PrimitiveTypes::STRING, 0);
  stringstream stringStringStream;
  stringTypeSpecifier.printToStream(mContext, stringStringStream);
  EXPECT_STREQ("string", stringStringStream.str().c_str());

  PrimitiveTypeSpecifier voidTypeSpecifier(PrimitiveTypes::VOID, 0);
  stringstream voidStringStream;
  voidTypeSpecifier.printToStream(mContext, voidStringStream);
  EXPECT_STREQ("void", voidStringStream.str().c_str());
}
