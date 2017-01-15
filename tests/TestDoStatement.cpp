//
//  TestDoStatement.cpp
//  Yazyk
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

#include "TestFileSampleRunner.hpp"
#include "yazyk/DoStatement.hpp"
#include "yazyk/IRGenerationContext.hpp"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace yazyk;

class MockExpression : public IExpression {
public:
  MOCK_CONST_METHOD1(generateIR, Value* (IRGenerationContext&));
};

class MockStatement : public IStatement {
public:
  MOCK_CONST_METHOD1(generateIR, Value* (IRGenerationContext&));
};

struct DoStatementTest : Test {
  IRGenerationContext mContext;
  NiceMock<MockExpression> mConditionExpression;
  NiceMock<MockStatement> mStatement;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Function* mFunction;
  
  DoStatementTest() {
    LLVMContext &llvmContext = mContext.getLLVMContext();
    Value* statementValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 3);
    ON_CALL(mStatement, generateIR(_)).WillByDefault(Return(statementValue));
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(llvmContext), false);
    mFunction = Function::Create(functionType, GlobalValue::InternalLinkage, "test");
    mContext.setBasicBlock(BasicBlock::Create(llvmContext, "entry", mFunction));
    mContext.pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~DoStatementTest() {
    delete mFunction;
    delete mStringStream;
  }
};

TEST_F(DoStatementTest, DoStatementSimpleTest) {
  Value * conditionValue = ConstantInt::get(Type::getInt1Ty(mContext.getLLVMContext()), 1);
  ON_CALL(mConditionExpression, generateIR(_)).WillByDefault(testing::Return(conditionValue));
  
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

TEST_F(TestFileSampleRunner, DoStatementRunTest) {
  runFile("tests/samples/test_do_statement.yz", "5");
}