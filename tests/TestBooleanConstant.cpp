//
//  TestBooleanConstant.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link BooleanConstant}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "wisey/BooleanConstant.hpp"
#include "wisey/BooleanConstants.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct BooleanConstantTest : public Test {
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  IRGenerationContext mContext;

  BooleanConstantTest() {
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~BooleanConstantTest() {
    delete mStringStream;
  }
};

TEST_F(BooleanConstantTest, booleanTrueConstantTest) {
  Value* irValue = BooleanConstants::BOOLEAN_TRUE->generateIR(mContext);
  
  *mStringStream << *irValue;
  EXPECT_STREQ("i1 true", mStringStream->str().c_str());
}

TEST_F(BooleanConstantTest, booleanFalseConstantTest) {
  Value* irValue = BooleanConstants::BOOLEAN_FALSE->generateIR(mContext);
  
  *mStringStream << *irValue;
  EXPECT_STREQ("i1 false", mStringStream->str().c_str());
}

TEST_F(BooleanConstantTest, booleanConstantTypeTest) {
  BooleanConstant booleanConstant(true);
  
  EXPECT_EQ(booleanConstant.getType(mContext), PrimitiveTypes::BOOLEAN_TYPE);
}

TEST_F(BooleanConstantTest, releaseOwnershipDeathTest) {
  BooleanConstant booleanConstant(true);
  
  EXPECT_EXIT(booleanConstant.releaseOwnership(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Can not release ownership of a boolean constant, it is not a heap pointer");
}

TEST_F(TestFileSampleRunner, booleanConstantRunTest) {
  runFile("tests/samples/test_boolean_constant.yz", "7");
}

TEST_F(TestFileSampleRunner, booleanVariableRunTest) {
  runFile("tests/samples/test_boolean_variable.yz", "5");
}
