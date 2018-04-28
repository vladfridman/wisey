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
#include "TestFileRunner.hpp"
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
  Block* mThenBlock;
  Block* mElseBlock;
  NiceMock<MockExpression>* mCondition;
  NiceMock<MockStatement>* mThenStatement;
  NiceMock<MockStatement>* mElseStatement;
  CompoundStatement* mThenCompoundStatement;
  CompoundStatement* mElseCompoundStatement;
  
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Function* mFunction;
  
  IfElseStatementTest() :
  mThenBlock(new Block()),
  mElseBlock(new Block()),
  mCondition(new NiceMock<MockExpression>()),
  mThenStatement(new NiceMock<MockStatement>()),
  mElseStatement(new NiceMock<MockStatement>()),
  mThenCompoundStatement(new CompoundStatement(mThenBlock, 0)),
  mElseCompoundStatement(new CompoundStatement(mElseBlock, 0)) {
    LLVMContext &llvmContext = mContext.getLLVMContext();
    Value* conditionValue = ConstantInt::get(Type::getInt1Ty(llvmContext), 1);
    ON_CALL(*mCondition, generateIR(_, _)).WillByDefault(Return(conditionValue));
    ON_CALL(*mCondition, getType(_)).WillByDefault(Return(PrimitiveTypes::BOOLEAN_TYPE));
    mThenBlock->getStatements().push_back(mThenStatement);
    mElseBlock->getStatements().push_back(mElseStatement);
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(llvmContext), false);
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

TEST_F(IfElseStatementTest, generateIRTest) {
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

TEST_F(TestFileRunner, ifThenElseStatementRunTest) {
  runFile("tests/samples/test_if_then_else_statement.yz", "7");
}

TEST_F(TestFileRunner, ifThenElseStatementWithClearedObjectsRunTest) {
  runFile("tests/samples/test_if_then_else_statement_with_cleared_objects.yz", "5");
}

TEST_F(TestFileRunner, ifElseIfElseRunTest) {
  runFile("tests/samples/test_if_else_if_else.yz", "5");
}

TEST_F(TestFileRunner, compareObjectToNullIfElseRunTest) {
  runFile("tests/samples/test_compare_object_to_null_if_else.yz", "5");
}
