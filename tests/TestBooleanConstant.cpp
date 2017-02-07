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
#include "yazyk/BooleanConstant.hpp"
#include "yazyk/BooleanConstants.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

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

TEST_F(BooleanConstantTest, BooleanTrueConstantTest) {
  Value* irValue = BooleanConstants::BOOLEAN_TRUE->generateIR(mContext);
  
  *mStringStream << *irValue;
  EXPECT_STREQ("i1 true", mStringStream->str().c_str());
}

TEST_F(BooleanConstantTest, BooleanFalseConstantTest) {
  Value* irValue = BooleanConstants::BOOLEAN_FALSE->generateIR(mContext);
  
  *mStringStream << *irValue;
  EXPECT_STREQ("i1 false", mStringStream->str().c_str());
}

TEST_F(BooleanConstantTest, TestBooleanConstantType) {
  BooleanConstant booleanConstant(true);
  
  EXPECT_EQ(booleanConstant.getType(mContext), PrimitiveTypes::BOOLEAN_TYPE);
}

TEST_F(TestFileSampleRunner, BooleanConstantTest) {
  runFile("tests/samples/test_boolean_constant.yz", "7");
}

TEST_F(TestFileSampleRunner, BooleanVariableTest) {
  runFile("tests/samples/test_boolean_variable.yz", "5");
}
