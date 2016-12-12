//
//  TestConditionalExpression.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/12/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ConditionalExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Value.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/ConditionalExpression.hpp"

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

struct ConditionalExpressionTest : Test {
  IRGenerationContext mContext;
  NiceMock<MockExpression> mConditionExpression;
  NiceMock<MockExpression> mIfTrueExpression;
  NiceMock<MockExpression> mIfFalseExpression;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Function* mFunction;
  
  ConditionalExpressionTest() {
    LLVMContext &llvmContext = mContext.getLLVMContext();
    Value * ifTrueValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 3);
    Value * ifFalseValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 5);
    ON_CALL(mIfTrueExpression, generateIR(_)).WillByDefault(Return(ifTrueValue));
    ON_CALL(mIfFalseExpression, generateIR(_)).WillByDefault(Return(ifFalseValue));
    
    FunctionType* functionType =
      FunctionType::get(Type::getInt32Ty(llvmContext), false);
    mFunction = Function::Create(functionType, GlobalValue::InternalLinkage, "test");
    mContext.pushBlock(BasicBlock::Create(llvmContext, "entry", mFunction));
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~ConditionalExpressionTest() {
    delete mFunction;
  }
};

TEST_F(ConditionalExpressionTest, ConditionalExpressionRunWithFalse) {
  Value * conditionValue = ConstantInt::get(Type::getInt1Ty(mContext.getLLVMContext()), 0);
  ON_CALL(mConditionExpression, generateIR(_)).WillByDefault(testing::Return(conditionValue));

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

TEST_F(ConditionalExpressionTest, ConditionalExpressionRunWithTrue) {
  Value * conditionValue = ConstantInt::get(Type::getInt1Ty(mContext.getLLVMContext()), 1);
  ON_CALL(mConditionExpression, generateIR(_)).WillByDefault(testing::Return(conditionValue));
  
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

TEST_F(TestFileSampleRunner, ConditionalExpressionRunTrueConditionTest) {
  runFile("tests/samples/test_conditional_with_true.yz", "3");
}

TEST_F(TestFileSampleRunner, ConditionalExpressionRunFlaseConditionTest) {
  runFile("tests/samples/test_conditional_with_false.yz", "5");
}
