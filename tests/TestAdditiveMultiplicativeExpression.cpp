//
//  testAddditiveMultiplicativeExpression.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/9/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link AddditiveMultiplicativeExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/AddditiveMultiplicativeExpression.hpp"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Property;
using ::testing::Return;
using ::testing::StrEq;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace yazyk;

class MockExpression : public IExpression {
public:
  MOCK_METHOD1(generateIR, Value* (IRGenerationContext&));
};

struct AddditiveMultiplicativeExpressionTest : Test {
  IRGenerationContext context;
  NiceMock<MockExpression> lhs;
  NiceMock<MockExpression> rhs;
  BasicBlock* basicBlock;
  string stringBuffer;
  raw_string_ostream* stringStream;

  AddditiveMultiplicativeExpressionTest() {
    Value * lhsValue = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 3);
    Value * rhsValue = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 5);
    ON_CALL(lhs, generateIR(_)).WillByDefault(Return(lhsValue));
    ON_CALL(rhs, generateIR(_)).WillByDefault(Return(rhsValue));
    basicBlock = BasicBlock::Create(context.getLLVMContext(), "test");
    context.pushBlock(basicBlock);
    stringStream = new raw_string_ostream(stringBuffer);
  }
  
  ~AddditiveMultiplicativeExpressionTest() {
    delete basicBlock;
    delete stringStream;
  }
};

TEST_F(AddditiveMultiplicativeExpressionTest, AdditionTest) {
  AddditiveMultiplicativeExpression expression(lhs, '+', rhs);
  expression.generateIR(context);
  
  ASSERT_EQ(1ul, basicBlock->size());
  Instruction &instruction = basicBlock->front();
  string stringBuffer;
  raw_string_ostream stringStream(stringBuffer);
  stringStream << instruction;
  ASSERT_STREQ(stringStream.str().c_str(), "  %add = add i32 3, 5");
}

TEST_F(AddditiveMultiplicativeExpressionTest, SubtractionTest) {
  AddditiveMultiplicativeExpression expression(lhs, '-', rhs);
  expression.generateIR(context);
  
  ASSERT_EQ(1ul, basicBlock->size());
  Instruction &instruction = basicBlock->front();
  string stringBuffer;
  raw_string_ostream stringStream(stringBuffer);
  stringStream << instruction;
  ASSERT_STREQ(stringStream.str().c_str(), "  %sub = sub i32 3, 5");
}

TEST_F(TestFileSampleRunner, AdditionRunTest) {
  runFile("tests/samples/test_addition.yz", "7");
}

TEST_F(TestFileSampleRunner, SubtractionRunTest) {
  runFile("tests/samples/test_subtraction.yz", "14");
}

TEST_F(TestFileSampleRunner, MultiplicationRunTest) {
  runFile("tests/samples/test_multiplication.yz", "50");
}

TEST_F(TestFileSampleRunner, DivisionRunTest) {
  runFile("tests/samples/test_division.yz", "5");
}
