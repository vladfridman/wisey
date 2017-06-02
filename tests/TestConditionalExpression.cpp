//
//  TestConditionalExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/12/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ConditionalExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/ConditionalExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace wisey;

struct ConditionalExpressionTest : Test {
  IRGenerationContext mContext;
  NiceMock<MockExpression>* mConditionExpression;
  NiceMock<MockExpression>* mIfTrueExpression;
  NiceMock<MockExpression>* mIfFalseExpression;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Function* mFunction;
  
  ConditionalExpressionTest() :
  mConditionExpression(new NiceMock<MockExpression>()),
  mIfTrueExpression(new NiceMock<MockExpression>()),
  mIfFalseExpression(new NiceMock<MockExpression>()) {
    LLVMContext &llvmContext = mContext.getLLVMContext();
    Value* ifTrueValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 3);
    Value* ifFalseValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 5);
    ON_CALL(*mIfTrueExpression, generateIR(_)).WillByDefault(Return(ifTrueValue));
    ON_CALL(*mIfTrueExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    ON_CALL(*mIfFalseExpression, generateIR(_)).WillByDefault(Return(ifFalseValue));
    ON_CALL(*mIfFalseExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    ON_CALL(*mConditionExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::BOOLEAN_TYPE));
    
    FunctionType* functionType =
      FunctionType::get(Type::getInt32Ty(llvmContext), false);
    mFunction = Function::Create(functionType, GlobalValue::InternalLinkage, "test");
    mContext.setBasicBlock(BasicBlock::Create(llvmContext, "entry", mFunction));
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~ConditionalExpressionTest() {
    delete mFunction;
    delete mStringStream;
  }
};

TEST_F(ConditionalExpressionTest, conditionalExpressionRunWithFalse) {
  Value * conditionValue = ConstantInt::get(Type::getInt1Ty(mContext.getLLVMContext()), 0);
  ON_CALL(*mConditionExpression, generateIR(_)).WillByDefault(testing::Return(conditionValue));

  ConditionalExpression expression(mConditionExpression, mIfTrueExpression, mIfFalseExpression);
  expression.generateIR(mContext);

  ASSERT_EQ(4ul, mFunction->size());
  Function::iterator iterator = mFunction->begin();
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("entry", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br i1 false, label %cond.true, label %cond.false");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("cond.true", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br label %cond.end");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("cond.false", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br label %cond.end");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("cond.end", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(),
               "  %cond = phi i32 [ 3, %cond.true ], [ 5, %cond.false ]");
}

TEST_F(ConditionalExpressionTest, conditionalExpressionRunWithTrue) {
  Value * conditionValue = ConstantInt::get(Type::getInt1Ty(mContext.getLLVMContext()), 1);
  ON_CALL(*mConditionExpression, generateIR(_)).WillByDefault(testing::Return(conditionValue));
 
  ConditionalExpression expression(mConditionExpression, mIfTrueExpression, mIfFalseExpression);
  expression.generateIR(mContext);
  
  ASSERT_EQ(4ul, mFunction->size());
  Function::iterator iterator = mFunction->begin();
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("entry", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br i1 true, label %cond.true, label %cond.false");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("cond.true", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br label %cond.end");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("cond.false", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br label %cond.end");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("cond.end", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(),
               "  %cond = phi i32 [ 3, %cond.true ], [ 5, %cond.false ]");
}

TEST_F(ConditionalExpressionTest, incompatibleTypesDeathTest) {
  Mock::AllowLeak(mConditionExpression);
  Mock::AllowLeak(mIfTrueExpression);
  Mock::AllowLeak(mIfFalseExpression);
  
  Value* trueValue = ConstantFP::get(Type::getFloatTy(mContext.getLLVMContext()), 5.5);
  ON_CALL(*mIfTrueExpression, generateIR(_)).WillByDefault(Return(trueValue));
  ON_CALL(*mIfTrueExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));
  
  ConditionalExpression expression(mConditionExpression, mIfTrueExpression, mIfFalseExpression);
  
  EXPECT_EXIT(expression.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types in conditional expression operation");
}

TEST_F(ConditionalExpressionTest, voidTypesDeathTest) {
  Mock::AllowLeak(mConditionExpression);
  Mock::AllowLeak(mIfTrueExpression);
  Mock::AllowLeak(mIfFalseExpression);
  
  ON_CALL(*mIfTrueExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::VOID_TYPE));
  ON_CALL(*mIfFalseExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::VOID_TYPE));
  
  ConditionalExpression expression(mConditionExpression, mIfTrueExpression, mIfFalseExpression);
  
  EXPECT_EXIT(expression.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Can not use expressions of type VOID in a conditional expression");
}

TEST_F(ConditionalExpressionTest, conditionIsNotBooleanDeathTest) {
  Mock::AllowLeak(mConditionExpression);
  Mock::AllowLeak(mIfTrueExpression);
  Mock::AllowLeak(mIfFalseExpression);
  
  ON_CALL(*mConditionExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::VOID_TYPE));
  
  ConditionalExpression expression(mConditionExpression, mIfTrueExpression, mIfFalseExpression);
  
  EXPECT_EXIT(expression.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Condition in a conditional expression is not of type BOOLEAN");
}

TEST_F(ConditionalExpressionTest, releaseOwnershipDeathTest) {
  Mock::AllowLeak(mConditionExpression);
  Mock::AllowLeak(mIfTrueExpression);
  Mock::AllowLeak(mIfFalseExpression);
  
  ConditionalExpression expression(mConditionExpression, mIfTrueExpression, mIfFalseExpression);
  
  EXPECT_EXIT(expression.releaseOwnership(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Can not release ownership of a conditional expression result, "
              "it is not a heap pointer");
}

TEST_F(TestFileSampleRunner, conditionalExpressionRunTrueConditionRunTest) {
  runFile("tests/samples/test_conditional_with_true.yz", "3");
}

TEST_F(TestFileSampleRunner, conditionalExpressionRunFlaseConditionRunTest) {
  runFile("tests/samples/test_conditional_with_false.yz", "5");
}
