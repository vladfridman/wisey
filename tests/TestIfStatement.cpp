//
//  TestIfStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IfStatement}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/IfStatement.hpp"
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

struct IfStatementTest : Test {
  IRGenerationContext mContext;
  NiceMock<MockExpression> mCondition;
  NiceMock<MockStatement> mThenStatement;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Function* mFunction;
  
  IfStatementTest() {
    LLVMContext &llvmContext = mContext.getLLVMContext();
    Value* conditionValue = ConstantInt::get(Type::getInt1Ty(llvmContext), 1);
    ON_CALL(mCondition, generateIR(_)).WillByDefault(Return(conditionValue));
    Value* thenStatementValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 2);
    ON_CALL(mThenStatement, generateIR(_)).WillByDefault(Return(thenStatementValue));
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(llvmContext), false);
    mFunction = Function::Create(functionType, GlobalValue::InternalLinkage, "test");
    mContext.setBasicBlock(BasicBlock::Create(llvmContext, "entry", mFunction));
    mContext.pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~IfStatementTest() {
    delete mFunction;
    delete mStringStream;
  }
};

TEST_F(IfStatementTest, IfStatementSimpleTest) {
  IfStatement ifStatement(mCondition, mThenStatement);
  ifStatement.generateIR(mContext);
  
  ASSERT_EQ(3ul, mFunction->size());
  Function::iterator iterator = mFunction->begin();
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("entry", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br i1 true, label %if.then, label %if.end");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("if.then", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br label %if.end");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(0ul, iterator->size());
  EXPECT_STREQ("if.end", iterator->getName().str().c_str());
}

TEST_F(TestFileSampleRunner, IfThenStatementRunTest) {
  runFile("tests/samples/test_if_then_statement.yz", "9");
}
