//
//  TestDereferenceExpression.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/13/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link DereferenceExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "wisey/DereferenceExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct DereferenceExpressionTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  NiceMock<MockExpression>* mExpression;
  
public:
  
  DereferenceExpressionTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mExpression(new NiceMock<MockExpression>()) {
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    Type* int64Type = Type::getInt64Ty(mLLVMContext);
    Value* value = IRWriter::newAllocaInst(mContext, int64Type, "variable");
    ON_CALL(*mExpression, generateIR(_, _)).WillByDefault(Return(value));
    ON_CALL(*mExpression, getType(_)).
    WillByDefault(Return(PrimitiveTypes::LONG_TYPE->getPointerType()));
    ON_CALL(*mExpression, printToStream(_, _)).WillByDefault(Invoke(printExpression));

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~DereferenceExpressionTest() {
    delete mStringStream;
  }
  
  static void printExpression(IRGenerationContext& context, iostream& stream) {
    stream << "expression";
  }
};

TEST_F(DereferenceExpressionTest, isConstantTest) {
  DereferenceExpression expression(mExpression);
  
  EXPECT_FALSE(expression.isConstant());
}

TEST_F(DereferenceExpressionTest, getVariableTest) {
  DereferenceExpression expression(mExpression);
  vector<const IExpression*> arrayIndices;
  EXPECT_EQ(expression.getVariable(mContext, arrayIndices), nullptr);
}

TEST_F(DereferenceExpressionTest, generateIRTest) {
  DereferenceExpression expression(mExpression);
  Value* result = expression.generateIR(mContext, PrimitiveTypes::VOID_TYPE);
  
  *mStringStream << *result;
  EXPECT_STREQ("  %0 = load i64, i64* %variable", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(DereferenceExpressionTest, getTypeTest) {
  DereferenceExpression expression(mExpression);

  EXPECT_EQ(PrimitiveTypes::LONG_TYPE, expression.getType(mContext));
}

TEST_F(DereferenceExpressionTest, printToStreamTest) {
  DereferenceExpression expression(mExpression);
  
  stringstream stringStream;
  expression.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::llvm::dereference(expression)", stringStream.str().c_str());
}
