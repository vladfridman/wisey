//
//  TestBooleanConstant.cpp
//  Wisey
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

#include "TestFileRunner.hpp"
#include "BooleanConstant.hpp"
#include "IRGenerationContext.hpp"
#include "PrimitiveTypes.hpp"

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
  BooleanConstant booleanTrue(true, 0);
  Value* irValue = booleanTrue.generateIR(mContext, PrimitiveTypes::VOID);
  
  *mStringStream << *irValue;
  EXPECT_STREQ("i1 true", mStringStream->str().c_str());
}

TEST_F(BooleanConstantTest, booleanFalseConstantTest) {
  BooleanConstant booleanFalse(false, 0);
  Value* irValue = booleanFalse.generateIR(mContext, PrimitiveTypes::VOID);
  
  *mStringStream << *irValue;
  EXPECT_STREQ("i1 false", mStringStream->str().c_str());
}

TEST_F(BooleanConstantTest, booleanConstantTypeTest) {
  BooleanConstant booleanConstant(true, 0);
  
  EXPECT_EQ(booleanConstant.getType(mContext), PrimitiveTypes::BOOLEAN);
}

TEST_F(BooleanConstantTest, isConstantTest) {
  BooleanConstant trueConstant(true, 0);
  
  EXPECT_TRUE(trueConstant.isConstant());
}

TEST_F(BooleanConstantTest, isAssignableTest) {
  BooleanConstant trueConstant(true, 0);
  
  EXPECT_FALSE(trueConstant.isAssignable());
}

TEST_F(BooleanConstantTest, printToStreamTest) {
  BooleanConstant trueConstant(true, 0);
  BooleanConstant falseConstant(false, 0);

  stringstream stringStreamTrue;
  trueConstant.printToStream(mContext, stringStreamTrue);
  EXPECT_STREQ("true", stringStreamTrue.str().c_str());

  stringstream stringStreamFalse;
  falseConstant.printToStream(mContext, stringStreamFalse);
  EXPECT_STREQ("false", stringStreamFalse.str().c_str());
}

TEST_F(TestFileRunner, booleanConstantRunTest) {
  runFile("tests/samples/test_boolean_constant.yz", 7);
}

TEST_F(TestFileRunner, booleanVariableRunTest) {
  runFile("tests/samples/test_boolean_variable.yz", 5);
}
