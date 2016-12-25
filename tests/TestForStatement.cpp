//
//  TestForStatement.cpp
//  Yazyk
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

#include "TestFileSampleRunner.hpp"
#include "yazyk/ForStatement.hpp"
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

struct ForStatementTest : Test {
  IRGenerationContext mContext;
  NiceMock<MockStatement> mStartStatement;
  NiceMock<MockStatement> mConditionStatement;
  NiceMock<MockExpression> mIncrementExpression;
  NiceMock<MockStatement> mBodyStatement;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Function* mFunction;
  
  ForStatementTest() {
    LLVMContext &llvmContext = mContext.getLLVMContext();
    Value* startStatementValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 0);
    ON_CALL(mStartStatement, generateIR(_)).WillByDefault(Return(startStatementValue));
    Value* conditionStatementValue = ConstantInt::get(Type::getInt1Ty(llvmContext), 1);
    ON_CALL(mConditionStatement, generateIR(_)).WillByDefault(Return(conditionStatementValue));
    Value* incrementExpressionValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 2);
    ON_CALL(mIncrementExpression, generateIR(_)).WillByDefault(Return(incrementExpressionValue));
    Value* bodyStatementValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 1);
    ON_CALL(mBodyStatement, generateIR(_)).WillByDefault(Return(bodyStatementValue));
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(llvmContext), false);
    mFunction = Function::Create(functionType, GlobalValue::InternalLinkage, "test");
    mContext.pushBlock(BasicBlock::Create(llvmContext, "entry", mFunction));
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~ForStatementTest() {
    delete mFunction;
    delete mStringStream;
  }
};

TEST_F(ForStatementTest, ForStatementSimpleTest) {
  ForStatement forStatement(mStartStatement,
                            mConditionStatement,
                            mIncrementExpression,
                            mBodyStatement);
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

TEST_F(TestFileSampleRunner, ForStatementRunTest) {
  runFile("tests/samples/test_for_statement.yz", "90");
}

