//
//  TestMultiplyExpression.cpp
//  runtests
//
//  Created by Vladimir Fridman on 8/5/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link MultiplyExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/MultiplyExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
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

struct MultiplyExpressionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockExpression>* mLeft;
  NiceMock<MockExpression>* mRight;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  MultiplyExpressionTest() :
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
  
  ~MultiplyExpressionTest() {
    delete mStringStream;
  }
  
  static void printLeftExpression(IRGenerationContext& context, iostream& stream) {
    stream << "i";
  }
  
  static void printRightExpression(IRGenerationContext& context, iostream& stream) {
    stream << "j";
  }
};

TEST_F(MultiplyExpressionTest, isConstantTest) {
  MultiplyExpression expression(mLeft, mRight, 11);

  EXPECT_FALSE(expression.isConstant());
}

TEST_F(MultiplyExpressionTest, isAssignableTest) {
  MultiplyExpression expression(mLeft, mRight, 11);

  EXPECT_FALSE(expression.isAssignable());
}

TEST_F(MultiplyExpressionTest, generateIRTest) {
  MultiplyExpression expression(mLeft, mRight, 11);
  
  expression.generateIR(mContext, PrimitiveTypes::VOID);
  
  ASSERT_EQ(1ul, mBasicBlock->size());
  Instruction &instruction = mBasicBlock->front();
  *mStringStream << instruction;
  ASSERT_STREQ("  %mul = mul i32 3, 5", mStringStream->str().c_str());
}

TEST_F(MultiplyExpressionTest, printToStreamTest) {
  MultiplyExpression expression(mLeft, mRight, 11);

  stringstream stringStream;
  ON_CALL(*mLeft, printToStream(_, _)).WillByDefault(Invoke(printLeftExpression));
  ON_CALL(*mRight, printToStream(_, _)).WillByDefault(Invoke(printRightExpression));
  expression.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("i * j", stringStream.str().c_str());
}

TEST_F(MultiplyExpressionTest, incompatibleTypesDeathTest) {
  Value* rightValue = ConstantFP::get(Type::getFloatTy(mLLVMContext), 5.5);
  ON_CALL(*mRight, generateIR(_, _)).WillByDefault(Return(rightValue));
  ON_CALL(*mRight, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT));
  
  MultiplyExpression expression(mLeft, mRight, 11);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(expression.generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(11): Error: Incompatible types in '*' operation that require an explicit cast\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(MultiplyExpressionTest, nonPrimitiveTypesDeathTest) {
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
  
  MultiplyExpression expression(mLeft, mRight, 9);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(expression.generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(9): Error: Can not do operation '*' on non-primitive types\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(MultiplyExpressionTest, voidTypesDeathTest) {
  Mock::AllowLeak(mLeft);
  Mock::AllowLeak(mRight);
  
  ON_CALL(*mLeft, getType(_)).WillByDefault(Return(PrimitiveTypes::VOID));
  ON_CALL(*mRight, getType(_)).WillByDefault(Return(PrimitiveTypes::VOID));
  
  MultiplyExpression expression(mLeft, mRight, 5);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(expression.generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(5): Error: Can not use expressions of type VOID in a '*' operation\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(MultiplyExpressionTest, explicitCastNeededOnGenerateIRDeathTest) {
  Mock::AllowLeak(mLeft);
  Mock::AllowLeak(mRight);
  
  ON_CALL(*mLeft, getType(_)).WillByDefault(Return(PrimitiveTypes::LONG));
  ON_CALL(*mRight, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT));
  
  MultiplyExpression expression(mLeft, mRight, 3);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(expression.generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Incompatible types in '*' operation that require an explicit cast\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(MultiplyExpressionTest, explicitCastNeededOnGetTypeDeathTest) {
  Mock::AllowLeak(mLeft);
  Mock::AllowLeak(mRight);
  
  ON_CALL(*mLeft, getType(_)).WillByDefault(Return(PrimitiveTypes::LONG));
  ON_CALL(*mRight, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT));
  
  MultiplyExpression expression(mLeft, mRight, 1);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(expression.getType(mContext));
  EXPECT_STREQ("/tmp/source.yz(1): Error: Incompatible types in '*' operation that require an explicit cast\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(MultiplyExpressionTest, getLeftTest) {
  MultiplyExpression expression(mLeft, mRight, 11);
  EXPECT_EQ(mLeft, expression.getLeft());
}

TEST_F(MultiplyExpressionTest, getRightTest) {
  MultiplyExpression expression(mLeft, mRight, 11);
  EXPECT_EQ(mRight, expression.getRight());
}

TEST_F(MultiplyExpressionTest, getOperationTest) {
  MultiplyExpression expression(mLeft, mRight, 11);
  EXPECT_STREQ("*", expression.getOperation().c_str());
}

TEST_F(TestFileRunner, multiplicationRunTest) {
  runFile("tests/samples/test_multiplication.yz", 50);
}
