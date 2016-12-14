//
//  testRelationalExpression.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/11/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link RelationalExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/RelationalExpression.hpp"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace yazyk;

class MockExpression : public IExpression {
public:
  MOCK_METHOD1(generateIR, Value* (IRGenerationContext&));
};

struct RelationalExpressionTest : Test {
  IRGenerationContext context;
  NiceMock<MockExpression> mLeftExpression;
  NiceMock<MockExpression> mRightExpression;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  RelationalExpressionTest() {
    Value * leftValue = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 3);
    Value * rightValue = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 5);
    ON_CALL(mLeftExpression, generateIR(_)).WillByDefault(Return(leftValue));
    ON_CALL(mRightExpression, generateIR(_)).WillByDefault(Return(rightValue));
    mBasicBlock = BasicBlock::Create(context.getLLVMContext(), "test");
    context.pushBlock(mBasicBlock);
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~RelationalExpressionTest() {
    delete mBasicBlock;
    delete mStringStream;
  }
};

TEST_F(RelationalExpressionTest, lessThanTest) {
  RelationalExpression expression(mLeftExpression, RELATIONAL_OPERATION_LT, mRightExpression);
  expression.generateIR(context);
  
  ASSERT_EQ(1ul, mBasicBlock->size());
  Instruction &instruction = mBasicBlock->front();
  *mStringStream << instruction;
  ASSERT_STREQ(mStringStream->str().c_str(), "  %cmp = icmp slt i32 3, 5");
}

TEST_F(RelationalExpressionTest, greaterThanOrEqualTest) {
  RelationalExpression expression(mLeftExpression, RELATIONAL_OPERATION_GE, mRightExpression);
  expression.generateIR(context);
  
  ASSERT_EQ(1ul, mBasicBlock->size());
  Instruction &instruction = mBasicBlock->front();
  *mStringStream << instruction;
  ASSERT_STREQ(mStringStream->str().c_str(), "  %cmp = icmp sge i32 3, 5");
}

TEST_F(TestFileSampleRunner, LessThanRunTest) {
  runFile("tests/samples/test_less_than.yz", "1");
}

TEST_F(TestFileSampleRunner, GreaterThanOrEqualRunTest) {
  runFile("tests/samples/test_greater_than_or_equal.yz", "1");
}

TEST_F(TestFileSampleRunner, EqualRunTest) {
  runFile("tests/samples/test_equal.yz", "0");
}

TEST_F(TestFileSampleRunner, NotEqualRunTest) {
  runFile("tests/samples/test_not_equal.yz", "1");
}
