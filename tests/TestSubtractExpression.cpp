//
//  TestSubtractExpression.cpp
//  runtests
//
//  Created by Vladimir Fridman on 8/6/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link SubtractExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "IRGenerationContext.hpp"
#include "LLVMPrimitiveTypes.hpp"
#include "PrimitiveTypes.hpp"
#include "SubtractExpression.hpp"

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

struct SubtractExpressionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockExpression>* mLeft;
  NiceMock<MockExpression>* mRight;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  SubtractExpressionTest() :
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
  }
  
  ~SubtractExpressionTest() {
    delete mStringStream;
  }
  
  static void printLeftExpression(IRGenerationContext& context, iostream& stream) {
    stream << "i";
  }
  
  static void printRightExpression(IRGenerationContext& context, iostream& stream) {
    stream << "j";
  }
};

TEST_F(SubtractExpressionTest, isConstantTest) {
  SubtractExpression expression(mLeft, mRight, 11);
  
  EXPECT_FALSE(expression.isConstant());
}

TEST_F(SubtractExpressionTest, isAssignableTest) {
  SubtractExpression expression(mLeft, mRight, 11);
  
  EXPECT_FALSE(expression.isAssignable());
}

TEST_F(SubtractExpressionTest, generateIRTest) {
  SubtractExpression expression(mLeft, mRight, 11);
  
  expression.generateIR(mContext, PrimitiveTypes::VOID);
  
  ASSERT_EQ(1ul, mBasicBlock->size());
  Instruction &instruction = mBasicBlock->front();
  *mStringStream << instruction;
  ASSERT_STREQ("  %sub = sub i32 3, 5", mStringStream->str().c_str());
}

TEST_F(SubtractExpressionTest, pointerSubtractTest) {
  const LLVMPointerType* pointerType = LLVMPrimitiveTypes::I8->getPointerType(mContext, 0);
  Value* leftValue = ConstantPointerNull::get(pointerType->getLLVMType(mContext));
  Value* rightValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  ON_CALL(*mLeft, generateIR(_, _)).WillByDefault(Return(leftValue));
  ON_CALL(*mLeft, getType(_)).WillByDefault(Return(pointerType));
  ON_CALL(*mRight, generateIR(_, _)).WillByDefault(Return(rightValue));
  ON_CALL(*mRight, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
  
  SubtractExpression expression(mLeft, mRight, 11);
  expression.generateIR(mContext, PrimitiveTypes::VOID);
  
  *mStringStream << *mBasicBlock;
  ASSERT_STREQ("\nentry:                                            ; No predecessors!"
               "\n  %sub = sub i32 0, 3"
               "\n  %0 = getelementptr i8, i8* null, i32 %sub\n",
               mStringStream->str().c_str());
}

TEST_F(SubtractExpressionTest, printToStreamTest) {
  SubtractExpression expression(mLeft, mRight, 11);
  
  stringstream stringStream;
  ON_CALL(*mLeft, printToStream(_, _)).WillByDefault(Invoke(printLeftExpression));
  ON_CALL(*mRight, printToStream(_, _)).WillByDefault(Invoke(printRightExpression));
  expression.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("i - j", stringStream.str().c_str());
}

TEST_F(SubtractExpressionTest, incompatibleTypesDeathTest) {
  Value* rightValue = ConstantFP::get(Type::getFloatTy(mLLVMContext), 5.5);
  ON_CALL(*mRight, generateIR(_, _)).WillByDefault(Return(rightValue));
  ON_CALL(*mRight, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT));
  
  SubtractExpression expression(mLeft, mRight, 11);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(expression.generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(11): Error: Incompatible types in '-' operation that require an explicit cast\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(SubtractExpressionTest, nonPrimitiveTypesDeathTest) {
  Mock::AllowLeak(mLeft);
  Mock::AllowLeak(mRight);
  
  string modelFullName = "systems.vos.wisey.compiler.tests.MShape";
  StructType* structType = StructType::create(mLLVMContext, modelFullName);
  Model* model = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                                 modelFullName,
                                 structType,
                                 mContext.getImportProfile(),
                                 0);
  
  ON_CALL(*mLeft, getType(_)).WillByDefault(Return(model));
  ON_CALL(*mRight, getType(_)).WillByDefault(Return(model));
  
  SubtractExpression expression(mLeft, mRight, 9);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(expression.generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(9): Error: Can not do operation '-' on non-primitive types\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(SubtractExpressionTest, voidTypesDeathTest) {
  Mock::AllowLeak(mLeft);
  Mock::AllowLeak(mRight);
  
  ON_CALL(*mLeft, getType(_)).WillByDefault(Return(PrimitiveTypes::VOID));
  ON_CALL(*mRight, getType(_)).WillByDefault(Return(PrimitiveTypes::VOID));
  
  SubtractExpression expression(mLeft, mRight, 5);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(expression.generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(5): Error: Can not use expressions of type VOID in a '-' operation\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(SubtractExpressionTest, explicitCastNeededOnGenerateIRDeathTest) {
  Mock::AllowLeak(mLeft);
  Mock::AllowLeak(mRight);
  
  ON_CALL(*mLeft, getType(_)).WillByDefault(Return(PrimitiveTypes::LONG));
  ON_CALL(*mRight, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT));
  
  SubtractExpression expression(mLeft, mRight, 3);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(expression.generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Incompatible types in '-' operation that require an explicit cast\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(SubtractExpressionTest, explicitCastNeededOnGetTypeDeathTest) {
  Mock::AllowLeak(mLeft);
  Mock::AllowLeak(mRight);
  
  ON_CALL(*mLeft, getType(_)).WillByDefault(Return(PrimitiveTypes::LONG));
  ON_CALL(*mRight, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT));
  
  SubtractExpression expression(mLeft, mRight, 1);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(expression.getType(mContext));
  EXPECT_STREQ("/tmp/source.yz(1): Error: Incompatible types in '-' operation that require an explicit cast\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(SubtractExpressionTest, getLeftTest) {
  SubtractExpression expression(mLeft, mRight, 11);
  EXPECT_EQ(mLeft, expression.getLeft());
}

TEST_F(SubtractExpressionTest, getRightTest) {
  SubtractExpression expression(mLeft, mRight, 11);
  EXPECT_EQ(mRight, expression.getRight());
}

TEST_F(SubtractExpressionTest, getOperationTest) {
  SubtractExpression expression(mLeft, mRight, 11);
  EXPECT_STREQ("-", expression.getOperation().c_str());
}

TEST_F(TestFileRunner, subtractionRunTest) {
  runFile("tests/samples/test_subtraction.yz", 14);
}

TEST_F(TestFileRunner, subtractionOfDoubleRunTest) {
  runFile("tests/samples/test_subtraction_of_doubles.yz", -2);
}
