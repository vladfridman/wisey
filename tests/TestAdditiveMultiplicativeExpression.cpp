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
#include "yazyk/ILLVMBridge.hpp"

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

class MockLLVMBridge : public ILLVMBridge {
public:
  MOCK_CONST_METHOD5(createBinaryOperator, Value* (Instruction::BinaryOps, Value*, Value*, const Twine&, BasicBlock*));
};

struct AddditiveMultiplicativeExpressionTest : Test {
  IRGenerationContext context;
  NiceMock<MockExpression> lhs;
  NiceMock<MockExpression> rhs;
  NiceMock<MockLLVMBridge> llvmBridge;
  Value * lhsValue;
  Value * rhsValue;
  BasicBlock * bblock;
  
  AddditiveMultiplicativeExpressionTest() {
    lhsValue = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 3);
    rhsValue = ConstantInt::get(Type::getInt32Ty(context.getLLVMContext()), 5);

    bblock = BasicBlock::Create(context.getLLVMContext(), "test");
    context.pushBlock(bblock);

    ON_CALL(lhs, generateIR(_)).WillByDefault(Return(lhsValue));
    ON_CALL(rhs, generateIR(_)).WillByDefault(Return(rhsValue));
  }
};

TEST_F(AddditiveMultiplicativeExpressionTest, AdditionTest) {
  EXPECT_CALL(llvmBridge, createBinaryOperator(Instruction::Add,
                                               lhsValue,
                                               rhsValue,
                                               Property(&Twine::str, StrEq("add")),
                                               bblock));
  
  AddditiveMultiplicativeExpression expression(lhs, '+', rhs, &llvmBridge);
  expression.generateIR(context);
  
  delete bblock;
}

TEST_F(AddditiveMultiplicativeExpressionTest, SubtractionTest) {
  EXPECT_CALL(llvmBridge, createBinaryOperator(Instruction::Sub,
                                               lhsValue,
                                               rhsValue,
                                               Property(&Twine::str, StrEq("sub")),
                                               bblock));
  
  AddditiveMultiplicativeExpression expression(lhs, '-', rhs, &llvmBridge);
  expression.generateIR(context);
  
  delete bblock;
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
