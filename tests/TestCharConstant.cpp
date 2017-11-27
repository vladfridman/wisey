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

#include "TestFileSampleRunner.hpp"
#include "wisey/CharConstant.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

struct CharConstantTest : public ::testing::Test {
  IRGenerationContext mContext;
  CharConstant mCharConstant;

  CharConstantTest() : mCharConstant('y') { }
};

TEST_F(CharConstantTest, getVariableTest) {
  EXPECT_EQ(mCharConstant.getVariable(mContext), nullptr);
}

TEST_F(CharConstantTest, charConstantTest) {
  string stringBuffer;
  raw_string_ostream* stringStream = new raw_string_ostream(stringBuffer);
  
  Value* irValue = mCharConstant.generateIR(mContext, IR_GENERATION_NORMAL);

  *stringStream << *irValue;
  EXPECT_STREQ("i16 121", stringStream->str().c_str());
}

TEST_F(CharConstantTest, charConstantTypeTest) {
  EXPECT_EQ(mCharConstant.getType(mContext), PrimitiveTypes::CHAR_TYPE);
}

TEST_F(CharConstantTest, printToStreamTest) {
  stringstream stringStream;
  mCharConstant.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("'y'", stringStream.str().c_str());
}

TEST_F(CharConstantTest, isConstantTest) {
  EXPECT_TRUE(mCharConstant.isConstant());
}

TEST_F(TestFileSampleRunner, charVariableRunTest) {
  runFile("tests/samples/test_char_variable.yz", "7");
}
