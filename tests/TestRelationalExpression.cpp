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
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Value.h>

#include "TestFileSampleRunner.hpp"
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
  NiceMock<MockExpression> lhs;
  NiceMock<MockExpression> rhs;
  BasicBlock* basicBlock;
  string stringBuffer;
  raw_string_ostream* stringStream;
  
  RelationalExpressionTest() {
    Value * lhsValue = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 3);
    Value * rhsValue = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 5);
    ON_CALL(lhs, generateIR(_)).WillByDefault(Return(lhsValue));
    ON_CALL(rhs, generateIR(_)).WillByDefault(Return(rhsValue));
    basicBlock = BasicBlock::Create(context.getLLVMContext(), "test");
    context.pushBlock(basicBlock);
    stringStream = new raw_string_ostream(stringBuffer);
  }
  
  ~RelationalExpressionTest() {
    delete basicBlock;
    delete stringStream;
  }
};

TEST_F(RelationalExpressionTest, lessThanTest) {
  RelationalExpression expression(lhs, RELATIONAL_OPERATION_LT, rhs);
  expression.generateIR(context);
  
  ASSERT_EQ(1ul, basicBlock->size());
  Instruction &instruction = basicBlock->front();
  string stringBuffer;
  raw_string_ostream stringStream(stringBuffer);
  stringStream << instruction;
  ASSERT_STREQ(stringStream.str().c_str(), "  %cmp = icmp slt i32 3, 5");
}

TEST_F(RelationalExpressionTest, greaterThanOrEqualTest) {
  RelationalExpression expression(lhs, RELATIONAL_OPERATION_GE, rhs);
  expression.generateIR(context);
  
  ASSERT_EQ(1ul, basicBlock->size());
  Instruction &instruction = basicBlock->front();
  *stringStream << instruction;
  ASSERT_STREQ(stringStream->str().c_str(), "  %cmp = icmp sge i32 3, 5");
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
