//
//  TestLogicalAndExpression.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LogicalAndExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/LogicalAndExpression.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::_;
using ::testing::ExitedWithCode;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

class MockExpression : public IExpression {
public:
  MOCK_CONST_METHOD1(generateIR, Value* (IRGenerationContext&));
};

struct LogicalAndExpressionTest : Test {
  IRGenerationContext mContext;
  NiceMock<MockExpression> mLeftExpression;
  NiceMock<MockExpression> mRightExpression;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Function* mFunction;
  Value* mFalseValue;
  Value* mTrueValue;
  
  LogicalAndExpressionTest() {
    LLVMContext &llvmContext = mContext.getLLVMContext();
    mFalseValue = ConstantInt::get(Type::getInt1Ty(llvmContext), 0);
    mTrueValue = ConstantInt::get(Type::getInt1Ty(llvmContext), 1);
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(llvmContext), false);
    mFunction = Function::Create(functionType, GlobalValue::InternalLinkage, "test");
    mContext.setBasicBlock(BasicBlock::Create(llvmContext, "entry", mFunction));
    mContext.pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~LogicalAndExpressionTest() {
    delete mFunction;
    delete mStringStream;
  }
};


TEST_F(LogicalAndExpressionTest, LogicalAndTrueValueTest) {
  ON_CALL(mLeftExpression, generateIR(_)).WillByDefault(Return(mTrueValue));
  ON_CALL(mRightExpression, generateIR(_)).WillByDefault(Return(mTrueValue));

  LogicalAndExpression expression(mLeftExpression, mRightExpression);
  expression.generateIR(mContext);
  
  ASSERT_EQ(3ul, mFunction->size());
  Function::iterator iterator = mFunction->begin();
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("entry", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br i1 true, label %land.rhs, label %land.end");
  mStringBuffer.clear();

  iterator++;
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("land.rhs", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br label %land.end");
  mStringBuffer.clear();

  iterator++;
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("land.end", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(),
               "  %land = phi i1 [ false, %entry ], [ true, %land.rhs ]");
  mStringBuffer.clear();
}

TEST_F(LogicalAndExpressionTest, LogicalAndFalseValueTest) {
  ON_CALL(mLeftExpression, generateIR(_)).WillByDefault(Return(mTrueValue));
  ON_CALL(mRightExpression, generateIR(_)).WillByDefault(Return(mFalseValue));
  
  LogicalAndExpression expression(mLeftExpression, mRightExpression);
  expression.generateIR(mContext);
  
  ASSERT_EQ(3ul, mFunction->size());
  Function::iterator iterator = mFunction->begin();
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("entry", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br i1 true, label %land.rhs, label %land.end");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("land.rhs", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br label %land.end");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("land.end", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(),
               "  %land = phi i1 [ false, %entry ], [ false, %land.rhs ]");
  mStringBuffer.clear();
}

TEST_F(TestFileSampleRunner, LogicalAndExpressionResultFalseRunTest) {
  runFile("tests/samples/test_logical_and_result_false.yz", "0");
}

TEST_F(TestFileSampleRunner, LogicalAndExpressionResultTrueRunTest) {
  runFile("tests/samples/test_logical_and_result_true.yz", "1");
}
