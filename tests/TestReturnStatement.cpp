//
//  TestReturnStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ReturnStatement}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/FunctionDeclaration.hpp"
#include "yazyk/node.hpp"
#include "yazyk/ReturnStatement.hpp"

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
  MOCK_METHOD1(generateIR, Value* (IRGenerationContext&));
};

struct ReturnStatementTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockExpression> mExpression;
  string mStringBuffer;
  raw_string_ostream* mStringStream;

public:
  ReturnStatementTest() {
    Value * value = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 3);
    ON_CALL(mExpression, generateIR(_)).WillByDefault(Return(value));

    mStringStream = new raw_string_ostream(mStringBuffer);
  }

  ~ReturnStatementTest() {
    delete mStringStream;
  }
};

TEST_F(ReturnStatementTest, ParentFunctionIsNullDeathTest) {
  mContext.pushBlock(BasicBlock::Create(mContext.getLLVMContext(), "entry"));
  ReturnStatement returnStatement(mExpression);

  Mock::AllowLeak(&mExpression);
  EXPECT_EXIT(returnStatement.generateIR(mContext),
              ExitedWithCode(1),
              "No corresponding method found for RETURN");
}

TEST_F(ReturnStatementTest, ParentFunctionIsIncopatableTypeDeathTest) {
  LLVMContext &llvmContext = mContext.getLLVMContext();

  FunctionType* functionType = FunctionType::get(Type::getVoidTy(llvmContext), false);
  Function* function = Function::Create(functionType, GlobalValue::InternalLinkage, "test");

  mContext.pushBlock(BasicBlock::Create(llvmContext, "entry", function));
  ReturnStatement returnStatement(mExpression);
  
  Mock::AllowLeak(&mExpression);
  EXPECT_EXIT(returnStatement.generateIR(mContext),
              ExitedWithCode(1),
              "Can not cast return value to function type");
}

TEST_F(ReturnStatementTest, ParentFunctionIntTest) {
  LLVMContext &llvmContext = mContext.getLLVMContext();
  FunctionType* functionType = FunctionType::get(Type::getInt64Ty(llvmContext), false);
  Function* function = Function::Create(functionType, GlobalValue::InternalLinkage, "test");
  BasicBlock* basicBlock = BasicBlock::Create(llvmContext, "entry", function);
  mContext.pushBlock(basicBlock);
  ReturnStatement returnStatement(mExpression);

  returnStatement.generateIR(mContext);
  
  ASSERT_EQ(2ul, basicBlock->size());
  *mStringStream << *basicBlock;
  string expected = string() +
    "\nentry:"
    "\n  %conv = zext i32 3 to i64" +
    "\n  ret i64 %conv\n";
  ASSERT_STREQ(mStringStream->str().c_str(), expected.c_str());
  
  delete function;
}

TEST_F(TestFileSampleRunner, ReturnStatementRunTest) {
  runFile("tests/samples/test_return_int.yz", "5");
}
