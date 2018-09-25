//
//  TestLogicalOrExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LogicalOrExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "IRGenerationContext.hpp"
#include "LogicalOrExpression.hpp"
#include "PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::ExitedWithCode;
using ::testing::Invoke;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LogicalOrExpressionTest : Test {
  IRGenerationContext mContext;
  NiceMock<MockExpression>* mLeftExpression;
  NiceMock<MockExpression>* mRightExpression;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Function* mFunction;
  Value* mFalseValue;
  Value* mTrueValue;
  
  LogicalOrExpressionTest() :
  mLeftExpression(new NiceMock<MockExpression>()),
  mRightExpression(new NiceMock<MockExpression>()) {
    TestPrefix::generateIR(mContext);
    
    LLVMContext &llvmContext = mContext.getLLVMContext();
    mFalseValue = ConstantInt::get(Type::getInt1Ty(llvmContext), 0);
    mTrueValue = ConstantInt::get(Type::getInt1Ty(llvmContext), 1);
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(llvmContext), false);
    mFunction = Function::Create(functionType, GlobalValue::InternalLinkage, "test");
    mContext.setBasicBlock(BasicBlock::Create(llvmContext, "entry", mFunction));
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~LogicalOrExpressionTest() {
    delete mFunction;
    delete mStringStream;
  }

  static void printLeftExpression(IRGenerationContext& context, iostream& stream) {
    stream << "a";
  }
  
  static void printRightExpression(IRGenerationContext& context, iostream& stream) {
    stream << "b";
  }
};

TEST_F(LogicalOrExpressionTest, logicalAndTrueValueTest) {
  ON_CALL(*mLeftExpression, generateIR(_, _)).WillByDefault(Return(mTrueValue));
  ON_CALL(*mLeftExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::BOOLEAN));
  ON_CALL(*mRightExpression, generateIR(_, _)).WillByDefault(Return(mFalseValue));
  ON_CALL(*mRightExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::BOOLEAN));

  LogicalOrExpression expression(mLeftExpression, mRightExpression, 0);
  expression.generateIR(mContext, PrimitiveTypes::VOID);
  
  ASSERT_EQ(3ul, mFunction->size());
  Function::iterator iterator = mFunction->begin();
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("entry", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br i1 true, label %lor.end, label %lor.rhs");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("lor.rhs", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br label %lor.end");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("lor.end", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(),
               "  %lor = phi i1 [ true, %entry ], [ false, %lor.rhs ]");
  mStringBuffer.clear();
}

TEST_F(LogicalOrExpressionTest, logicalAndFalseValueTest) {
  ON_CALL(*mLeftExpression, generateIR(_, _)).WillByDefault(Return(mFalseValue));
  ON_CALL(*mLeftExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::BOOLEAN));
  ON_CALL(*mRightExpression, generateIR(_, _)).WillByDefault(Return(mFalseValue));
  ON_CALL(*mRightExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::BOOLEAN));

  LogicalOrExpression expression(mLeftExpression, mRightExpression, 0);
  expression.generateIR(mContext, PrimitiveTypes::VOID);
  
  ASSERT_EQ(3ul, mFunction->size());
  Function::iterator iterator = mFunction->begin();
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("entry", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br i1 false, label %lor.end, label %lor.rhs");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("lor.rhs", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br label %lor.end");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("lor.end", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(),
               "  %lor = phi i1 [ true, %entry ], [ false, %lor.rhs ]");
  mStringBuffer.clear();
}

TEST_F(LogicalOrExpressionTest, logicalOrExpressionTypeTest) {
  LogicalOrExpression expression(mLeftExpression, mRightExpression, 0);
  
  EXPECT_EQ(expression.getType(mContext), PrimitiveTypes::BOOLEAN);
}

TEST_F(LogicalOrExpressionTest, isConstantTest) {
  LogicalOrExpression expression(mLeftExpression, mRightExpression, 0);

  EXPECT_FALSE(expression.isConstant());
}

TEST_F(LogicalOrExpressionTest, isAssignableTest) {
  LogicalOrExpression expression(mLeftExpression, mRightExpression, 0);
  
  EXPECT_FALSE(expression.isAssignable());
}

TEST_F(LogicalOrExpressionTest, printToStreamTest) {
  LogicalOrExpression expression(mLeftExpression, mRightExpression, 0);
  
  stringstream stringStream;
  ON_CALL(*mLeftExpression, printToStream(_, _)).WillByDefault(Invoke(printLeftExpression));
  ON_CALL(*mRightExpression, printToStream(_, _)).WillByDefault(Invoke(printRightExpression));
  expression.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("a || b", stringStream.str().c_str());
}

TEST_F(TestFileRunner, LogicalOrExpressionResultFalseRunTest) {
  runFile("tests/samples/test_logical_or_result_false.yz", 0);
}

TEST_F(TestFileRunner, LogicalOrExpressionResultTrueRunTest) {
  runFile("tests/samples/test_logical_or_result_true.yz", 1);
}
