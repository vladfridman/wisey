//
//  testAdditiveMultiplicativeExpression.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/9/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link AdditiveMultiplicativeExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileSampleRunner.hpp"
#include "yazyk/AdditiveMultiplicativeExpression.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Property;
using ::testing::Return;
using ::testing::StrEq;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace yazyk;

struct AdditiveMultiplicativeExpressionTest : Test {
  IRGenerationContext mContext;
  NiceMock<MockExpression> mLeftExpression;
  NiceMock<MockExpression> mRightExpression;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;

  AdditiveMultiplicativeExpressionTest() {
    LLVMContext &llvmContext = mContext.getLLVMContext();
    Value * leftValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 3);
    Value * rightValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 5);
    ON_CALL(mLeftExpression, generateIR(_)).WillByDefault(Return(leftValue));
    ON_CALL(mLeftExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    ON_CALL(mRightExpression, generateIR(_)).WillByDefault(Return(rightValue));
    ON_CALL(mRightExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    mBasicBlock = BasicBlock::Create(llvmContext, "test");
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~AdditiveMultiplicativeExpressionTest() {
    delete mBasicBlock;
    delete mStringStream;
  }
};

TEST_F(AdditiveMultiplicativeExpressionTest, AdditionTest) {
  AdditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression);
  expression.generateIR(mContext);
  
  ASSERT_EQ(1ul, mBasicBlock->size());
  Instruction &instruction = mBasicBlock->front();
  *mStringStream << instruction;
  ASSERT_STREQ(mStringStream->str().c_str(), "  %add = add i32 3, 5");
}

TEST_F(AdditiveMultiplicativeExpressionTest, SubtractionTest) {
  AdditiveMultiplicativeExpression expression(mLeftExpression, '-', mRightExpression);
  expression.generateIR(mContext);
  
  ASSERT_EQ(1ul, mBasicBlock->size());
  Instruction &instruction = mBasicBlock->front();
  *mStringStream << instruction;
  ASSERT_STREQ(mStringStream->str().c_str(), "  %sub = sub i32 3, 5");
}

TEST_F(AdditiveMultiplicativeExpressionTest, IncompatibleTypesDeathTest) {
  Mock::AllowLeak(&mLeftExpression);
  Mock::AllowLeak(&mRightExpression);

  Value* rightValue = ConstantFP::get(Type::getFloatTy(mContext.getLLVMContext()), 5.5);
  ON_CALL(mRightExpression, generateIR(_)).WillByDefault(Return(rightValue));
  ON_CALL(mRightExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));

  AdditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression);

  EXPECT_EXIT(expression.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types in '\\+' operation");
}

TEST_F(AdditiveMultiplicativeExpressionTest, NonPrimitiveTypesDeathTest) {
  Mock::AllowLeak(&mLeftExpression);
  Mock::AllowLeak(&mRightExpression);
  
  StructType* structType = StructType::create(mContext.getLLVMContext(), "Shape");
  vector<Interface*> interfaces;
  map<string, ModelField*> fields;
  vector<Method*> methods;
  Model* model = new Model("Shape", structType, fields, methods, interfaces);

  ON_CALL(mLeftExpression, getType(_)).WillByDefault(Return(model));
  ON_CALL(mRightExpression, getType(_)).WillByDefault(Return(model));
  
  AdditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression);
  
  EXPECT_EXIT(expression.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Can not do operation '\\+' on non-primitive types");
}

TEST_F(AdditiveMultiplicativeExpressionTest, VoidTypesDeathTest) {
  Mock::AllowLeak(&mLeftExpression);
  Mock::AllowLeak(&mRightExpression);
  
  ON_CALL(mLeftExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::VOID_TYPE));
  ON_CALL(mRightExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::VOID_TYPE));
  
  AdditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression);
  
  EXPECT_EXIT(expression.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Can not use expressions of type VOID in a '\\+' operation");
}

TEST_F(AdditiveMultiplicativeExpressionTest, ExplicitCastNeededOnGenerateIRDeathTest) {
  Mock::AllowLeak(&mLeftExpression);
  Mock::AllowLeak(&mRightExpression);
  
  ON_CALL(mLeftExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::LONG_TYPE));
  ON_CALL(mRightExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));
  
  AdditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression);
  
  EXPECT_EXIT(expression.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types in '\\+' operation that require an explicit cast");
}

TEST_F(AdditiveMultiplicativeExpressionTest, ExplicitCastNeededOnGetTypeDeathTest) {
  Mock::AllowLeak(&mLeftExpression);
  Mock::AllowLeak(&mRightExpression);
  
  ON_CALL(mLeftExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::LONG_TYPE));
  ON_CALL(mRightExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));
  
  AdditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression);
  
  EXPECT_EXIT(expression.getType(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types in '\\+' operation that require an explicit cast");
}

TEST_F(AdditiveMultiplicativeExpressionTest, releaseOwnershipDeathTest) {
  Mock::AllowLeak(&mLeftExpression);
  Mock::AllowLeak(&mRightExpression);

  AdditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression);
  
  EXPECT_EXIT(expression.releaseOwnership(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Can not release ownership of an additive/multiplicative expression, "
              "it is not a heap pointer");
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

TEST_F(TestFileSampleRunner, AdditionWithCastRunTest) {
  runFile("tests/samples/test_addition_with_cast.yz", "3");
}

TEST_F(TestFileSampleRunner, SubtractionOfDoubleRunTest) {
  runFile("tests/samples/test_subtraction_of_doubles.yz", "-2");
}
