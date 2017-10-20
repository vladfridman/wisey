//
//  testAdditiveMultiplicativeExpression.cpp
//  Wisey
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
#include "wisey/AdditiveMultiplicativeExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Property;
using ::testing::Return;
using ::testing::StrEq;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace wisey;

struct AdditiveMultiplicativeExpressionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockExpression>* mLeftExpression;
  NiceMock<MockExpression>* mRightExpression;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;

  AdditiveMultiplicativeExpressionTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mLeftExpression(new NiceMock<MockExpression>()),
  mRightExpression(new NiceMock<MockExpression>()) {
    Value* leftValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
    Value* rightValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
    ON_CALL(*mLeftExpression, generateIR(_)).WillByDefault(Return(leftValue));
    ON_CALL(*mLeftExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    ON_CALL(*mRightExpression, generateIR(_)).WillByDefault(Return(rightValue));
    ON_CALL(*mRightExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    mBasicBlock = BasicBlock::Create(mLLVMContext, "test");
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~AdditiveMultiplicativeExpressionTest() {
    delete mBasicBlock;
    delete mStringStream;
  }

  static void printLeftExpression(IRGenerationContext& context, iostream& stream) {
    stream << "i";
  }

  static void printRightExpression(IRGenerationContext& context, iostream& stream) {
    stream << "j";
  }
};

TEST_F(AdditiveMultiplicativeExpressionTest, isConstantTest) {
  AdditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression);

  EXPECT_FALSE(expression.isConstant());
}

TEST_F(AdditiveMultiplicativeExpressionTest, getVariableTest) {
  AdditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression);
  
  EXPECT_EQ(expression.getVariable(mContext), nullptr);
}

TEST_F(AdditiveMultiplicativeExpressionTest, additionTest) {
  AdditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression);
  expression.generateIR(mContext);
  
  ASSERT_EQ(1ul, mBasicBlock->size());
  Instruction &instruction = mBasicBlock->front();
  *mStringStream << instruction;
  ASSERT_STREQ(mStringStream->str().c_str(), "  %add = add i32 3, 5");
}

TEST_F(AdditiveMultiplicativeExpressionTest, subtractionTest) {
  AdditiveMultiplicativeExpression expression(mLeftExpression, '-', mRightExpression);
  expression.generateIR(mContext);
  
  ASSERT_EQ(1ul, mBasicBlock->size());
  Instruction &instruction = mBasicBlock->front();
  *mStringStream << instruction;
  ASSERT_STREQ(mStringStream->str().c_str(), "  %sub = sub i32 3, 5");
}

TEST_F(AdditiveMultiplicativeExpressionTest, existsInOuterScopeTest) {
  AdditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression);
  
  EXPECT_FALSE(expression.existsInOuterScope(mContext));
}

TEST_F(AdditiveMultiplicativeExpressionTest, printToStreamTest) {
  AdditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression);
  
  stringstream stringStream;
  ON_CALL(*mLeftExpression, printToStream(_, _)).WillByDefault(Invoke(printLeftExpression));
  ON_CALL(*mRightExpression, printToStream(_, _)).WillByDefault(Invoke(printRightExpression));
  expression.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("i + j", stringStream.str().c_str());
}

TEST_F(AdditiveMultiplicativeExpressionTest, incompatibleTypesDeathTest) {
  Mock::AllowLeak(mLeftExpression);
  Mock::AllowLeak(mRightExpression);

  Value* rightValue = ConstantFP::get(Type::getFloatTy(mLLVMContext), 5.5);
  ON_CALL(*mRightExpression, generateIR(_)).WillByDefault(Return(rightValue));
  ON_CALL(*mRightExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));

  AdditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression);

  EXPECT_EXIT(expression.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Incompatible types in '\\+' operation");
}

TEST_F(AdditiveMultiplicativeExpressionTest, nonPrimitiveTypesDeathTest) {
  Mock::AllowLeak(mLeftExpression);
  Mock::AllowLeak(mRightExpression);
  
  string modelFullName = "systems.vos.wisey.compiler.tests.MShape";
  StructType* structType = StructType::create(mLLVMContext, modelFullName);
  Model* model = Model::newModel(modelFullName, structType);

  ON_CALL(*mLeftExpression, getType(_)).WillByDefault(Return(model));
  ON_CALL(*mRightExpression, getType(_)).WillByDefault(Return(model));
  
  AdditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression);
  
  EXPECT_EXIT(expression.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Can not do operation '\\+' on non-primitive types");
}

TEST_F(AdditiveMultiplicativeExpressionTest, voidTypesDeathTest) {
  Mock::AllowLeak(mLeftExpression);
  Mock::AllowLeak(mRightExpression);
  
  ON_CALL(*mLeftExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::VOID_TYPE));
  ON_CALL(*mRightExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::VOID_TYPE));
  
  AdditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression);
  
  EXPECT_EXIT(expression.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Can not use expressions of type VOID in a '\\+' operation");
}

TEST_F(AdditiveMultiplicativeExpressionTest, explicitCastNeededOnGenerateIRDeathTest) {
  Mock::AllowLeak(mLeftExpression);
  Mock::AllowLeak(mRightExpression);
  
  ON_CALL(*mLeftExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::LONG_TYPE));
  ON_CALL(*mRightExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));
  
  AdditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression);
  
  EXPECT_EXIT(expression.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Incompatible types in '\\+' operation that require an explicit cast");
}

TEST_F(AdditiveMultiplicativeExpressionTest, explicitCastNeededOnGetTypeDeathTest) {
  Mock::AllowLeak(mLeftExpression);
  Mock::AllowLeak(mRightExpression);
  
  ON_CALL(*mLeftExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::LONG_TYPE));
  ON_CALL(*mRightExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));
  
  AdditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression);
  
  EXPECT_EXIT(expression.getType(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Incompatible types in '\\+' operation that require an explicit cast");
}

TEST_F(AdditiveMultiplicativeExpressionTest, releaseOwnershipDeathTest) {
  Mock::AllowLeak(mLeftExpression);
  Mock::AllowLeak(mRightExpression);

  AdditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression);
  
  EXPECT_EXIT(expression.releaseOwnership(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Can not release ownership of an additive/multiplicative expression, "
              "it is not a heap pointer");
}

TEST_F(AdditiveMultiplicativeExpressionTest, addReferenceToOwnerDeathTest) {
  Mock::AllowLeak(mLeftExpression);
  Mock::AllowLeak(mRightExpression);
  
  AdditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression);
  
  EXPECT_EXIT(expression.addReferenceToOwner(mContext, NULL),
              ::testing::ExitedWithCode(1),
              "Error: Can not add a reference to non owner type "
              "additive/multiplicative expression");
}

TEST_F(TestFileSampleRunner, additionRunTest) {
  runFile("tests/samples/test_addition.yz", "7");
}

TEST_F(TestFileSampleRunner, subtractionRunTest) {
  runFile("tests/samples/test_subtraction.yz", "14");
}

TEST_F(TestFileSampleRunner, multiplicationRunTest) {
  runFile("tests/samples/test_multiplication.yz", "50");
}

TEST_F(TestFileSampleRunner, divisionRunTest) {
  runFile("tests/samples/test_division.yz", "5");
}

TEST_F(TestFileSampleRunner, additionWithCastRunTest) {
  runFile("tests/samples/test_addition_with_cast.yz", "3");
}

TEST_F(TestFileSampleRunner, subtractionOfDoubleRunTest) {
  runFile("tests/samples/test_subtraction_of_doubles.yz", "-2");
}
