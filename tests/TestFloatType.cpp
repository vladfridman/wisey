//
//  TestFloatType.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link FloatType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "wisey/FloatType.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

struct FloatTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  FloatType mFloatType;
  
public:
  
  FloatTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~FloatTypeTest() {
    delete mStringStream;
  }
};

TEST_F(FloatTypeTest, floatTypeTest) {
  EXPECT_EQ(mFloatType.getLLVMType(mLLVMContext), Type::getFloatTy(mLLVMContext));
  EXPECT_STREQ(mFloatType.getName().c_str(), "float");
  EXPECT_EQ(mFloatType.getTypeKind(), PRIMITIVE_TYPE);
}

TEST_F(FloatTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mFloatType.canAutoCastTo(PrimitiveTypes::VOID_TYPE));
  EXPECT_FALSE(mFloatType.canAutoCastTo(PrimitiveTypes::BOOLEAN_TYPE));
  EXPECT_FALSE(mFloatType.canAutoCastTo(PrimitiveTypes::CHAR_TYPE));
  EXPECT_FALSE(mFloatType.canAutoCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mFloatType.canAutoCastTo(PrimitiveTypes::LONG_TYPE));
  EXPECT_TRUE(mFloatType.canAutoCastTo(PrimitiveTypes::FLOAT_TYPE));
  EXPECT_TRUE(mFloatType.canAutoCastTo(PrimitiveTypes::DOUBLE_TYPE));
}

TEST_F(FloatTypeTest, canCastTest) {
  EXPECT_FALSE(mFloatType.canCastTo(PrimitiveTypes::VOID_TYPE));
  EXPECT_TRUE(mFloatType.canCastTo(PrimitiveTypes::BOOLEAN_TYPE));
  EXPECT_TRUE(mFloatType.canCastTo(PrimitiveTypes::CHAR_TYPE));
  EXPECT_TRUE(mFloatType.canCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(mFloatType.canCastTo(PrimitiveTypes::LONG_TYPE));
  EXPECT_TRUE(mFloatType.canCastTo(PrimitiveTypes::FLOAT_TYPE));
  EXPECT_TRUE(mFloatType.canCastTo(PrimitiveTypes::DOUBLE_TYPE));
}

TEST_F(FloatTypeTest, castToTest) {
  Value* result;
  Value* expressionValue = ConstantFP::get(Type::getFloatTy(mLLVMContext), 2.5);
  
  EXPECT_EXIT(mFloatType.castTo(mContext, expressionValue, PrimitiveTypes::VOID_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: can not cast from type 'float' to 'void'");
  
  result = mFloatType.castTo(mContext, expressionValue, PrimitiveTypes::BOOLEAN_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = fptosi float 2.500000e+00 to i1", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mFloatType.castTo(mContext, expressionValue, PrimitiveTypes::CHAR_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv1 = fptosi float 2.500000e+00 to i16", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mFloatType.castTo(mContext, expressionValue, PrimitiveTypes::INT_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv2 = fptosi float 2.500000e+00 to i32", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mFloatType.castTo(mContext, expressionValue, PrimitiveTypes::LONG_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv3 = fptosi float 2.500000e+00 to i64", mStringStream->str().c_str());
  mStringBuffer.clear();

  result = mFloatType.castTo(mContext, expressionValue, PrimitiveTypes::FLOAT_TYPE);
  EXPECT_EQ(result, expressionValue);
  
  result = mFloatType.castTo(mContext, expressionValue, PrimitiveTypes::DOUBLE_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv4 = fpext float 2.500000e+00 to double", mStringStream->str().c_str());
  mStringBuffer.clear();
}
