//
//  TestReminderByExpression.cpp
//  runtests
//
//  Created by Vladimir Fridman on 9/7/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ReminderByExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/ReminderByExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalPrimitiveVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ReminderByExpressionTest : public Test {
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
  
  ReminderByExpressionTest() :
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
    
    AllocaInst* alloc = IRWriter::newAllocaInst(mContext, Type::getFloatTy(mLLVMContext), mName);
    mVariable = new LocalPrimitiveVariable(mName, PrimitiveTypes::FLOAT, alloc, 0);
    mContext.getScopes().setVariable(mContext, mVariable);
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    ON_CALL(*mAdjustment, printToStream(_, _)).WillByDefault(Invoke(printAdjustment));
    ON_CALL(*mAdjustment, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT));
    llvm::Constant* three = ConstantFP::get(Type::getFloatTy(mLLVMContext), 3.3);
    ON_CALL(*mAdjustment, generateIR(_, _)).WillByDefault(Return(three));
  }
  
  ~ReminderByExpressionTest() {
    delete mStringStream;
    delete mAdjustment;
  }
  
  static void printAdjustment(IRGenerationContext& context, iostream& stream) {
    stream << "3.3";
  }
};

TEST_F(ReminderByExpressionTest, floatMultiplyByTest) {
  ReminderByExpression* expression = new ReminderByExpression(mIdentifier, mAdjustment, 0);
  expression->generateIR(mContext, PrimitiveTypes::VOID);
  
  *mStringStream << *mDeclareBlock;
  *mStringStream << *mEntryBlock;
  
  string expected =
  "\ndeclare:"
  "\n  %foo = alloca float"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = load float, float* %foo"
  "\n  %1 = frem float %0, 0x400A666660000000"
  "\n  store float %1, float* %foo\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ReminderByExpressionTest, incorrectIdentifierTypeDeathTest) {
  Identifier* identifier = new Identifier("bar", 0);
  
  ReminderByExpression* expression = new ReminderByExpression(identifier, mAdjustment, 5);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, Type::getInt8Ty(mLLVMContext), "");
  LocalPrimitiveVariable* variable = new LocalPrimitiveVariable("bar",
                                                                PrimitiveTypes::BYTE,
                                                                alloc,
                                                                5);
  mContext.getScopes().setVariable(mContext, variable);
  string expected = "/tmp/source.yz(5): Error: Incompatible types: need explicit cast from type float to byte\n";
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(expression->generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ(expected.c_str(), buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(ReminderByExpressionTest, expressionTypeTest) {
  ReminderByExpression* expression = new ReminderByExpression(mIdentifier, mAdjustment, 0);
  
  EXPECT_EQ(expression->getType(mContext), PrimitiveTypes::FLOAT);
}

TEST_F(ReminderByExpressionTest, isConstantTest) {
  ReminderByExpression* expression = new ReminderByExpression(mIdentifier, mAdjustment, 0);
  
  EXPECT_FALSE(expression->isConstant());
}

TEST_F(ReminderByExpressionTest, isAssignableTest) {
  ReminderByExpression* expression = new ReminderByExpression(mIdentifier, mAdjustment, 0);
  
  EXPECT_FALSE(expression->isAssignable());
}

TEST_F(ReminderByExpressionTest, printToStreamTest) {
  ReminderByExpression* expression = new ReminderByExpression(mIdentifier, mAdjustment, 0);
  stringstream stringStreamIncrement;
  expression->printToStream(mContext, stringStreamIncrement);
  EXPECT_STREQ("foo %= 3.3", stringStreamIncrement.str().c_str());
}

TEST_F(ReminderByExpressionTest, getLeftTest) {
  ReminderByExpression* expression = new ReminderByExpression(mIdentifier, mAdjustment, 0);
  EXPECT_EQ(mIdentifier, expression->getLeft());
}

TEST_F(ReminderByExpressionTest, getRightTest) {
  ReminderByExpression* expression = new ReminderByExpression(mIdentifier, mAdjustment, 0);
  EXPECT_EQ(mAdjustment, expression->getRight());
}

TEST_F(ReminderByExpressionTest, getOperationTest) {
  ReminderByExpression* expression = new ReminderByExpression(mIdentifier, mAdjustment, 0);
  EXPECT_STREQ("%=", expression->getOperation().c_str());
}

TEST_F(TestFileRunner, reminderByRunTest) {
  runFile("tests/samples/test_reminder_by.yz", 1);
}
