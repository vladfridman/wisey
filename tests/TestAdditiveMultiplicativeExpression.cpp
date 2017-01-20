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

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/AddditiveMultiplicativeExpression.hpp"
#include "yazyk/IRGenerationContext.hpp"

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
  MOCK_CONST_METHOD1(generateIR, Value* (IRGenerationContext&));
};

struct AddditiveMultiplicativeExpressionTest : Test {
  IRGenerationContext mContext;
  NiceMock<MockExpression> mLeftExpression;
  NiceMock<MockExpression> mRightExpression;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;

  AddditiveMultiplicativeExpressionTest() {
    LLVMContext &llvmContext = mContext.getLLVMContext();
    Value * leftValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 3);
    Value * rightValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 5);
    ON_CALL(mLeftExpression, generateIR(_)).WillByDefault(Return(leftValue));
    ON_CALL(mRightExpression, generateIR(_)).WillByDefault(Return(rightValue));
    mBasicBlock = BasicBlock::Create(llvmContext, "test");
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~AddditiveMultiplicativeExpressionTest() {
    delete mBasicBlock;
    delete mStringStream;
  }
};

TEST_F(AddditiveMultiplicativeExpressionTest, AdditionTest) {
  AddditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression);
  expression.generateIR(mContext);
  
  ASSERT_EQ(1ul, mBasicBlock->size());
  Instruction &instruction = mBasicBlock->front();
  *mStringStream << instruction;
  ASSERT_STREQ(mStringStream->str().c_str(), "  %add = add i32 3, 5");
}

TEST_F(AddditiveMultiplicativeExpressionTest, SubtractionTest) {
  AddditiveMultiplicativeExpression expression(mLeftExpression, '-', mRightExpression);
  expression.generateIR(mContext);
  
  ASSERT_EQ(1ul, mBasicBlock->size());
  Instruction &instruction = mBasicBlock->front();
  *mStringStream << instruction;
  ASSERT_STREQ(mStringStream->str().c_str(), "  %sub = sub i32 3, 5");
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
