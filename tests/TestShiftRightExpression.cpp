//
//  TestShiftRightExpression.cpp
//  runtests
//
//  Created by Vladimir Fridman on 5/24/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ShiftRightExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ShiftRightExpression.hpp"

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

struct ShiftRightExpressionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockExpression>* mLeftExpression;
  NiceMock<MockExpression>* mRightExpression;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  ShiftRightExpression* mShiftRightExpression;
  
  ShiftRightExpressionTest() :
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
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    mShiftRightExpression = new ShiftRightExpression(mLeftExpression, mRightExpression, 9);
  }
  
  ~ShiftRightExpressionTest() {
    delete mStringStream;
    delete mShiftRightExpression;
  }
  
  static void printLeftExpression(IRGenerationContext& context, iostream& stream) {
    stream << "i";
  }
  
  static void printRightExpression(IRGenerationContext& context, iostream& stream) {
    stream << "j";
  }
};

TEST_F(ShiftRightExpressionTest, isConstantTest) {
  EXPECT_FALSE(mShiftRightExpression->isConstant());
}

TEST_F(ShiftRightExpressionTest, isAssignableTest) {
  EXPECT_FALSE(mShiftRightExpression->isAssignable());
}

TEST_F(ShiftRightExpressionTest, generateIRTest) {
  mShiftRightExpression->generateIR(mContext, PrimitiveTypes::VOID);
  
  ASSERT_EQ(1ul, mBasicBlock->size());
  Instruction &instruction = mBasicBlock->front();
  *mStringStream << instruction;
  ASSERT_STREQ("  %0 = ashr i32 3, 5", mStringStream->str().c_str());
}

TEST_F(ShiftRightExpressionTest, printToStreamTest) {
  stringstream stringStream;
  ON_CALL(*mLeftExpression, printToStream(_, _)).WillByDefault(Invoke(printLeftExpression));
  ON_CALL(*mRightExpression, printToStream(_, _)).WillByDefault(Invoke(printRightExpression));
  mShiftRightExpression->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("i >> j", stringStream.str().c_str());
}

TEST_F(ShiftRightExpressionTest, leftExpressionNotIntegerTypeDeathTest) {
  Mock::AllowLeak(mLeftExpression);
  Mock::AllowLeak(mRightExpression);
  
  Value* leftValue = ConstantFP::get(Type::getFloatTy(mLLVMContext), 5.5);
  ON_CALL(*mLeftExpression, generateIR(_, _)).WillByDefault(Return(leftValue));
  ON_CALL(*mLeftExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT));
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mShiftRightExpression->generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(9): Error: "
               "Left expression in shift right operation must be integer type\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(ShiftRightExpressionTest, rightExpressionNotIntegerTypeDeathTest) {
  Mock::AllowLeak(mLeftExpression);
  Mock::AllowLeak(mRightExpression);
  
  Value* rightValue = ConstantFP::get(Type::getFloatTy(mLLVMContext), 5.5);
  ON_CALL(*mRightExpression, generateIR(_, _)).WillByDefault(Return(rightValue));
  ON_CALL(*mRightExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT));
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mShiftRightExpression->generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(9): Error: "
               "Right expression in shift right operation must be integer type\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, shiftRightTest) {
  runFile("tests/samples/test_shiftright.yz", 1);
}
