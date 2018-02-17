//
//  TestArrayOwnerType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 1/16/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ArrayOwnerType}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/Support/raw_ostream.h>

#include "wisey/ArrayOwnerType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace std;
using namespace wisey;

using ::testing::Test;

struct ArrayOwnerTypeTest : public Test {
  IRGenerationContext mContext;
  llvm::LLVMContext& mLLVMContext;
  ArrayType* mArrayType;
  ArrayOwnerType* mArrayOwnerType;
  llvm::BasicBlock* mBasicBlock;
  string mStringBuffer;
  llvm::raw_string_ostream* mStringStream;

  ArrayOwnerTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    mArrayType = new ArrayType(PrimitiveTypes::LONG_TYPE, 1u);
    mArrayOwnerType = new ArrayOwnerType(mArrayType);

    llvm::FunctionType* functionType =
    llvm::FunctionType::get(llvm::Type::getInt32Ty(mContext.getLLVMContext()), false);
    llvm::Function* function = llvm::Function::Create(functionType,
                                                      llvm::GlobalValue::InternalLinkage,
                                                      "main",
                                                      mContext.getModule());
    mBasicBlock = llvm::BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new llvm::raw_string_ostream(mStringBuffer);
  }
};

TEST_F(ArrayOwnerTypeTest, getArrayTypeTest) {
  EXPECT_EQ(mArrayType, mArrayOwnerType->getArrayType(mContext));
}

TEST_F(ArrayOwnerTypeTest, getNameTest) {
  EXPECT_STREQ("long[]*", mArrayOwnerType->getTypeName().c_str());
}

TEST_F(ArrayOwnerTypeTest, getLLVMTypeTest) {
  llvm::PointerType* arrayLLVMType = mArrayOwnerType->getLLVMType(mContext);
  ASSERT_TRUE(arrayLLVMType->getPointerElementType()->isStructTy());
  llvm::StructType* arrayStruct = (llvm::StructType*) arrayLLVMType->getPointerElementType();

  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(0));
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(1));
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(2));
  EXPECT_EQ(llvm::ArrayType::get(llvm::Type::getInt64Ty(mLLVMContext), 0u),
            arrayStruct->getElementType(ArrayType::ARRAY_ELEMENTS_START_INDEX));
}

TEST_F(ArrayOwnerTypeTest, getTypeKindTest) {
  EXPECT_EQ(ARRAY_OWNER_TYPE, mArrayOwnerType->getTypeKind());
}

TEST_F(ArrayOwnerTypeTest, canCastToTest) {
  EXPECT_FALSE(mArrayOwnerType->canCastTo(mContext, PrimitiveTypes::STRING_TYPE));
  EXPECT_TRUE(mArrayOwnerType->canCastTo(mContext, mArrayOwnerType));
  EXPECT_TRUE(mArrayOwnerType->canCastTo(mContext, mArrayType));
}

TEST_F(ArrayOwnerTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mArrayOwnerType->canAutoCastTo(mContext, PrimitiveTypes::STRING_TYPE));
  EXPECT_TRUE(mArrayOwnerType->canAutoCastTo(mContext, mArrayOwnerType));
  EXPECT_TRUE(mArrayOwnerType->canAutoCastTo(mContext, mArrayType));
}

TEST_F(ArrayOwnerTypeTest, isOwnerTest) {
  EXPECT_TRUE(mArrayOwnerType->isOwner());
}

TEST_F(ArrayOwnerTypeTest, allocateVariableTest) {
  mArrayOwnerType->allocateVariable(mContext, "temp");
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = alloca { i64, i64, i64, [0 x i64] }*"
  "\n  store { i64, i64, i64, [0 x i64] }* null, { i64, i64, i64, [0 x i64] }** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}
