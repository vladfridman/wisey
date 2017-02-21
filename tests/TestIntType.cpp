//
//  TestIntType.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IntType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "yazyk/IntType.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

struct IntTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  IntType mIntType;
  
public:
  
  IntTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
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
  
  ~IntTypeTest() {
    delete mStringStream;
  }
};

TEST_F(IntTypeTest, IntTypeTest) {
  EXPECT_EQ(mIntType.getLLVMType(mLLVMContext), (llvm::Type*) Type::getInt32Ty(mLLVMContext));
  EXPECT_STREQ(mIntType.getName().c_str(), "int");
  EXPECT_EQ(mIntType.getTypeKind(), PRIMITIVE_TYPE);
}

TEST_F(IntTypeTest, CanCastLosslessTest) {
  EXPECT_FALSE(mIntType.canCastLosslessTo(PrimitiveTypes::VOID_TYPE));
  EXPECT_FALSE(mIntType.canCastLosslessTo(PrimitiveTypes::BOOLEAN_TYPE));
  EXPECT_FALSE(mIntType.canCastLosslessTo(PrimitiveTypes::CHAR_TYPE));
  EXPECT_TRUE(mIntType.canCastLosslessTo(PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(mIntType.canCastLosslessTo(PrimitiveTypes::LONG_TYPE));
  EXPECT_FALSE(mIntType.canCastLosslessTo(PrimitiveTypes::FLOAT_TYPE));
  EXPECT_TRUE(mIntType.canCastLosslessTo(PrimitiveTypes::DOUBLE_TYPE));
}

TEST_F(IntTypeTest, CanCastTest) {
  EXPECT_FALSE(mIntType.canCastTo(PrimitiveTypes::VOID_TYPE));
  EXPECT_TRUE(mIntType.canCastTo(PrimitiveTypes::BOOLEAN_TYPE));
  EXPECT_TRUE(mIntType.canCastTo(PrimitiveTypes::CHAR_TYPE));
  EXPECT_TRUE(mIntType.canCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(mIntType.canCastTo(PrimitiveTypes::LONG_TYPE));
  EXPECT_TRUE(mIntType.canCastTo(PrimitiveTypes::FLOAT_TYPE));
  EXPECT_TRUE(mIntType.canCastTo(PrimitiveTypes::DOUBLE_TYPE));
}

TEST_F(IntTypeTest, CastToTest) {
  Value* result;
  Value* expressionValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
  
  EXPECT_EXIT(mIntType.castTo(mContext, expressionValue, PrimitiveTypes::VOID_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: can not cast from type 'int' to 'void'");
  
  result = mIntType.castTo(mContext, expressionValue, PrimitiveTypes::BOOLEAN_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = trunc i32 5 to i1", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mIntType.castTo(mContext, expressionValue, PrimitiveTypes::CHAR_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv1 = trunc i32 5 to i16", mStringStream->str().c_str());
  mStringBuffer.clear();

  result = mIntType.castTo(mContext, expressionValue, PrimitiveTypes::INT_TYPE);
  EXPECT_EQ(result, expressionValue);
  
  result = mIntType.castTo(mContext, expressionValue, PrimitiveTypes::LONG_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv2 = zext i32 5 to i64", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mIntType.castTo(mContext, expressionValue, PrimitiveTypes::FLOAT_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv3 = sitofp i32 5 to float", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mIntType.castTo(mContext, expressionValue, PrimitiveTypes::DOUBLE_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv4 = sitofp i32 5 to double", mStringStream->str().c_str());
  mStringBuffer.clear();
}
