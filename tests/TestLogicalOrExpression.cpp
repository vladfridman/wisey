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
#include "TestFileSampleRunner.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LogicalOrExpression.hpp"
#include "wisey/PrimitiveTypes.hpp"

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

TEST_F(LogicalOrExpressionTest, getVariableTest) {
  LogicalOrExpression expression(mLeftExpression, mRightExpression);
  
  EXPECT_EQ(expression.getVariable(mContext), nullptr);
}

TEST_F(LogicalOrExpressionTest, logicalAndTrueValueTest) {
  ON_CALL(*mLeftExpression, generateIR(_)).WillByDefault(Return(mTrueValue));
  ON_CALL(*mRightExpression, generateIR(_)).WillByDefault(Return(mFalseValue));
  
  LogicalOrExpression expression(mLeftExpression, mRightExpression);
  expression.generateIR(mContext);
  
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
  ON_CALL(*mLeftExpression, generateIR(_)).WillByDefault(Return(mFalseValue));
  ON_CALL(*mRightExpression, generateIR(_)).WillByDefault(Return(mFalseValue));
  
  LogicalOrExpression expression(mLeftExpression, mRightExpression);
  expression.generateIR(mContext);
  
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
  LogicalOrExpression expression(mLeftExpression, mRightExpression);
  
  EXPECT_EQ(expression.getType(mContext), PrimitiveTypes::BOOLEAN_TYPE);
}

TEST_F(LogicalOrExpressionTest, existsInOuterScopeTest) {
  LogicalOrExpression expression(mLeftExpression, mRightExpression);
  
  EXPECT_FALSE(expression.existsInOuterScope(mContext));
}

TEST_F(LogicalOrExpressionTest, printToStreamTest) {
  LogicalOrExpression expression(mLeftExpression, mRightExpression);
  
  stringstream stringStream;
  ON_CALL(*mLeftExpression, printToStream(_, _)).WillByDefault(Invoke(printLeftExpression));
  ON_CALL(*mRightExpression, printToStream(_, _)).WillByDefault(Invoke(printRightExpression));
  expression.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("a || b", stringStream.str().c_str());
}

TEST_F(LogicalOrExpressionTest, releaseOwnershipDeathTest) {
  Mock::AllowLeak(&mLeftExpression);
  Mock::AllowLeak(&mRightExpression);
  
  LogicalOrExpression expression(mLeftExpression, mRightExpression);
  
  EXPECT_EXIT(expression.releaseOwnership(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Can not release ownership of a logical OR operation result, "
              "it is not a heap pointer");
}

TEST_F(LogicalOrExpressionTest, addReferenceToOwnerDeathTest) {
  Mock::AllowLeak(&mLeftExpression);
  Mock::AllowLeak(&mRightExpression);
  
  LogicalOrExpression expression(mLeftExpression, mRightExpression);
  
  EXPECT_EXIT(expression.addReferenceToOwner(mContext, NULL),
              ::testing::ExitedWithCode(1),
              "Error: Can not add a reference to non owner type logical OR operation expression");
}

TEST_F(TestFileSampleRunner, LogicalOrExpressionResultFalseRunTest) {
  runFile("tests/samples/test_logical_or_result_false.yz", "0");
}

TEST_F(TestFileSampleRunner, LogicalOrExpressionResultTrueRunTest) {
  runFile("tests/samples/test_logical_or_result_true.yz", "1");
}
