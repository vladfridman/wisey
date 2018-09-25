//
//  TestBitwiseOrExpression.cpp
//  runtests
//
//  Created by Vladimir Fridman on 8/5/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link BitwiseOrExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "BitwiseOrExpression.hpp"
#include "IRGenerationContext.hpp"
#include "PrimitiveTypes.hpp"

using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Property;
using ::testing::Return;
using ::testing::StrEq;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace wisey;

struct BitwiseOrExpressionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockExpression>* mLeft;
  NiceMock<MockExpression>* mRight;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  BitwiseOrExpression* mBitwiseOrExpression;
  
  BitwiseOrExpressionTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mLeft(new NiceMock<MockExpression>()),
  mRight(new NiceMock<MockExpression>()) {
    TestPrefix::generateIR(mContext);
    
    Value* leftValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
    Value* rightValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
    ON_CALL(*mLeft, generateIR(_, _)).WillByDefault(Return(leftValue));
    ON_CALL(*mLeft, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
    ON_CALL(*mRight, generateIR(_, _)).WillByDefault(Return(rightValue));
    ON_CALL(*mRight, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    BasicBlock* declareBlock = BasicBlock::Create(mLLVMContext, "declare", function);
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setDeclarationsBlock(declareBlock);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    mBitwiseOrExpression = new BitwiseOrExpression(mLeft, mRight, 11);
  }
  
  ~BitwiseOrExpressionTest() {
    delete mStringStream;
    delete mBitwiseOrExpression;
  }
  
  static void printLeftExpression(IRGenerationContext& context, iostream& stream) {
    stream << "i";
  }
  
  static void printRightExpression(IRGenerationContext& context, iostream& stream) {
    stream << "j";
  }
};

TEST_F(BitwiseOrExpressionTest, isConstantTest) {
  EXPECT_FALSE(mBitwiseOrExpression->isConstant());
}

TEST_F(BitwiseOrExpressionTest, isAssignableTest) {
  EXPECT_FALSE(mBitwiseOrExpression->isAssignable());
}

TEST_F(BitwiseOrExpressionTest, generateIRTest) {
  mBitwiseOrExpression->generateIR(mContext, PrimitiveTypes::VOID);
  
  ASSERT_EQ(1ul, mBasicBlock->size());
  Instruction &instruction = mBasicBlock->front();
  *mStringStream << instruction;
  ASSERT_STREQ("  %0 = or i32 3, 5", mStringStream->str().c_str());
}

TEST_F(BitwiseOrExpressionTest, printToStreamTest) {
  stringstream stringStream;
  ON_CALL(*mLeft, printToStream(_, _)).WillByDefault(Invoke(printLeftExpression));
  ON_CALL(*mRight, printToStream(_, _)).WillByDefault(Invoke(printRightExpression));
  mBitwiseOrExpression->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("i | j", stringStream.str().c_str());
}

TEST_F(BitwiseOrExpressionTest, leftExpressionNotIntegerTypeDeathTest) {
  Mock::AllowLeak(mLeft);
  Mock::AllowLeak(mRight);
  
  Value* leftValue = ConstantFP::get(Type::getFloatTy(mLLVMContext), 5.5);
  ON_CALL(*mLeft, generateIR(_, _)).WillByDefault(Return(leftValue));
  ON_CALL(*mLeft, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT));
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mBitwiseOrExpression->generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(11): Error: Left expression in | operation must be integer type\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(BitwiseOrExpressionTest, rightExpressionNotIntegerTypeDeathTest) {
  Mock::AllowLeak(mLeft);
  Mock::AllowLeak(mRight);
  
  Value* rightValue = ConstantFP::get(Type::getFloatTy(mLLVMContext), 5.5);
  ON_CALL(*mRight, generateIR(_, _)).WillByDefault(Return(rightValue));
  ON_CALL(*mRight, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT));
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mBitwiseOrExpression->generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(11): Error: Right expression in | operation must be integer type\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(BitwiseOrExpressionTest, getLeftTest) {
  EXPECT_EQ(mLeft, mBitwiseOrExpression->getLeft());
}

TEST_F(BitwiseOrExpressionTest, getRightTest) {
  EXPECT_EQ(mRight, mBitwiseOrExpression->getRight());
}

TEST_F(BitwiseOrExpressionTest, getOperationTest) {
  EXPECT_STREQ("|", mBitwiseOrExpression->getOperation().c_str());
}

TEST_F(TestFileRunner, bitwiseOrTest) {
  runFile("tests/samples/test_bitwise_or.yz", 7);
}
