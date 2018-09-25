//
//  TestDivideByExpression.cpp
//  runtests
//
//  Created by Vladimir Fridman on 8/26/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link DivideByExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "DivideByExpression.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "LocalPrimitiveVariable.hpp"
#include "PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct DivideByExpressionTest : public Test {
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
  
  DivideByExpressionTest() :
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
  
  ~DivideByExpressionTest() {
    delete mStringStream;
    delete mAdjustment;
  }
  
  static void printAdjustment(IRGenerationContext& context, iostream& stream) {
    stream << "3.3";
  }
};

TEST_F(DivideByExpressionTest, floatMultiplyByTest) {
  DivideByExpression* expression = new DivideByExpression(mIdentifier, mAdjustment, 0);
  expression->generateIR(mContext, PrimitiveTypes::VOID);
  
  *mStringStream << *mDeclareBlock;
  *mStringStream << *mEntryBlock;
  
  string expected =
  "\ndeclare:"
  "\n  %foo = alloca float"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = load float, float* %foo"
  "\n  %1 = fdiv float %0, 0x400A666660000000"
  "\n  store float %1, float* %foo\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(DivideByExpressionTest, incorrectIdentifierTypeDeathTest) {
  Identifier* identifier = new Identifier("bar", 0);
  
  DivideByExpression* expression = new DivideByExpression(identifier, mAdjustment, 5);
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

TEST_F(DivideByExpressionTest, expressionTypeTest) {
  DivideByExpression* expression = new DivideByExpression(mIdentifier, mAdjustment, 0);
  
  EXPECT_EQ(expression->getType(mContext), PrimitiveTypes::FLOAT);
}

TEST_F(DivideByExpressionTest, isConstantTest) {
  DivideByExpression* expression = new DivideByExpression(mIdentifier, mAdjustment, 0);
  
  EXPECT_FALSE(expression->isConstant());
}

TEST_F(DivideByExpressionTest, isAssignableTest) {
  DivideByExpression* expression = new DivideByExpression(mIdentifier, mAdjustment, 0);
  
  EXPECT_FALSE(expression->isAssignable());
}

TEST_F(DivideByExpressionTest, printToStreamTest) {
  DivideByExpression* expression = new DivideByExpression(mIdentifier, mAdjustment, 0);
  stringstream stringStreamIncrement;
  expression->printToStream(mContext, stringStreamIncrement);
  EXPECT_STREQ("foo /= 3.3", stringStreamIncrement.str().c_str());
}

TEST_F(DivideByExpressionTest, getLeftTest) {
  DivideByExpression* expression = new DivideByExpression(mIdentifier, mAdjustment, 0);
  EXPECT_EQ(mIdentifier, expression->getLeft());
}

TEST_F(DivideByExpressionTest, getRightTest) {
  DivideByExpression* expression = new DivideByExpression(mIdentifier, mAdjustment, 0);
  EXPECT_EQ(mAdjustment, expression->getRight());
}

TEST_F(DivideByExpressionTest, getOperationTest) {
  DivideByExpression* expression = new DivideByExpression(mIdentifier, mAdjustment, 0);
  EXPECT_STREQ("/=", expression->getOperation().c_str());
}

TEST_F(TestFileRunner, divideByRunTest) {
  runFile("tests/samples/test_divide_by.yz", 3);
}

