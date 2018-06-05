//
//  TestDereferenceExpression.cpp
//  runtests
//
//  Created by Vladimir Fridman on 6/5/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link DereferenceExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Instruction.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/DereferenceExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"

using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace wisey;

struct DereferenceExpressionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockExpression>* mExpression;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  BasicBlock* mBasicBlock;
  Function* mFunction;
  DereferenceExpression* mDereferenceExpression;
  
  DereferenceExpressionTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mExpression(new NiceMock<MockExpression>()) {
    TestPrefix::generateIR(mContext);
    
    const LLVMPointerType* pointerType = LLVMPrimitiveTypes::I64->getPointerType(mContext, 0);
    Value* value = ConstantPointerNull::get(pointerType->getLLVMType(mContext));
    ON_CALL(*mExpression, generateIR(_, _)).WillByDefault(Return(value));
    ON_CALL(*mExpression, getType(_)).WillByDefault(Return(pointerType));
    ON_CALL(*mExpression, printToStream(_, _)).WillByDefault(Invoke(printExpression));
    
    mDereferenceExpression = new DereferenceExpression(mExpression, 3);

    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "main",
                                 mContext.getModule());
    BasicBlock* declareBlock = BasicBlock::Create(mLLVMContext, "declare", mFunction);
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setDeclarationsBlock(declareBlock);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~DereferenceExpressionTest() {
    delete mDereferenceExpression;
    delete mStringStream;
  }
  
  static void printExpression(IRGenerationContext& context, iostream& stream) {
    stream << "expression";
  }
};

TEST_F(DereferenceExpressionTest, generateIRTest) {
  mDereferenceExpression->generateIR(mContext, LLVMPrimitiveTypes::I64);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = load i64, i64* null\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(DereferenceExpressionTest, isConstantTest) {
  EXPECT_FALSE(mDereferenceExpression->isConstant());
}

TEST_F(DereferenceExpressionTest, isAssignableTest) {
  EXPECT_FALSE(mDereferenceExpression->isAssignable());
}

TEST_F(DereferenceExpressionTest, printToStreamTest) {
  stringstream stringStream;
  
  mDereferenceExpression->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::llvm::dereference(expression)", stringStream.str().c_str());
}

TEST_F(DereferenceExpressionTest, incorrectTypeDeathTest) {
  ::Mock::AllowLeak(mExpression);
  
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(LLVMPrimitiveTypes::I64));

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mDereferenceExpression->generateIR(mContext, LLVMPrimitiveTypes::I64));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Argument in the ::llvm::dereference must be of llvm pointer type\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}
