//
//  TestBoolean.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Boolean}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/Boolean.hpp"
#include "yazyk/IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

struct BooleanTest : public Test {
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  IRGenerationContext mContext;

  BooleanTest() {
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~BooleanTest() {
    delete mStringStream;
  }
};

TEST_F(BooleanTest, BooleanTrueConstantTest) {
  Value* irValue = True.generateIR(mContext);
  
  *mStringStream << *irValue;
  EXPECT_STREQ("i1 true", mStringStream->str().c_str());
}

TEST_F(BooleanTest, BooleanFalseConstantTest) {
  Value* irValue = False.generateIR(mContext);
  
  *mStringStream << *irValue;
  EXPECT_STREQ("i1 false", mStringStream->str().c_str());
}

TEST_F(TestFileSampleRunner, BooleanConstantTest) {
  runFile("tests/samples/test_boolean_constant.yz", "7");
}
