//
//  TestDoubleType.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link DoubleType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "yazyk/DoubleType.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

struct DoubleTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  DoubleType mDoubleType;
  
public:
  
  DoubleTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
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
  
  ~DoubleTypeTest() {
    delete mStringStream;
  }
};

TEST_F(DoubleTypeTest, doubleTypeTest) {
  EXPECT_EQ(mDoubleType.getLLVMType(mLLVMContext), Type::getDoubleTy(mLLVMContext));
  EXPECT_STREQ(mDoubleType.getName().c_str(), "double");
  EXPECT_EQ(mDoubleType.getTypeKind(), PRIMITIVE_TYPE);
}

TEST_F(DoubleTypeTest, canCastLosslessTest) {
  EXPECT_FALSE(mDoubleType.canCastLosslessTo(PrimitiveTypes::VOID_TYPE));
  EXPECT_FALSE(mDoubleType.canCastLosslessTo(PrimitiveTypes::BOOLEAN_TYPE));
  EXPECT_FALSE(mDoubleType.canCastLosslessTo(PrimitiveTypes::CHAR_TYPE));
  EXPECT_FALSE(mDoubleType.canCastLosslessTo(PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mDoubleType.canCastLosslessTo(PrimitiveTypes::LONG_TYPE));
  EXPECT_FALSE(mDoubleType.canCastLosslessTo(PrimitiveTypes::FLOAT_TYPE));
  EXPECT_TRUE(mDoubleType.canCastLosslessTo(PrimitiveTypes::DOUBLE_TYPE));
}

TEST_F(DoubleTypeTest, canCastTest) {
  EXPECT_FALSE(mDoubleType.canCastTo(PrimitiveTypes::VOID_TYPE));
  EXPECT_TRUE(mDoubleType.canCastTo(PrimitiveTypes::BOOLEAN_TYPE));
  EXPECT_TRUE(mDoubleType.canCastTo(PrimitiveTypes::CHAR_TYPE));
  EXPECT_TRUE(mDoubleType.canCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(mDoubleType.canCastTo(PrimitiveTypes::LONG_TYPE));
  EXPECT_TRUE(mDoubleType.canCastTo(PrimitiveTypes::FLOAT_TYPE));
  EXPECT_TRUE(mDoubleType.canCastTo(PrimitiveTypes::DOUBLE_TYPE));
}

TEST_F(DoubleTypeTest, castToTest) {
  Value* result;
  Value* expressionValue = ConstantFP::get(Type::getDoubleTy(mLLVMContext), 2.5);
  
  EXPECT_EXIT(mDoubleType.castTo(mContext, expressionValue, PrimitiveTypes::VOID_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: can not cast from type 'double' to 'void'");
  
  result = mDoubleType.castTo(mContext, expressionValue, PrimitiveTypes::BOOLEAN_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = fptosi double 2.500000e+00 to i1", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mDoubleType.castTo(mContext, expressionValue, PrimitiveTypes::CHAR_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv1 = fptosi double 2.500000e+00 to i16", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mDoubleType.castTo(mContext, expressionValue, PrimitiveTypes::INT_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv2 = fptosi double 2.500000e+00 to i32", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mDoubleType.castTo(mContext, expressionValue, PrimitiveTypes::LONG_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv3 = fptosi double 2.500000e+00 to i64", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mDoubleType.castTo(mContext, expressionValue, PrimitiveTypes::FLOAT_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv4 = fptrunc double 2.500000e+00 to float", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mDoubleType.castTo(mContext, expressionValue, PrimitiveTypes::DOUBLE_TYPE);
  EXPECT_EQ(result, expressionValue);
}
