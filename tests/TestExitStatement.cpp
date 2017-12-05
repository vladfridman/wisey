//
//  TestExitStatement.cpp
//  runtests
//
//  Created by Vladimir Fridman on 10/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ExitStatement}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/ExitStatement.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ExitStatementTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockExpression>* mMockExpression;
  ExitStatement* mExitStatement;
  BasicBlock* mBasicBlock;
  Function* mFunction;
  string mStringBuffer;
  raw_string_ostream* mStringStream;

public:
  
  ExitStatementTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mMockExpression(new NiceMock<MockExpression>()) {
    Value* intValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
    ON_CALL(*mMockExpression, generateIR(_, _)).WillByDefault(Return(intValue));
    ON_CALL(*mMockExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));

    mExitStatement = new ExitStatement(mMockExpression, 0);

    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    mFunction = Function::Create(functionType, GlobalValue::InternalLinkage, "test");
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~ExitStatementTest() {
    delete mMockExpression;
    delete mFunction;
    delete mStringStream;
  }
};

TEST_F(ExitStatementTest, exitStatementTest) {
  mExitStatement->generateIR(mContext);
  
  string expected =
  "\nentry:"
  "\n  call void @exit(i32 3)"
  "\n  unreachable\n";

  *mStringStream << *mBasicBlock;
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ExitStatementTest, exitStatementDeathTest) {
  Mock::AllowLeak(mMockExpression);
  Value* floatValue = ConstantFP::get(Type::getFloatTy(mLLVMContext), 3.5);
  ON_CALL(*mMockExpression, generateIR(_, _)).WillByDefault(Return(floatValue));
  ON_CALL(*mMockExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));

  EXPECT_EXIT(mExitStatement->generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Can not auto cast exit statement parameter to int");
}

TEST_F(TestFileSampleRunner, exitStatementRunTest) {
  compileAndRunFile("tests/samples/test_exit_statement.yz", 3);
}
