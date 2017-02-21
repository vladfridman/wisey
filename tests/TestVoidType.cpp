//
//  TestVoidType.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link VoidType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "yazyk/PrimitiveTypes.hpp"
#include "yazyk/VoidType.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

struct VoidTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  VoidType mVoidType;
  
public:
  
  VoidTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
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
  
  ~VoidTypeTest() {
    delete mStringStream;
  }
};

TEST_F(VoidTypeTest, VoidTypeTest) {
  EXPECT_EQ(mVoidType.getLLVMType(mLLVMContext), Type::getVoidTy(mLLVMContext));
  EXPECT_STREQ(mVoidType.getName().c_str(), "void");
  EXPECT_EQ(mVoidType.getTypeKind(), PRIMITIVE_TYPE);
}

TEST_F(VoidTypeTest, CanCastLosslessTest) {
  EXPECT_TRUE(mVoidType.canCastLosslessTo(PrimitiveTypes::VOID_TYPE));
  EXPECT_FALSE(mVoidType.canCastLosslessTo(PrimitiveTypes::BOOLEAN_TYPE));
  EXPECT_FALSE(mVoidType.canCastLosslessTo(PrimitiveTypes::CHAR_TYPE));
  EXPECT_FALSE(mVoidType.canCastLosslessTo(PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mVoidType.canCastLosslessTo(PrimitiveTypes::LONG_TYPE));
  EXPECT_FALSE(mVoidType.canCastLosslessTo(PrimitiveTypes::FLOAT_TYPE));
  EXPECT_FALSE(mVoidType.canCastLosslessTo(PrimitiveTypes::DOUBLE_TYPE));
}

TEST_F(VoidTypeTest, CanCastTest) {
  EXPECT_TRUE(mVoidType.canCastTo(PrimitiveTypes::VOID_TYPE));
  EXPECT_FALSE(mVoidType.canCastTo(PrimitiveTypes::BOOLEAN_TYPE));
  EXPECT_FALSE(mVoidType.canCastTo(PrimitiveTypes::CHAR_TYPE));
  EXPECT_FALSE(mVoidType.canCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mVoidType.canCastTo(PrimitiveTypes::LONG_TYPE));
  EXPECT_FALSE(mVoidType.canCastTo(PrimitiveTypes::FLOAT_TYPE));
  EXPECT_FALSE(mVoidType.canCastTo(PrimitiveTypes::DOUBLE_TYPE));
}

TEST_F(VoidTypeTest, CastToTest) {
  Value* result;
  Value* expressionValue = ConstantFP::get(Type::getDoubleTy(mLLVMContext), 2.5);
  
  result = mVoidType.castTo(mContext, expressionValue, PrimitiveTypes::VOID_TYPE);
  EXPECT_EQ(result, expressionValue);

  EXPECT_EXIT(mVoidType.castTo(mContext, expressionValue, PrimitiveTypes::BOOLEAN_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: can not cast from type 'void' to 'boolean'");
  
  EXPECT_EXIT(mVoidType.castTo(mContext, expressionValue, PrimitiveTypes::CHAR_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: can not cast from type 'void' to 'char'");
  
  EXPECT_EXIT(mVoidType.castTo(mContext, expressionValue, PrimitiveTypes::INT_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: can not cast from type 'void' to 'int'");
  
  EXPECT_EXIT(mVoidType.castTo(mContext, expressionValue, PrimitiveTypes::LONG_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: can not cast from type 'void' to 'long'");
  
  EXPECT_EXIT(mVoidType.castTo(mContext, expressionValue, PrimitiveTypes::FLOAT_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: can not cast from type 'void' to 'float'");
  
  EXPECT_EXIT(mVoidType.castTo(mContext, expressionValue, PrimitiveTypes::DOUBLE_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: can not cast from type 'void' to 'double'");
  
}
