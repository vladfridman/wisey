//
//  TestIncrementExpression.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/16/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IncrementExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/IncrementExpression.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/LocalStackVariable.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

struct IncrementExpressionTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext = mContext.getLLVMContext();
  BasicBlock* mBlock = BasicBlock::Create(mLLVMContext, "entry");
  string mName = "foo";
  Identifier mIdentifier;
  string mStringBuffer;
  raw_string_ostream* mStringStream;

public:
  
  IncrementExpressionTest() : mIdentifier(mName, "bar") {
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();
    AllocaInst* alloc = new AllocaInst(Type::getInt32Ty(mLLVMContext),
                                       mName,
                                       mBlock);
    
    LocalStackVariable* variable = new LocalStackVariable(mName, PrimitiveTypes::INT_TYPE, alloc);
    mContext.getScopes().setVariable(variable);
    mStringStream = new raw_string_ostream(mStringBuffer);
  }

  ~IncrementExpressionTest() {
    delete mBlock;
    delete mStringStream;
  }
};

TEST_F(IncrementExpressionTest, incrementByOneExpressionTest) {
  IncrementExpression* expression = IncrementExpression::newIncrementByOne(mIdentifier);
  expression->generateIR(mContext);
 
  ASSERT_EQ(4ul, mBlock->size());

  BasicBlock::iterator iterator = mBlock->begin();
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %foo = alloca i32");
  mStringBuffer.clear();
  
  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %bar = load i32, i32* %foo");
  mStringBuffer.clear();
  
  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %inc = add i32 %bar, 1");
  mStringBuffer.clear();
  
  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  store i32 %inc, i32* %foo");
  mStringBuffer.clear();
}

TEST_F(IncrementExpressionTest, decrementByOneExpressionTest) {
  IncrementExpression* expression = IncrementExpression::newDecrementByOne(mIdentifier);
  expression->generateIR(mContext);
  
  ASSERT_EQ(4ul, mBlock->size());
  
  BasicBlock::iterator iterator = mBlock->begin();
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %foo = alloca i32");
  mStringBuffer.clear();
  
  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %bar = load i32, i32* %foo");
  mStringBuffer.clear();
  
  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %dec = add i32 %bar, -1");
  mStringBuffer.clear();
  
  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  store i32 %dec, i32* %foo");
  mStringBuffer.clear();
}

TEST_F(IncrementExpressionTest, incorrectIdentifierTypeDeathTest) {
  IncrementExpression* expression = IncrementExpression::newIncrementByOne(mIdentifier);
  LocalStackVariable* variable = new LocalStackVariable(mName, PrimitiveTypes::FLOAT_TYPE, NULL);
  mContext.getScopes().setVariable(variable);
  string expected = "Error: Identifier foo is of a type that is "
    "incopatible with increment/decrement operation";
  
  EXPECT_EXIT(expression->generateIR(mContext),
              ::testing::ExitedWithCode(1),
              expected);
}

TEST_F(IncrementExpressionTest, releaseOwnershipDeathTest) {
  IncrementExpression* expression = IncrementExpression::newIncrementByOne(mIdentifier);
  
  EXPECT_EXIT(expression->releaseOwnership(mContext),
              ::testing::ExitedWithCode(1),
              "Can not release ownership of an increment operation result, "
              "it is not a heap pointer");
}

TEST_F(IncrementExpressionTest, incrementExpressionTypeTest) {
  IncrementExpression* expression = IncrementExpression::newIncrementByOne(mIdentifier);

  EXPECT_EQ(expression->getType(mContext), PrimitiveTypes::INT_TYPE);
}

TEST_F(TestFileSampleRunner, incrementByOneRunTest) {
  runFile("tests/samples/test_increment_by_one.yz", "3");
}

TEST_F(TestFileSampleRunner, decrementByOneRunTest) {
  runFile("tests/samples/test_decrement_by_one.yz", "5");
}

TEST_F(TestFileSampleRunner, incrementByOneWrappedIdentifierRunTest) {
  runFile("tests/samples/test_increment_by_one_wrapped_identifier.yz", "7");
}

