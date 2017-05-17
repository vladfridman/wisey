//
//  TestIfElseStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/24/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IfElseStatement}
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
#include "TestFileSampleRunner.hpp"
#include "wisey/Block.hpp"
#include "wisey/CompoundStatement.hpp"
#include "wisey/IfElseStatement.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace wisey;

struct IfElseStatementTest : Test {
  IRGenerationContext mContext;
  NiceMock<MockExpression> mCondition;
  NiceMock<MockStatement> mThenStatement;
  NiceMock<MockStatement> mElseStatement;
  Block mThenBlock;
  Block mElseBlock;
  CompoundStatement mThenCompoundStatement;
  CompoundStatement mElseCompoundStatement;
  
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Function* mFunction;
  
  IfElseStatementTest() : mThenCompoundStatement(mThenBlock), mElseCompoundStatement(mElseBlock) {
    LLVMContext &llvmContext = mContext.getLLVMContext();
    Value* conditionValue = ConstantInt::get(Type::getInt1Ty(llvmContext), 1);
    ON_CALL(mCondition, generateIR(_)).WillByDefault(Return(conditionValue));
    ON_CALL(mCondition, getType(_)).WillByDefault(Return(PrimitiveTypes::BOOLEAN_TYPE));
    Value* thenStatementValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 2);
    ON_CALL(mThenStatement, generateIR(_)).WillByDefault(Return(thenStatementValue));
    mThenBlock.getStatements().push_back(&mThenStatement);
    Value* elseStatementValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 3);
    ON_CALL(mElseStatement, generateIR(_)).WillByDefault(Return(elseStatementValue));
    mElseBlock.getStatements().push_back(&mElseStatement);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(llvmContext), false);
    mFunction = Function::Create(functionType, GlobalValue::InternalLinkage, "test");
    mContext.setBasicBlock(BasicBlock::Create(llvmContext, "entry", mFunction));
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~IfElseStatementTest() {
    delete mFunction;
    delete mStringStream;
  }
};

TEST_F(IfElseStatementTest, ifThenStatementSimpleTest) {
  IfElseStatement ifElseStatement(mCondition, mThenCompoundStatement, mElseCompoundStatement);
  ifElseStatement.generateIR(mContext);
  
  ASSERT_EQ(4ul, mFunction->size());
  Function::iterator iterator = mFunction->begin();
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("entry", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br i1 true, label %if.then, label %if.else");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("if.then", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br label %if.end");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(1ul, iterator->size());
  EXPECT_STREQ("if.else", iterator->getName().str().c_str());
  *mStringStream << iterator->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  br label %if.end");
  mStringBuffer.clear();
  
  iterator++;
  ASSERT_EQ(0ul, iterator->size());
  EXPECT_STREQ("if.end", iterator->getName().str().c_str());
}

TEST_F(TestFileSampleRunner, ifThenElseStatementRunTest) {
  runFile("tests/samples/test_if_then_else_statement.yz", "7");
}
