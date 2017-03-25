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
#include "yazyk/PrimitiveTypes.hpp"
#include "yazyk/RelationalExpression.hpp"

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace yazyk;

class MockExpression : public IExpression {
public:
  MOCK_CONST_METHOD1(generateIR, Value* (IRGenerationContext&));
  MOCK_CONST_METHOD1(getType, IType* (IRGenerationContext&));
  MOCK_CONST_METHOD1(releaseOwnership, void (IRGenerationContext&));
};

struct RelationalExpressionTest : Test {
  IRGenerationContext mContext;
  NiceMock<MockExpression> mLeftExpression;
  NiceMock<MockExpression> mRightExpression;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  RelationalExpressionTest() {
    Value* leftValue = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 3);
    Value* rightValue = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 5);
    ON_CALL(mLeftExpression, generateIR(_)).WillByDefault(Return(leftValue));
    ON_CALL(mRightExpression, generateIR(_)).WillByDefault(Return(rightValue));
    mBasicBlock = BasicBlock::Create(mContext.getLLVMContext(), "test");
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~RelationalExpressionTest() {
    delete mBasicBlock;
    delete mStringStream;
  }
};

TEST_F(RelationalExpressionTest, lessThanTest) {
  RelationalExpression expression(mLeftExpression, RELATIONAL_OPERATION_LT, mRightExpression);
  expression.generateIR(mContext);
  
  ASSERT_EQ(1ul, mBasicBlock->size());
  EXPECT_EQ(expression.getType(mContext), PrimitiveTypes::BOOLEAN_TYPE);
  Instruction &instruction = mBasicBlock->front();
  *mStringStream << instruction;
  ASSERT_STREQ(mStringStream->str().c_str(), "  %cmp = icmp slt i32 3, 5");
}

TEST_F(RelationalExpressionTest, greaterThanOrEqualTest) {
  RelationalExpression expression(mLeftExpression, RELATIONAL_OPERATION_GE, mRightExpression);
  expression.generateIR(mContext);
  
  ASSERT_EQ(1ul, mBasicBlock->size());
  EXPECT_EQ(expression.getType(mContext), PrimitiveTypes::BOOLEAN_TYPE);
  Instruction &instruction = mBasicBlock->front();
  *mStringStream << instruction;
  ASSERT_STREQ(mStringStream->str().c_str(), "  %cmp = icmp sge i32 3, 5");
}

TEST_F(RelationalExpressionTest, releaseOwnershipDeathTest) {
  Mock::AllowLeak(&mLeftExpression);
  Mock::AllowLeak(&mRightExpression);
  RelationalExpression expression(mLeftExpression, RELATIONAL_OPERATION_GE, mRightExpression);

  EXPECT_EXIT(expression.releaseOwnership(mContext),
              ::testing::ExitedWithCode(1),
              "Can not release ownership of a relational expression result, "
              "it is not a heap pointer");
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
