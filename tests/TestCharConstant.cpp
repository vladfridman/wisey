//
//  TestCharConstant.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link CharConstant}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/CharConstant.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

struct CharConstantTest : public ::testing::Test {
  IRGenerationContext mContext;
  CharConstant mCharConstant;

  CharConstantTest() : mCharConstant("y", 0) {
    TestPrefix::generateIR(mContext);
  }
};

TEST_F(CharConstantTest, charConstantTest) {
  string stringBuffer;
  raw_string_ostream* stringStream = new raw_string_ostream(stringBuffer);
  
  Value* irValue = mCharConstant.generateIR(mContext, PrimitiveTypes::VOID);

  *stringStream << *irValue;
  EXPECT_STREQ("i8 121", stringStream->str().c_str());
}

TEST_F(CharConstantTest, charConstantTypeTest) {
  EXPECT_EQ(mCharConstant.getType(mContext), PrimitiveTypes::CHAR);
}

TEST_F(CharConstantTest, printToStreamTest) {
  stringstream stringStream;
  mCharConstant.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("'y'", stringStream.str().c_str());
}

TEST_F(CharConstantTest, newLineTest) {
  string stringBuffer;
  raw_string_ostream* stringStream = new raw_string_ostream(stringBuffer);
  CharConstant constant("\n", 5);
  
  Value* irValue = constant.generateIR(mContext, PrimitiveTypes::VOID);
  
  *stringStream << *irValue;
  EXPECT_STREQ("i8 10", stringStream->str().c_str());
}

TEST_F(CharConstantTest, emptyValueDeathTest) {
  CharConstant constant("", 5);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(constant.generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(5): Error: Character in characted constant is not specified\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(CharConstantTest, stringInsteadOfCharDeathTest) {
  CharConstant constant("aa", 5);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(constant.generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(5): Error: String specified instead of a character constant\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(CharConstantTest, unknownEscapeSequenceDeathTest) {
  CharConstant constant("\\a", 5);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(constant.generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(5): Error: Unknown escape sequence\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(CharConstantTest, isConstantTest) {
  EXPECT_TRUE(mCharConstant.isConstant());
}

TEST_F(CharConstantTest, isAssignableTest) {
  EXPECT_FALSE(mCharConstant.isAssignable());
}

TEST_F(TestFileRunner, charVariableRunTest) {
  runFile("tests/samples/test_char_variable.yz", 7);
}
