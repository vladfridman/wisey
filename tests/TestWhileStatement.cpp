//
//  TestWhileStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link WhileStatement}
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
#include "wisey/IRGenerationContext.hpp"
#include "wisey/WhileStatement.hpp"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace wisey;

struct WhileStatementTest : Test {
  IRGenerationContext mContext;
  NiceMock<MockExpression>* mConditionExpression;
  NiceMock<MockStatement>* mStatement;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Function* mFunction;
  
  WhileStatementTest() :
  mConditionExpression(new NiceMock<MockExpression>()),
  mStatement(new NiceMock<MockStatement>()) {
    LLVMContext &llvmContext = mContext.getLLVMContext();
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(llvmContext), false);
    mFunction = Function::Create(functionType, GlobalValue::InternalLinkage, "test");
    mContext.setBasicBlock(BasicBlock::Create(llvmContext, "entry", mFunction));
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~WhileStatementTest() {
    delete mFunction;
    delete mStringStream;
  }
};

TEST_F(WhileStatementTest, whileStatementSimpleTest) {
  Value * conditionValue = ConstantInt::get(Type::getInt1Ty(mContext.getLLVMContext()), 1);
  ON_CALL(*mConditionExpression, generateIR(_, _)).WillByDefault(testing::Return(conditionValue));
  
  WhileStatement whileStatement(mConditionExpression, mStatement);
  whileStatement.generateIR(mContext);
  
  ASSERT_EQ(4ul, mFunction->size());
  Function::iterator iterator = mFunction->begin();
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("entry", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br label %while.cond");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("while.cond", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br i1 true, label %while.body, label %while.end");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("while.body", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br label %while.cond");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(0ul, iterator->size());
  EXPECT_STREQ("while.end", iterator->getName().str().c_str());
}

TEST_F(TestFileRunner, whileStatementRunTest) {
  runFile("tests/samples/test_while_statement.yz", "10");
}

TEST_F(TestFileRunner, unreachableWhileStatementRunDeathTest) {
  expectFailCompile("tests/samples/test_unreachable_while_statement.yz",
                    1,
                    "tests/samples/test_unreachable_while_statement.yz\\(9\\): Error: Statement unreachable");
}
