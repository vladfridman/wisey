//
//  TestNullExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link NullExpression}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/NullExpression.hpp"
#include "wisey/NullType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct NullExpressionTest : public Test {
  
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NullExpression* mNullExpression;
  
  NullExpressionTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mNullExpression(new NullExpression(0)) { }
  
};

TEST_F(NullExpressionTest, generateIRTest) {
  Value* expexted = ConstantExpr::getNullValue(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  ASSERT_EQ(expexted, mNullExpression->generateIR(mContext, PrimitiveTypes::VOID_TYPE));
}

TEST_F(NullExpressionTest, getTypeTest) {
  ASSERT_EQ(mNullExpression->getType(mContext), NullType::NULL_TYPE);
}

TEST_F(NullExpressionTest, isConstantTest) {
  EXPECT_TRUE(mNullExpression->isConstant());
}

TEST_F(NullExpressionTest, isAssignableTest) {
  EXPECT_FALSE(mNullExpression->isAssignable());
}

TEST_F(NullExpressionTest, printToStreamTest) {
  stringstream stringStream;
  mNullExpression->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("null", stringStream.str().c_str());
}

