//
//  TestDoStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link DoStatement}
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
#include "wisey/DoStatement.hpp"
#include "wisey/IRGenerationContext.hpp"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace wisey;

struct DoStatementTest : Test {
  IRGenerationContext mContext;
  NiceMock<MockExpression>* mConditionExpression;
  NiceMock<MockStatement>* mStatement;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Function* mFunction;
  
  DoStatementTest() :
  mConditionExpression(new NiceMock<MockExpression>()),
  mStatement(new NiceMock<MockStatement>()) {
    LLVMContext &llvmContext = mContext.getLLVMContext();
    Value* statementValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 3);
    ON_CALL(*mStatement, generateIR(_)).WillByDefault(Return(statementValue));
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(llvmContext), false);
    mFunction = Function::Create(functionType, GlobalValue::InternalLinkage, "test");
    mContext.setBasicBlock(BasicBlock::Create(llvmContext, "entry", mFunction));
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~DoStatementTest() {
    delete mFunction;
    delete mStringStream;
  }
};

TEST_F(DoStatementTest, doStatementSimpleTest) {
  Value * conditionValue = ConstantInt::get(Type::getInt1Ty(mContext.getLLVMContext()), 1);
  ON_CALL(*mConditionExpression, generateIR(_, _)).WillByDefault(testing::Return(conditionValue));
  
  DoStatement doStatement(mStatement, mConditionExpression);
  doStatement.generateIR(mContext);
  
  ASSERT_EQ(4ul, mFunction->size());
  Function::iterator iterator = mFunction->begin();
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("entry", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br label %do.body");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("do.cond", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br i1 true, label %do.body, label %do.end");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("do.body", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br label %do.cond");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(0ul, iterator->size());
  EXPECT_STREQ("do.end", iterator->getName().str().c_str());
}

TEST_F(TestFileRunner, doStatementRunTest) {
  runFile("tests/samples/test_do_statement.yz", "5");
}
