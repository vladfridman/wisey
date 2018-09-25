//
//  testRelationalExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/11/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link RelationalExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "IRGenerationContext.hpp"
#include "PrimitiveTypes.hpp"
#include "RelationalExpression.hpp"

using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace wisey;

struct RelationalExpressionTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockExpression>* mLeftExpression;
  NiceMock<MockExpression>* mRightExpression;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  Model* mModel;
  Node* mNode;
  raw_string_ostream* mStringStream;
  
  RelationalExpressionTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mLeftExpression(new NiceMock<MockExpression>()),
  mRightExpression(new NiceMock<MockExpression>()) {
    TestPrefix::generateIR(mContext);
    
    LLVMContext& llvmContext = mContext.getLLVMContext();
    Value* leftValue = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 3);
    Value* rightValue = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 5);
    ON_CALL(*mLeftExpression, generateIR(_, _)).WillByDefault(Return(leftValue));
    ON_CALL(*mLeftExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
    ON_CALL(*mRightExpression, generateIR(_, _)).WillByDefault(Return(rightValue));
    ON_CALL(*mRightExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
    mBasicBlock = BasicBlock::Create(mContext.getLLVMContext(), "test");
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();

    string modelFullName = "systems.vos.wisey.compiler.tests.MSquare";
    StructType* modelStructType = StructType::create(llvmContext, modelFullName);
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                             modelFullName,
                             modelStructType,
                             mContext.getImportProfile(),
                             0);

    string nodeFullName = "systems.vos.wisey.compiler.tests.NElement";
    StructType* nodeStructType = StructType::create(llvmContext, nodeFullName);
    mNode = Node::newNode(AccessLevel::PUBLIC_ACCESS,
                          nodeFullName,
                          nodeStructType,
                          mContext.getImportProfile(),
                          0);

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~RelationalExpressionTest() {
    delete mBasicBlock;
    delete mStringStream;
  }
  
  static void printLeftExpression(IRGenerationContext& context, iostream& stream) {
    stream << "left";
  }
  
  static void printRightExpression(IRGenerationContext& context, iostream& stream) {
    stream << "right";
  }
};

TEST_F(RelationalExpressionTest, intLessThanTest) {
  RelationalExpression expression(mLeftExpression, RELATIONAL_OPERATION_LT, mRightExpression, 0);
  expression.generateIR(mContext, PrimitiveTypes::VOID);
  
  ASSERT_EQ(1ul, mBasicBlock->size());
  EXPECT_EQ(expression.getType(mContext), PrimitiveTypes::BOOLEAN);
  Instruction &instruction = mBasicBlock->front();
  *mStringStream << instruction;
  ASSERT_STREQ(mStringStream->str().c_str(), "  %cmp = icmp slt i32 3, 5");
}

TEST_F(RelationalExpressionTest, intGreaterThanOrEqualTest) {
  RelationalExpression expression(mLeftExpression, RELATIONAL_OPERATION_GE, mRightExpression, 0);
  expression.generateIR(mContext, PrimitiveTypes::VOID);
  
  ASSERT_EQ(1ul, mBasicBlock->size());
  EXPECT_EQ(expression.getType(mContext), PrimitiveTypes::BOOLEAN);
  Instruction &instruction = mBasicBlock->front();
  *mStringStream << instruction;
  ASSERT_STREQ(mStringStream->str().c_str(), "  %cmp = icmp sge i32 3, 5");
}

TEST_F(RelationalExpressionTest, compareIntsWithCastTest) {
  Value* leftValue = ConstantInt::get(Type::getInt64Ty(mContext.getLLVMContext()), 3);
  ON_CALL(*mLeftExpression, generateIR(_, _)).WillByDefault(Return(leftValue));
  ON_CALL(*mLeftExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::LONG));

  RelationalExpression expression(mLeftExpression, RELATIONAL_OPERATION_GE, mRightExpression, 0);
  expression.generateIR(mContext, PrimitiveTypes::VOID);
  
  ASSERT_EQ(2ul, mBasicBlock->size());
  EXPECT_EQ(expression.getType(mContext), PrimitiveTypes::BOOLEAN);
  *mStringStream << mBasicBlock->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  %conv = sext i32 5 to i64");
  mStringBuffer.clear();
  *mStringStream << mBasicBlock->back();
  EXPECT_STREQ(mStringStream->str().c_str(), "  %cmp = icmp sge i64 3, %conv");
}


TEST_F(RelationalExpressionTest, floatLessThanTest) {
  Value* leftValue = ConstantFP::get(Type::getFloatTy(mContext.getLLVMContext()), 3.2);
  Value* rightValue = ConstantFP::get(Type::getFloatTy(mContext.getLLVMContext()), 5.6);
  ON_CALL(*mLeftExpression, generateIR(_, _)).WillByDefault(Return(leftValue));
  ON_CALL(*mLeftExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT));
  ON_CALL(*mRightExpression, generateIR(_, _)).WillByDefault(Return(rightValue));
  ON_CALL(*mRightExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT));

  RelationalExpression expression(mLeftExpression, RELATIONAL_OPERATION_LT, mRightExpression, 0);
  expression.generateIR(mContext, PrimitiveTypes::VOID);
  
  ASSERT_EQ(1ul, mBasicBlock->size());
  EXPECT_EQ(expression.getType(mContext), PrimitiveTypes::BOOLEAN);
  Instruction &instruction = mBasicBlock->front();
  *mStringStream << instruction;
  ASSERT_STREQ(mStringStream->str().c_str(),
               "  %cmp = fcmp olt float 0x40099999A0000000, 0x4016666660000000");
}

TEST_F(RelationalExpressionTest, isConstantTest) {
  RelationalExpression expression(mLeftExpression, RELATIONAL_OPERATION_LE, mRightExpression, 0);
  
  EXPECT_FALSE(expression.isConstant());
}

TEST_F(RelationalExpressionTest, isAssignableTest) {
  RelationalExpression expression(mLeftExpression, RELATIONAL_OPERATION_LE, mRightExpression, 0);
  
  EXPECT_FALSE(expression.isAssignable());
}

TEST_F(RelationalExpressionTest, printToStreamTest) {
  RelationalExpression expression(mLeftExpression, RELATIONAL_OPERATION_LE, mRightExpression, 0);

  stringstream stringStream;
  ON_CALL(*mLeftExpression, printToStream(_, _)).WillByDefault(Invoke(printLeftExpression));
  ON_CALL(*mRightExpression, printToStream(_, _)).WillByDefault(Invoke(printRightExpression));
  expression.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("left <= right", stringStream.str().c_str());
}

TEST_F(RelationalExpressionTest, objectAndNonObjectCompareDeathTest) {
  Mock::AllowLeak(mLeftExpression);
  Mock::AllowLeak(mRightExpression);

  ON_CALL(*mRightExpression, getType(_)).WillByDefault(Return(mModel));
  RelationalExpression expression(mLeftExpression, RELATIONAL_OPERATION_LT, mRightExpression, 3);

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(expression.generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Can not compare objects to primitive types\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(RelationalExpressionTest, incompatableObjectsCompareDeathTest) {
  Mock::AllowLeak(mLeftExpression);
  Mock::AllowLeak(mRightExpression);
  
  Value* modelNull = ConstantPointerNull::get(mModel->getLLVMType(mContext));
  ON_CALL(*mLeftExpression, getType(_)).WillByDefault(Return(mModel));
  ON_CALL(*mLeftExpression, generateIR(_, _)).WillByDefault(Return(modelNull));
  
  Value* nodeNull = ConstantPointerNull::get(mNode->getLLVMType(mContext));
  ON_CALL(*mRightExpression, getType(_)).WillByDefault(Return(mNode));
  ON_CALL(*mRightExpression, generateIR(_, _)).WillByDefault(Return(nodeNull));
  
  RelationalExpression expression(mLeftExpression, RELATIONAL_OPERATION_EQ, mRightExpression, 3);
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(expression.generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Can not compare types systems.vos.wisey.compiler.tests.MSquare and systems.vos.wisey.compiler.tests.NElement\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(RelationalExpressionTest, compareObjectsWithNonEqualityTypePredicateDeathTest) {
  Mock::AllowLeak(mLeftExpression);
  Mock::AllowLeak(mRightExpression);
  
  ON_CALL(*mLeftExpression, getType(_)).WillByDefault(Return(mModel));
  ON_CALL(*mRightExpression, getType(_)).WillByDefault(Return(mModel));
  RelationalExpression expression(mLeftExpression, RELATIONAL_OPERATION_GT, mRightExpression, 5);
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(expression.generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(5): Error: Objects can only be used to compare for equality\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(RelationalExpressionTest, incompatablePrimitiveTypesDeathTest) {
  Mock::AllowLeak(mLeftExpression);
  Mock::AllowLeak(mRightExpression);
  
  ON_CALL(*mLeftExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT));
  RelationalExpression expression(mLeftExpression, RELATIONAL_OPERATION_EQ, mRightExpression, 7);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(expression.generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(7): Error: Can not compare types float and int\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(RelationalExpressionTest, getLeftTest) {
  RelationalExpression expression(mLeftExpression, RELATIONAL_OPERATION_EQ, mRightExpression, 7);
  EXPECT_EQ(mLeftExpression, expression.getLeft());
}

TEST_F(RelationalExpressionTest, getRightTest) {
  RelationalExpression expression(mLeftExpression, RELATIONAL_OPERATION_EQ, mRightExpression, 7);
  EXPECT_EQ(mRightExpression, expression.getRight());
}

TEST_F(RelationalExpressionTest, getOperationTest) {
  RelationalExpression expression(mLeftExpression, RELATIONAL_OPERATION_EQ, mRightExpression, 7);
  EXPECT_STREQ("==", expression.getOperation().c_str());
}

TEST_F(TestFileRunner, lessThanRunTest) {
  runFile("tests/samples/test_less_than.yz", 1);
}

TEST_F(TestFileRunner, greaterThanOrEqualRunTest) {
  runFile("tests/samples/test_greater_than_or_equal.yz", 1);
}

TEST_F(TestFileRunner, equalRunTest) {
  runFile("tests/samples/test_equal.yz", 0);
}

TEST_F(TestFileRunner, notEqualRunTest) {
  runFile("tests/samples/test_not_equal.yz", 1);
}

TEST_F(TestFileRunner, compareModelToNullRunTest) {
  runFile("tests/samples/test_compare_model_to_null.yz", 0);
}

TEST_F(TestFileRunner, compareModelToItselfRunTest) {
  runFile("tests/samples/test_compare_model_to_itself.yz", 1);
}
