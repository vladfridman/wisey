//
//  TestIfStatement.cpp
//  Wisey
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

#include "MockExpression.hpp"
#include "MockStatement.hpp"
#include "TestFileRunner.hpp"
#include "Block.hpp"
#include "CompoundStatement.hpp"
#include "IfStatement.hpp"
#include "IRGenerationContext.hpp"
#include "PrimitiveTypes.hpp"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace wisey;

struct IfStatementTest : Test {
  IRGenerationContext mContext;
  NiceMock<MockExpression>* mCondition;
  NiceMock<MockStatement>* mThenStatement;
  Block* mThenBlock;
  CompoundStatement* mThenCompoundStatement;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Function* mFunction;
  
  IfStatementTest() :
  mCondition(new NiceMock<MockExpression>()),
  mThenStatement(new NiceMock<MockStatement>()),
  mThenBlock(new Block()),
  mThenCompoundStatement(new CompoundStatement(mThenBlock, 0)) {
    LLVMContext &llvmContext = mContext.getLLVMContext();
    Value* conditionValue = ConstantInt::get(Type::getInt1Ty(llvmContext), 1);
    ON_CALL(*mCondition, generateIR(_, _)).WillByDefault(Return(conditionValue));
    ON_CALL(*mCondition, getType(_)).WillByDefault(Return(PrimitiveTypes::BOOLEAN));
    mThenBlock->getStatements().push_back(mThenStatement);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(llvmContext), false);
    mFunction = Function::Create(functionType, GlobalValue::InternalLinkage, "test");
    mContext.setBasicBlock(BasicBlock::Create(llvmContext, "entry", mFunction));
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~IfStatementTest() {
    delete mFunction;
    delete mStringStream;
  }
};

TEST_F(IfStatementTest, generateIRTest) {
  IfStatement ifStatement(mCondition, mThenCompoundStatement);
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

TEST_F(TestFileRunner, ifThenStatementRunTest) {
  runFile("tests/samples/test_if_then_statement.yz", 9);
}

TEST_F(TestFileRunner, ifStatemenScopeRunTest) {
  runFile("tests/samples/test_fibonacci_model_calculator.yz", 21);
}

TEST_F(TestFileRunner, compareObjectToNullIfRunTest) {
  runFile("tests/samples/test_compare_object_to_null_if.yz", 3);
}

TEST_F(TestFileRunner, unreachableIfStatementRunDeathTest) {
  expectFailCompile("tests/samples/test_unreachable_if_statement.yz",
                    1,
                    "tests/samples/test_unreachable_if_statement.yz\\(9\\): Error: Statement unreachable");
}
