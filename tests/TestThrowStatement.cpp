//
//  TestThrowStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 4/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ThrowStatement}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MockExpression.hpp"
#include "MockType.hpp"
#include "TestFileSampleRunner.hpp"
#include "yazyk/ThrowStatement.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ThrowStatementTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockExpression> mMockExpression;
  NiceMock<MockType> mMockType;
  
  ThrowStatementTest() : mLLVMContext(mContext.getLLVMContext()) {
  }
};

TEST_F(ThrowStatementTest, wrongExpressionTypeDeathTest) {
  Mock::AllowLeak(&mMockType);
  Mock::AllowLeak(&mMockExpression);
  
  ON_CALL(mMockType, getTypeKind()).WillByDefault(Return(CONTROLLER_TYPE));
  ON_CALL(mMockExpression, getType(_)).WillByDefault(Return(&mMockType));
  ThrowStatement throwStatement(mMockExpression);
  
  EXPECT_EXIT(throwStatement.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Thrown object can only be a model");
}

TEST_F(ThrowStatementTest, modelExpressionTypeTest) {
  ON_CALL(mMockType, getTypeKind()).WillByDefault(Return(MODEL_TYPE));
  ON_CALL(mMockExpression, getType(_)).WillByDefault(Return(&mMockType));
  ThrowStatement throwStatement(mMockExpression);
  
  EXPECT_EQ(throwStatement.generateIR(mContext), nullptr);
}

TEST_F(TestFileSampleRunner, throwStatementParseRunTest) {
  runFile("tests/samples/test_throw.yz", "3");
}
