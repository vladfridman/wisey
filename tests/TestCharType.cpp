//
//  TestCharType.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link CharType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "yazyk/CharType.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

struct CharTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  CharType mCharType;
  
public:
  
  CharTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
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
  
  ~CharTypeTest() {
    delete mStringStream;
  }
};

TEST_F(CharTypeTest, CharTypeTest) {
  EXPECT_EQ(mCharType.getLLVMType(mLLVMContext), (llvm::Type*) Type::getInt16Ty(mLLVMContext));
  EXPECT_STREQ(mCharType.getName().c_str(), "char");
  EXPECT_EQ(mCharType.getTypeKind(), PRIMITIVE_TYPE);
}

TEST_F(CharTypeTest, CanCastLosslessTest) {
  EXPECT_FALSE(mCharType.canCastLosslessTo(PrimitiveTypes::VOID_TYPE));
  EXPECT_FALSE(mCharType.canCastLosslessTo(PrimitiveTypes::BOOLEAN_TYPE));
  EXPECT_TRUE(mCharType.canCastLosslessTo(PrimitiveTypes::CHAR_TYPE));
  EXPECT_TRUE(mCharType.canCastLosslessTo(PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(mCharType.canCastLosslessTo(PrimitiveTypes::LONG_TYPE));
  EXPECT_TRUE(mCharType.canCastLosslessTo(PrimitiveTypes::FLOAT_TYPE));
  EXPECT_TRUE(mCharType.canCastLosslessTo(PrimitiveTypes::DOUBLE_TYPE));
}

TEST_F(CharTypeTest, CanCastTest) {
  EXPECT_FALSE(mCharType.canCastTo(PrimitiveTypes::VOID_TYPE));
  EXPECT_TRUE(mCharType.canCastTo(PrimitiveTypes::BOOLEAN_TYPE));
  EXPECT_TRUE(mCharType.canCastTo(PrimitiveTypes::CHAR_TYPE));
  EXPECT_TRUE(mCharType.canCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(mCharType.canCastTo(PrimitiveTypes::LONG_TYPE));
  EXPECT_TRUE(mCharType.canCastTo(PrimitiveTypes::FLOAT_TYPE));
  EXPECT_TRUE(mCharType.canCastTo(PrimitiveTypes::DOUBLE_TYPE));
}

TEST_F(CharTypeTest, CastToTest) {
  Value* result;
  Value* expressionValue = ConstantInt::get(Type::getInt16Ty(mLLVMContext), 'a');
  
  EXPECT_EXIT(mCharType.castTo(mContext, expressionValue, PrimitiveTypes::VOID_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: can not cast from type 'char' to 'void'");
  
  result = mCharType.castTo(mContext, expressionValue, PrimitiveTypes::BOOLEAN_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = trunc i16 97 to i1", mStringStream->str().c_str());
  mStringBuffer.clear();

  result = mCharType.castTo(mContext, expressionValue, PrimitiveTypes::CHAR_TYPE);
  EXPECT_EQ(result, expressionValue);
  
  result = mCharType.castTo(mContext, expressionValue, PrimitiveTypes::INT_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv1 = zext i16 97 to i32", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mCharType.castTo(mContext, expressionValue, PrimitiveTypes::LONG_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv2 = zext i16 97 to i64", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mCharType.castTo(mContext, expressionValue, PrimitiveTypes::FLOAT_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv3 = sitofp i16 97 to float", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mCharType.castTo(mContext, expressionValue, PrimitiveTypes::DOUBLE_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv4 = sitofp i16 97 to double", mStringStream->str().c_str());
  mStringBuffer.clear();
}
