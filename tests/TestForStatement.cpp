//
//  TestForStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ForStatement}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "MockStatement.hpp"
#include "TestFileRunner.hpp"
#include "wisey/ForStatement.hpp"
#include "wisey/IRGenerationContext.hpp"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace wisey;

struct ForStatementTest : Test {
  IRGenerationContext mContext;
  NiceMock<MockStatement>* mStartStatement;
  NiceMock<MockExpression>* mConditionExpression;
  NiceMock<MockExpression>* mIncrementExpression;
  NiceMock<MockStatement>* mBodyStatement;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Function* mFunction;
  
  ForStatementTest() :
  mStartStatement(new NiceMock<MockStatement>()),
  mConditionExpression(new NiceMock<MockExpression>()),
  mIncrementExpression(new NiceMock<MockExpression>()),
  mBodyStatement(new NiceMock<MockStatement>()) {
    LLVMContext &llvmContext = mContext.getLLVMContext();
    Value* conditionStatementValue = ConstantInt::get(Type::getInt1Ty(llvmContext), 1);
    ON_CALL(*mConditionExpression, generateIR(_, _)).WillByDefault(Return(conditionStatementValue));
    Value* incrementExpressionValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 2);
    ON_CALL(*mIncrementExpression, generateIR(_, _))
    .WillByDefault(Return(incrementExpressionValue));
    
    FunctionType* functionType =
      FunctionType::get(Type::getInt32Ty(llvmContext), false);
    mFunction = Function::Create(functionType, GlobalValue::InternalLinkage, "test");
    mContext.setBasicBlock(BasicBlock::Create(llvmContext, "entry", mFunction));
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~ForStatementTest() {
    delete mFunction;
    delete mStringStream;
  }
};

TEST_F(ForStatementTest, forStatementSimpleTest) {
  ForStatement forStatement(mStartStatement,
                            mConditionExpression,
                            mIncrementExpression,
                            mBodyStatement,
                            0);
  forStatement.generateIR(mContext);
  
  ASSERT_EQ(5ul, mFunction->size());
  Function::iterator iterator = mFunction->begin();
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("entry", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br label %for.cond");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("for.cond", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br i1 true, label %for.body, label %for.end");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("for.body", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br label %for.inc");
  mStringBuffer.clear();

  iterator++;
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("for.inc", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br label %for.cond");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(0ul, iterator->size());
  EXPECT_STREQ("for.end", iterator->getName().str().c_str());
}

TEST_F(TestFileRunner, forStatementRunTest) {
  runFile("tests/samples/test_for_statement.yz", 90);
}

TEST_F(TestFileRunner, forStatementWithNoIncrementRunTest) {
  runFile("tests/samples/test_for_statement_with_no_increment.yz", 100);
}

TEST_F(TestFileRunner, forStatementWithNoDeclarationRunTest) {
  runFile("tests/samples/test_for_statement_with_no_declaration.yz", 10);
}

TEST_F(TestFileRunner, forStatementWithNoDeclarationNoIncrementRunTest) {
  runFile("tests/samples/test_for_statement_with_no_declaration_no_increment.yz", 11);
}

TEST_F(TestFileRunner, forStatementLocalVariableScopeRunDeathTest) {
  expectFailCompile("tests/samples/test_for_statement_scope.yz",
                    1,
                    "tests/samples/test_for_statement_scope.yz\\(10\\): Error: Undeclared variable 'i'");
}

TEST_F(TestFileRunner, unreachableForStatementRunDeathTest) {
  expectFailCompile("tests/samples/test_unreachable_for_statement.yz",
                    1,
                    "tests/samples/test_unreachable_for_statement.yz\\(9\\): Error: Statement unreachable");
}
