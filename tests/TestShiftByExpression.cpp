//
//  TestShiftByExpression.cpp
//  runtests
//
//  Created by Vladimir Fridman on 8/25/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ShiftByExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalPrimitiveVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ShiftByExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ShiftByExpressionTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mEntryBlock;
  BasicBlock* mDeclareBlock;
  string mName = "foo";
  Identifier* mIdentifier;
  LocalPrimitiveVariable* mVariable;
  NiceMock<MockExpression>* mAdjustment;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  ShiftByExpressionTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mIdentifier(new Identifier(mName, 0)),
  mAdjustment(new NiceMock<MockExpression>()) {
    TestPrefix::generateIR(mContext);
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mDeclareBlock = BasicBlock::Create(mLLVMContext, "declare", function);
    mEntryBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setDeclarationsBlock(mDeclareBlock);
    mContext.setBasicBlock(mEntryBlock);
    mContext.getScopes().pushScope();
    
    AllocaInst* alloc = IRWriter::newAllocaInst(mContext, Type::getInt32Ty(mLLVMContext), mName);
    mVariable = new LocalPrimitiveVariable(mName, PrimitiveTypes::INT, alloc, 0);
    mContext.getScopes().setVariable(mContext, mVariable);
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    ON_CALL(*mAdjustment, printToStream(_, _)).WillByDefault(Invoke(printAdjustment));
    ON_CALL(*mAdjustment, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
    ConstantInt* three = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
    ON_CALL(*mAdjustment, generateIR(_, _)).WillByDefault(Return(three));
  }
  
  ~ShiftByExpressionTest() {
    delete mStringStream;
    delete mAdjustment;
  }
  
  static void printAdjustment(IRGenerationContext& context, iostream& stream) {
    stream << "3";
  }
};

TEST_F(ShiftByExpressionTest, newShiftLeftByTest) {
  ShiftByExpression* expression = ShiftByExpression::newShiftLeftBy(mIdentifier, mAdjustment, 0);
  expression->generateIR(mContext, PrimitiveTypes::VOID);
  
  *mStringStream << *mDeclareBlock;
  *mStringStream << *mEntryBlock;
  
  string expected =
  "\ndeclare:"
  "\n  %foo = alloca i32"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = load i32, i32* %foo"
  "\n  %1 = shl i32 %0, 3"
  "\n  store i32 %1, i32* %foo\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ShiftByExpressionTest, newShiftRightByTest) {
  ShiftByExpression* expression = ShiftByExpression::newShiftRightBy(mIdentifier, mAdjustment, 0);
  expression->generateIR(mContext, PrimitiveTypes::VOID);
  
  *mStringStream << *mDeclareBlock;
  *mStringStream << *mEntryBlock;
  
  string expected =
  "\ndeclare:"
  "\n  %foo = alloca i32"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = load i32, i32* %foo"
  "\n  %1 = ashr i32 %0, 3"
  "\n  store i32 %1, i32* %foo\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ShiftByExpressionTest, incorrectIdentifierTypeDeathTest) {
  Identifier* identifier = new Identifier("bar", 0);
  
  ShiftByExpression* expression = ShiftByExpression::newShiftLeftBy(identifier, mAdjustment, 5);
  LocalPrimitiveVariable* variable = new LocalPrimitiveVariable("bar",
                                                                PrimitiveTypes::FLOAT,
                                                                NULL,
                                                                5);
  mContext.getScopes().setVariable(mContext, variable);
  string expected = "/tmp/source.yz(5): Error: Left expression in <<= operation must be integer type\n";
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(expression->generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ(expected.c_str(), buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(ShiftByExpressionTest, ShiftByExpressionTypeTest) {
  ShiftByExpression* expression = ShiftByExpression::newShiftLeftBy(mIdentifier, mAdjustment, 0);
  
  EXPECT_EQ(expression->getType(mContext), PrimitiveTypes::INT);
}

TEST_F(ShiftByExpressionTest, isConstantTest) {
  ShiftByExpression* expression = ShiftByExpression::newShiftLeftBy(mIdentifier, mAdjustment, 0);
  
  EXPECT_FALSE(expression->isConstant());
}

TEST_F(ShiftByExpressionTest, isAssignableTest) {
  ShiftByExpression* expression = ShiftByExpression::newShiftLeftBy(mIdentifier, mAdjustment, 0);
  
  EXPECT_FALSE(expression->isAssignable());
}

TEST_F(ShiftByExpressionTest, printToStreamTest) {
  ShiftByExpression* incrementExpression = ShiftByExpression::newShiftLeftBy(mIdentifier,
                                                                             mAdjustment,
                                                                             0);
  stringstream stringStreamIncrement;
  incrementExpression->printToStream(mContext, stringStreamIncrement);
  EXPECT_STREQ("foo <<= 3", stringStreamIncrement.str().c_str());
  
  ShiftByExpression* decrementExpression = ShiftByExpression::newShiftRightBy(mIdentifier,
                                                                              mAdjustment,
                                                                              0);
  stringstream stringStreamDecrement;
  decrementExpression->printToStream(mContext, stringStreamDecrement);
  EXPECT_STREQ("foo >>= 3", stringStreamDecrement.str().c_str());
}

TEST_F(ShiftByExpressionTest, getLeftTest) {
  ShiftByExpression* expression = ShiftByExpression::newShiftRightBy(mIdentifier, mAdjustment, 0);
  EXPECT_EQ(mIdentifier, expression->getLeft());
}

TEST_F(ShiftByExpressionTest, getRightTest) {
  ShiftByExpression* expression = ShiftByExpression::newShiftRightBy(mIdentifier, mAdjustment, 0);
  EXPECT_EQ(mAdjustment, expression->getRight());
}

TEST_F(ShiftByExpressionTest, getOperationTest) {
  ShiftByExpression* expression = ShiftByExpression::newShiftRightBy(mIdentifier, mAdjustment, 0);
  EXPECT_STREQ(">>=", expression->getOperation().c_str());
}

TEST_F(TestFileRunner, shiftLeftByRunTest) {
  runFile("tests/samples/test_shift_left_by.yz", 8);
}

TEST_F(TestFileRunner, shiftRightByRunTest) {
  runFile("tests/samples/test_shift_right_by.yz", 2);
}

TEST_F(TestFileRunner, illigalShiftByRunDeathTest) {
  expectFailCompile("tests/samples/test_illegal_shift_by.yz",
                    1,
                    "tests/samples/test_illegal_shift_by.yz\\(14\\): "
                    "Error: <<= operation may only be applied to variables");
}

