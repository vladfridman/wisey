//
//  TestIncrementExpression.cpp
//  Wisey
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

#include "TestFileRunner.hpp"
#include "wisey/IncrementExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalPrimitiveVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct IncrementExpressionTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock = BasicBlock::Create(mLLVMContext, "entry");
  string mName = "foo";
  Identifier* mIdentifier;
  LocalPrimitiveVariable* mVariable;
  string mStringBuffer;
  raw_string_ostream* mStringStream;

public:
  
  IncrementExpressionTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mIdentifier(new Identifier(mName, 0)) {
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();

    AllocaInst* alloc = IRWriter::newAllocaInst(mContext, Type::getInt32Ty(mLLVMContext), mName);
    mVariable = new LocalPrimitiveVariable(mName, PrimitiveTypes::INT_TYPE, alloc);
    mContext.getScopes().setVariable(mVariable);
    mStringStream = new raw_string_ostream(mStringBuffer);
  }

  ~IncrementExpressionTest() {
    delete mStringStream;
  }
};

TEST_F(IncrementExpressionTest, getVariableTest) {
  IncrementExpression* expression = IncrementExpression::newIncrementByOne(mIdentifier, 0);
  vector<const IExpression*> arrayIndices;

  EXPECT_EQ(expression->getVariable(mContext, arrayIndices), mVariable);
}

TEST_F(IncrementExpressionTest, incrementByOneExpressionTest) {
  IncrementExpression* expression = IncrementExpression::newIncrementByOne(mIdentifier, 0);
  expression->generateIR(mContext, PrimitiveTypes::VOID_TYPE);
 
  *mStringStream << *mBlock;

  string expected =
  "\nentry:"
  "\n  %foo = alloca i32"
  "\n  %0 = load i32, i32* %foo"
  "\n  %inc = add i32 %0, 1"
  "\n  store i32 %inc, i32* %foo\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(IncrementExpressionTest, decrementByOneExpressionTest) {
  IncrementExpression* expression = IncrementExpression::newDecrementByOne(mIdentifier, 0);
  expression->generateIR(mContext, PrimitiveTypes::VOID_TYPE);
  
  *mStringStream << *mBlock;
  
  string expected =
  "\nentry:"
  "\n  %foo = alloca i32"
  "\n  %0 = load i32, i32* %foo"
  "\n  %dec = add i32 %0, -1"
  "\n  store i32 %dec, i32* %foo\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(IncrementExpressionTest, incorrectIdentifierTypeDeathTest) {
  Identifier* identifier = new Identifier("bar", 0);
  
  IncrementExpression* expression = IncrementExpression::newIncrementByOne(identifier, 0);
  LocalPrimitiveVariable* variable = new LocalPrimitiveVariable("bar",
                                                                PrimitiveTypes::FLOAT_TYPE,
                                                                NULL);
  mContext.getScopes().setVariable(variable);
  string expected = "Error: Expression is of a type that is incompatible with "
    "increment/decrement operation";
  
  EXPECT_EXIT(expression->generateIR(mContext, PrimitiveTypes::VOID_TYPE),
              ::testing::ExitedWithCode(1),
              expected);
}

TEST_F(IncrementExpressionTest, incrementExpressionTypeTest) {
  IncrementExpression* expression = IncrementExpression::newIncrementByOne(mIdentifier, 0);

  EXPECT_EQ(expression->getType(mContext), PrimitiveTypes::INT_TYPE);
}

TEST_F(IncrementExpressionTest, isConstantTest) {
  IncrementExpression* expression = IncrementExpression::newIncrementByOne(mIdentifier, 0);

  EXPECT_FALSE(expression->isConstant());
}

TEST_F(IncrementExpressionTest, printToStreamTest) {
  IncrementExpression* incrementExpression = IncrementExpression::newIncrementByOne(mIdentifier, 0);
  stringstream stringStreamIncrement;
  incrementExpression->printToStream(mContext, stringStreamIncrement);
  EXPECT_STREQ("foo++", stringStreamIncrement.str().c_str());

  IncrementExpression* decrementExpression = IncrementExpression::newDecrementByOne(mIdentifier, 0);
  stringstream stringStreamDecrement;
  decrementExpression->printToStream(mContext, stringStreamDecrement);
  EXPECT_STREQ("foo--", stringStreamDecrement.str().c_str());
}

TEST_F(TestFileRunner, incrementByOneRunTest) {
  runFile("tests/samples/test_increment_by_one.yz", "3");
}

TEST_F(TestFileRunner, decrementByOneRunTest) {
  runFile("tests/samples/test_decrement_by_one.yz", "5");
}

TEST_F(TestFileRunner, incrementByOneWrappedIdentifierRunTest) {
  runFile("tests/samples/test_increment_by_one_wrapped_identifier.yz", "7");
}

TEST_F(TestFileRunner, illigalIncrementRunDeathTest) {
  expectFailCompile("tests/samples/test_illegal_increment.yz",
                    1,
                    "Error: Increment/decrement operation may only be applied to variables");
}
