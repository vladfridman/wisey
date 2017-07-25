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

TEST_F(CharConstantTest, charConstantTest) {
  string stringBuffer;
  raw_string_ostream* stringStream = new raw_string_ostream(stringBuffer);
  
  Value* irValue = mCharConstant.generateIR(mContext);

  *stringStream << *irValue;
  EXPECT_STREQ("i16 121", stringStream->str().c_str());
}

TEST_F(CharConstantTest, charConstantTypeTest) {
  EXPECT_EQ(mCharConstant.getType(mContext), PrimitiveTypes::CHAR_TYPE);
}

TEST_F(CharConstantTest, releaseOwnershipDeathTest) {
  EXPECT_EXIT(mCharConstant.releaseOwnership(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Can not release ownership of a char constant, it is not a heap pointer");
}

TEST_F(CharConstantTest, addReferenceToOwnerDeathTest) {
  EXPECT_EXIT(mCharConstant.addReferenceToOwner(mContext, NULL),
              ::testing::ExitedWithCode(1),
              "Error: Can not add a reference to non owner type char constant expression");
}

TEST_F(CharConstantTest, existsInOuterScopeTest) {
  EXPECT_FALSE(mCharConstant.existsInOuterScope(mContext));
}

TEST_F(TestFileSampleRunner, charVariableRunTest) {
  runFile("tests/samples/test_char_variable.yz", "7");
}
