//
//  testAdditiveMultiplicativeExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/9/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link AdditiveMultiplicativeExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/AdditiveMultiplicativeExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/PrimitiveTypes.hpp"

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

struct AdditiveMultiplicativeExpressionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockExpression>* mLeftExpression;
  NiceMock<MockExpression>* mRightExpression;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;

  AdditiveMultiplicativeExpressionTest() :
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
  }
  
  ~AdditiveMultiplicativeExpressionTest() {
    delete mStringStream;
  }

  static void printLeftExpression(IRGenerationContext& context, iostream& stream) {
    stream << "i";
  }

  static void printRightExpression(IRGenerationContext& context, iostream& stream) {
    stream << "j";
  }
};

TEST_F(AdditiveMultiplicativeExpressionTest, getLeftTest) {
  AdditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression, 0);
  
  EXPECT_EQ(mLeftExpression, expression.getLeft());
}

TEST_F(AdditiveMultiplicativeExpressionTest, getRightTest) {
  AdditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression, 0);
  
  EXPECT_EQ(mRightExpression, expression.getRight());
}

TEST_F(AdditiveMultiplicativeExpressionTest, isConstantTest) {
  AdditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression, 0);

  EXPECT_FALSE(expression.isConstant());
}

TEST_F(AdditiveMultiplicativeExpressionTest, isAssignableTest) {
  AdditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression, 0);
  
  EXPECT_FALSE(expression.isAssignable());
}

TEST_F(AdditiveMultiplicativeExpressionTest, additionTest) {
  AdditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression, 0);
  expression.generateIR(mContext, PrimitiveTypes::VOID);
  
  ASSERT_EQ(1ul, mBasicBlock->size());
  Instruction &instruction = mBasicBlock->front();
  *mStringStream << instruction;
  ASSERT_STREQ("  %add = add i32 3, 5", mStringStream->str().c_str());
}

TEST_F(AdditiveMultiplicativeExpressionTest, subtractionTest) {
  AdditiveMultiplicativeExpression expression(mLeftExpression, '-', mRightExpression, 0);
  expression.generateIR(mContext, PrimitiveTypes::VOID);
  
  ASSERT_EQ(1ul, mBasicBlock->size());
  Instruction &instruction = mBasicBlock->front();
  *mStringStream << instruction;
  ASSERT_STREQ("  %sub = sub i32 3, 5", mStringStream->str().c_str());
}

TEST_F(AdditiveMultiplicativeExpressionTest, pointerAddTest) {
  const LLVMPointerType* pointerType = LLVMPrimitiveTypes::I8->getPointerType(mContext, 0);
  Value* leftValue = ConstantPointerNull::get(pointerType->getLLVMType(mContext));
  Value* rightValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  ON_CALL(*mLeftExpression, generateIR(_, _)).WillByDefault(Return(leftValue));
  ON_CALL(*mLeftExpression, getType(_)).WillByDefault(Return(pointerType));
  ON_CALL(*mRightExpression, generateIR(_, _)).WillByDefault(Return(rightValue));
  ON_CALL(*mRightExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));

  AdditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression, 0);
  expression.generateIR(mContext, PrimitiveTypes::VOID);
  
  ASSERT_EQ(1ul, mBasicBlock->size());
  Instruction &instruction = mBasicBlock->front();
  *mStringStream << instruction;
  ASSERT_STREQ("  %0 = getelementptr i8, i8* null, i32 3", mStringStream->str().c_str());
}

TEST_F(AdditiveMultiplicativeExpressionTest, pointerSubtractTest) {
  const LLVMPointerType* pointerType = LLVMPrimitiveTypes::I8->getPointerType(mContext, 0);
  Value* leftValue = ConstantPointerNull::get(pointerType->getLLVMType(mContext));
  Value* rightValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  ON_CALL(*mLeftExpression, generateIR(_, _)).WillByDefault(Return(leftValue));
  ON_CALL(*mLeftExpression, getType(_)).WillByDefault(Return(pointerType));
  ON_CALL(*mRightExpression, generateIR(_, _)).WillByDefault(Return(rightValue));
  ON_CALL(*mRightExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
  
  AdditiveMultiplicativeExpression expression(mLeftExpression, '-', mRightExpression, 0);
  expression.generateIR(mContext, PrimitiveTypes::VOID);
  
  *mStringStream << *mBasicBlock;
  ASSERT_STREQ("\nentry:                                            ; No predecessors!"
               "\n  %sub = sub i32 0, 3"
               "\n  %0 = getelementptr i8, i8* null, i32 %sub\n",
               mStringStream->str().c_str());
}

TEST_F(AdditiveMultiplicativeExpressionTest, printToStreamTest) {
  AdditiveMultiplicativeExpression expression(mLeftExpression, '%', mRightExpression, 0);
  
  stringstream stringStream;
  ON_CALL(*mLeftExpression, printToStream(_, _)).WillByDefault(Invoke(printLeftExpression));
  ON_CALL(*mRightExpression, printToStream(_, _)).WillByDefault(Invoke(printRightExpression));
  expression.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("i % j", stringStream.str().c_str());
}

TEST_F(AdditiveMultiplicativeExpressionTest, incompatibleTypesDeathTest) {
  Mock::AllowLeak(mLeftExpression);
  Mock::AllowLeak(mRightExpression);

  Value* rightValue = ConstantFP::get(Type::getFloatTy(mLLVMContext), 5.5);
  ON_CALL(*mRightExpression, generateIR(_, _)).WillByDefault(Return(rightValue));
  ON_CALL(*mRightExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT));

  AdditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression, 11);

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(expression.generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(11): Error: Incompatible types in '+' operation that require an explicit cast\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(AdditiveMultiplicativeExpressionTest, nonPrimitiveTypesDeathTest) {
  Mock::AllowLeak(mLeftExpression);
  Mock::AllowLeak(mRightExpression);
  
  string modelFullName = "systems.vos.wisey.compiler.tests.MShape";
  StructType* structType = StructType::create(mLLVMContext, modelFullName);
  Model* model = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                                 modelFullName,
                                 structType,
                                 mContext.getImportProfile(),
                                 0);

  ON_CALL(*mLeftExpression, getType(_)).WillByDefault(Return(model));
  ON_CALL(*mRightExpression, getType(_)).WillByDefault(Return(model));
  
  AdditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression, 9);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(expression.generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(9): Error: Can not do operation '+' on non-primitive types\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(AdditiveMultiplicativeExpressionTest, voidTypesDeathTest) {
  Mock::AllowLeak(mLeftExpression);
  Mock::AllowLeak(mRightExpression);
  
  ON_CALL(*mLeftExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::VOID));
  ON_CALL(*mRightExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::VOID));
  
  AdditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression, 5);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(expression.generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(5): Error: Can not use expressions of type VOID in a '+' operation\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(AdditiveMultiplicativeExpressionTest, explicitCastNeededOnGenerateIRDeathTest) {
  Mock::AllowLeak(mLeftExpression);
  Mock::AllowLeak(mRightExpression);
  
  ON_CALL(*mLeftExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::LONG));
  ON_CALL(*mRightExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT));
  
  AdditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression, 3);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(expression.generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Incompatible types in '+' operation that require an explicit cast\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(AdditiveMultiplicativeExpressionTest, explicitCastNeededOnGetTypeDeathTest) {
  Mock::AllowLeak(mLeftExpression);
  Mock::AllowLeak(mRightExpression);
  
  ON_CALL(*mLeftExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::LONG));
  ON_CALL(*mRightExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT));
  
  AdditiveMultiplicativeExpression expression(mLeftExpression, '+', mRightExpression, 1);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(expression.getType(mContext));
  EXPECT_STREQ("/tmp/source.yz(1): Error: Incompatible types in '+' operation that require an explicit cast\n",
              buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, additionRunTest) {
  runFile("tests/samples/test_addition.yz", 7);
}

TEST_F(TestFileRunner, subtractionRunTest) {
  runFile("tests/samples/test_subtraction.yz", 14);
}

TEST_F(TestFileRunner, multiplicationRunTest) {
  runFile("tests/samples/test_multiplication.yz", 50);
}

TEST_F(TestFileRunner, divisionRunTest) {
  runFile("tests/samples/test_division.yz", 5);
}

TEST_F(TestFileRunner, moduloRunTest) {
  runFile("tests/samples/test_modulo.yz", 3);
}

TEST_F(TestFileRunner, additionWithCastRunTest) {
  runFile("tests/samples/test_addition_with_cast.yz", 3);
}

TEST_F(TestFileRunner, subtractionOfDoubleRunTest) {
  runFile("tests/samples/test_subtraction_of_doubles.yz", -2);
}
