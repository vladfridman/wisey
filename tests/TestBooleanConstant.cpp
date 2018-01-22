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

#include "TestFileSampleRunner.hpp"
#include "wisey/BooleanConstant.hpp"
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

TEST_F(BooleanConstantTest, getVariableTest) {
  BooleanConstant booleanTrue(true);
  vector<const IExpression*> arrayIndices;
  EXPECT_EQ(booleanTrue.getVariable(mContext, arrayIndices), nullptr);
}

TEST_F(BooleanConstantTest, booleanTrueConstantTest) {
  BooleanConstant booleanTrue(true);
  Value* irValue = booleanTrue.generateIR(mContext, PrimitiveTypes::VOID_TYPE);
  
  *mStringStream << *irValue;
  EXPECT_STREQ("i1 true", mStringStream->str().c_str());
}

TEST_F(BooleanConstantTest, booleanFalseConstantTest) {
  BooleanConstant booleanFalse(false);
  Value* irValue = booleanFalse.generateIR(mContext, PrimitiveTypes::VOID_TYPE);
  
  *mStringStream << *irValue;
  EXPECT_STREQ("i1 false", mStringStream->str().c_str());
}

TEST_F(BooleanConstantTest, booleanConstantTypeTest) {
  BooleanConstant booleanConstant(true);
  
  EXPECT_EQ(booleanConstant.getType(mContext), PrimitiveTypes::BOOLEAN_TYPE);
}

TEST_F(BooleanConstantTest, isConstantTest) {
  BooleanConstant trueConstant(true);

  EXPECT_TRUE(trueConstant.isConstant());
}

TEST_F(BooleanConstantTest, printToStreamTest) {
  BooleanConstant trueConstant(true);
  BooleanConstant falseConstant(false);

  stringstream stringStreamTrue;
  trueConstant.printToStream(mContext, stringStreamTrue);
  EXPECT_STREQ("true", stringStreamTrue.str().c_str());

  stringstream stringStreamFalse;
  falseConstant.printToStream(mContext, stringStreamFalse);
  EXPECT_STREQ("false", stringStreamFalse.str().c_str());
}

TEST_F(TestFileSampleRunner, booleanConstantRunTest) {
  runFile("tests/samples/test_boolean_constant.yz", "7");
}

TEST_F(TestFileSampleRunner, booleanVariableRunTest) {
  runFile("tests/samples/test_boolean_variable.yz", "5");
}
