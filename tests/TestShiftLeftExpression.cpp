//
//  TestShiftLeftExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/24/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ShiftLeftExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "IRGenerationContext.hpp"
#include "PrimitiveTypes.hpp"
#include "ShiftLeftExpression.hpp"

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

struct ShiftLeftExpressionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockExpression>* mLeftExpression;
  NiceMock<MockExpression>* mRightExpression;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  ShiftLeftExpression* mShiftLeftExpression;
  
  ShiftLeftExpressionTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mLeftExpression(new NiceMock<MockExpression>()),
  mRightExpression(new NiceMock<MockExpression>()) {
    TestPrefix::generateIR(mContext);
    
    Value* leftValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
    Value* rightValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
    ON_CALL(*mLeftExpression, generateIR(_, _)).WillByDefault(Return(leftValue));
    ON_CALL(*mLeftExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
    ON_CALL(*mRightExpression, generateIR(_, _)).WillByDefault(Return(rightValue));
    ON_CALL(*mRightExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
    
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
    
    mShiftLeftExpression = new ShiftLeftExpression(mLeftExpression, mRightExpression, 11);
  }
  
  ~ShiftLeftExpressionTest() {
    delete mStringStream;
    delete mShiftLeftExpression;
  }
  
  static void printLeftExpression(IRGenerationContext& context, iostream& stream) {
    stream << "i";
  }
  
  static void printRightExpression(IRGenerationContext& context, iostream& stream) {
    stream << "j";
  }
};

TEST_F(ShiftLeftExpressionTest, isConstantTest) {
  EXPECT_FALSE(mShiftLeftExpression->isConstant());
}

TEST_F(ShiftLeftExpressionTest, isAssignableTest) {
  EXPECT_FALSE(mShiftLeftExpression->isAssignable());
}

TEST_F(ShiftLeftExpressionTest, generateIRTest) {
  mShiftLeftExpression->generateIR(mContext, PrimitiveTypes::VOID);
  
  ASSERT_EQ(1ul, mBasicBlock->size());
  Instruction &instruction = mBasicBlock->front();
  *mStringStream << instruction;
  ASSERT_STREQ("  %0 = shl i32 3, 5", mStringStream->str().c_str());
}

TEST_F(ShiftLeftExpressionTest, printToStreamTest) {
  stringstream stringStream;
  ON_CALL(*mLeftExpression, printToStream(_, _)).WillByDefault(Invoke(printLeftExpression));
  ON_CALL(*mRightExpression, printToStream(_, _)).WillByDefault(Invoke(printRightExpression));
  mShiftLeftExpression->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("i << j", stringStream.str().c_str());
}

TEST_F(ShiftLeftExpressionTest, leftExpressionNotIntegerTypeDeathTest) {
  Mock::AllowLeak(mLeftExpression);
  Mock::AllowLeak(mRightExpression);
  
  Value* leftValue = ConstantFP::get(Type::getFloatTy(mLLVMContext), 5.5);
  ON_CALL(*mLeftExpression, generateIR(_, _)).WillByDefault(Return(leftValue));
  ON_CALL(*mLeftExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT));
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mShiftLeftExpression->generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(11): Error: Left expression in << operation must be integer type\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(ShiftLeftExpressionTest, rightExpressionNotIntegerTypeDeathTest) {
  Mock::AllowLeak(mLeftExpression);
  Mock::AllowLeak(mRightExpression);
  
  Value* rightValue = ConstantFP::get(Type::getFloatTy(mLLVMContext), 5.5);
  ON_CALL(*mRightExpression, generateIR(_, _)).WillByDefault(Return(rightValue));
  ON_CALL(*mRightExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT));
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mShiftLeftExpression->generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(11): Error: Right expression in << operation must be integer type\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(ShiftLeftExpressionTest, getLeftTest) {
  EXPECT_EQ(mLeftExpression, mShiftLeftExpression->getLeft());
}

TEST_F(ShiftLeftExpressionTest, getRightTest) {
  EXPECT_EQ(mRightExpression, mShiftLeftExpression->getRight());
}

TEST_F(ShiftLeftExpressionTest, getOperationTest) {
  EXPECT_STREQ("<<", mShiftLeftExpression->getOperation().c_str());
}

TEST_F(TestFileRunner, shiftLeftTest) {
  runFile("tests/samples/test_shiftleft.yz", 32);
}

TEST_F(TestFileRunner, shiftLeftAutoCastTest) {
  runFile("tests/samples/test_shiftleft_autocast.yz", 32);
}
