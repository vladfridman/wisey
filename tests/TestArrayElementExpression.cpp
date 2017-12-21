//
//  TestArrayElementExpression.cpp
//  runtests
//
//  Created by Vladimir Fridman on 12/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ArrayElementExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "MockVariable.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/ArrayElementExpression.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/PrimitiveTypes.hpp"

using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace wisey;

struct ArrayElementExpressionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockExpression>* mArrayExpression;
  NiceMock<MockExpression>* mArrayIndexExpression;
  NiceMock<MockVariable>* mArrayVariable;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Function* mFunction;
  BasicBlock* mBasicBlock;
  wisey::ArrayType* mArrayType;
  ArrayElementExpression* mArrayElementExpression;
  
  ArrayElementExpressionTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mArrayExpression(new NiceMock<MockExpression>()),
  mArrayIndexExpression(new NiceMock<MockExpression>()),
  mArrayVariable(new NiceMock<MockVariable>()) {
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "main",
                                 mContext.getModule());
    
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    mArrayType = new wisey::ArrayType(PrimitiveTypes::INT_TYPE, 5);
    Value* alloca = IRWriter::newAllocaInst(mContext, mArrayType->getLLVMType(mContext), "");
    ON_CALL(*mArrayExpression, generateIR(_, _)).WillByDefault(Return(alloca));
    ON_CALL(*mArrayExpression, getType(_)).WillByDefault(Return(mArrayType));
    ON_CALL(*mArrayExpression, getVariable(_, _)).WillByDefault(Return(mArrayVariable));
    ON_CALL(*mArrayVariable, getType()).WillByDefault(Return(mArrayType));
    ON_CALL(*mArrayVariable, generateIdentifierIR(_)).WillByDefault(Return(alloca));
    ConstantInt* three = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
    ON_CALL(*mArrayIndexExpression, generateIR(_, _)).WillByDefault(Return(three));
    ON_CALL(*mArrayIndexExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    
    mArrayElementExpression = new ArrayElementExpression(mArrayExpression, mArrayIndexExpression);
  }
  
  ~ArrayElementExpressionTest() {
    delete mStringStream;
    delete mArrayExpression;
    delete mArrayIndexExpression;
    delete mArrayVariable;
  }
  
  static void printArrayExpression(IRGenerationContext& context, iostream& stream) {
    stream << "array";
  }
  
  static void printArrayIndex(IRGenerationContext& context, iostream& stream) {
    stream << "index";
  }
};

TEST_F(ArrayElementExpressionTest, generateIRTest) {
  mArrayElementExpression->generateIR(mContext, IR_GENERATION_NORMAL);
  
  *mStringStream << *mBasicBlock;
  
  EXPECT_STREQ("\nentry:"
               "\n  %0 = alloca [5 x i32]"
               "\n  %1 = getelementptr [5 x i32], [5 x i32]* %0, i32 0, i32 3"
               "\n  %2 = load i32, i32* %1\n",
               mStringStream->str().c_str());
}

TEST_F(ArrayElementExpressionTest, isConstantTest) {
  EXPECT_FALSE(mArrayElementExpression->isConstant());
}

TEST_F(ArrayElementExpressionTest, getTypeTest) {
  EXPECT_EQ(mArrayType->getBaseType(), mArrayElementExpression->getType(mContext));
}

TEST_F(ArrayElementExpressionTest, printToStreamTest) {
  stringstream stringStream;
  ON_CALL(*mArrayExpression, printToStream(_, _)).WillByDefault(Invoke(printArrayExpression));
  ON_CALL(*mArrayIndexExpression, printToStream(_, _)).WillByDefault(Invoke(printArrayIndex));
  mArrayElementExpression->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("array[index]", stringStream.str().c_str());
}

TEST_F(ArrayElementExpressionTest, generateIRForNonArrayTypeDeathTest) {
  Mock::AllowLeak(mArrayExpression);
  Mock::AllowLeak(mArrayIndexExpression);
  Mock::AllowLeak(mArrayVariable);
  ON_CALL(*mArrayVariable, getType()).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  
  EXPECT_EXIT(mArrayElementExpression->generateIR(mContext, IR_GENERATION_NORMAL),
              ::testing::ExitedWithCode(1),
              "Error: Expecting array type expression before \\[\\] but expression type is int");
}

TEST_F(ArrayElementExpressionTest, generateIRForNonIntTypeIndexDeathTest) {
  Mock::AllowLeak(mArrayExpression);
  Mock::AllowLeak(mArrayIndexExpression);
  Mock::AllowLeak(mArrayVariable);
  ON_CALL(*mArrayIndexExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));
  
  EXPECT_EXIT(mArrayElementExpression->generateIR(mContext, IR_GENERATION_NORMAL),
              ::testing::ExitedWithCode(1),
              "Error: Array index should be integer type, but it is float");
}

TEST_F(ArrayElementExpressionTest, getTypeDeathTest) {
  Mock::AllowLeak(mArrayExpression);
  Mock::AllowLeak(mArrayIndexExpression);
  Mock::AllowLeak(mArrayVariable);
  ON_CALL(*mArrayExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  
  EXPECT_EXIT(mArrayElementExpression->getType(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Expecting array type expression before \\[\\] but expression type is int");
}

TEST_F(ArrayElementExpressionTest, generateAssignmentIRDeathTest) {
  Mock::AllowLeak(mArrayExpression);
  Mock::AllowLeak(mArrayIndexExpression);
  Mock::AllowLeak(mArrayVariable);

  wisey::ArrayType* arrayType = new wisey::ArrayType(PrimitiveTypes::INT_TYPE, 5);
  mArrayType = new wisey::ArrayType(arrayType, 5);
  Value* alloca = IRWriter::newAllocaInst(mContext, mArrayType->getLLVMType(mContext), "");
  ON_CALL(*mArrayExpression, generateIR(_, _)).WillByDefault(Return(alloca));
  ON_CALL(*mArrayExpression, getType(_)).WillByDefault(Return(mArrayType));
  ON_CALL(*mArrayExpression, getVariable(_, _)).WillByDefault(Return(mArrayVariable));
  ON_CALL(*mArrayVariable, getType()).WillByDefault(Return(mArrayType));
  ON_CALL(*mArrayVariable, generateIdentifierIR(_)).WillByDefault(Return(alloca));
  ConstantInt* three = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  ON_CALL(*mArrayIndexExpression, generateIR(_, _)).WillByDefault(Return(three));
  ON_CALL(*mArrayIndexExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  
  mArrayElementExpression = new ArrayElementExpression(mArrayExpression, mArrayIndexExpression);

  EXPECT_EXIT(mArrayElementExpression->generateIR(mContext, IR_GENERATION_NORMAL),
              ::testing::ExitedWithCode(1),
              "Error: Expression does not reference an array element");
}

TEST_F(ArrayElementExpressionTest, generateIRDeathTest) {
  Mock::AllowLeak(mArrayExpression);
  Mock::AllowLeak(mArrayIndexExpression);
  Mock::AllowLeak(mArrayVariable);
  vector<const IExpression*> arrayIndices;
  
  EXPECT_EXIT(ArrayElementExpression::generateElementIR(mContext, mArrayType, NULL, arrayIndices),
              ::testing::ExitedWithCode(1),
              "Error: Expression does not reference an array element");
}

