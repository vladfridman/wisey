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
  
  ArrayOwnerTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    mArrayType = new ArrayType(PrimitiveTypes::LONG_TYPE, 5u);
    mArrayOwnerType = new ArrayOwnerType(mArrayType);
  }
};

TEST_F(ArrayOwnerTypeTest, getArrayTypeTest) {
  EXPECT_EQ(mArrayType, mArrayOwnerType->getArrayType());
}

TEST_F(ArrayOwnerTypeTest, getNameTest) {
  EXPECT_STREQ("long[5]*", mArrayOwnerType->getTypeName().c_str());
}

TEST_F(ArrayOwnerTypeTest, getLLVMTypeTest) {
  vector<llvm::Type*> dimentionTypes;
  dimentionTypes.push_back(llvm::Type::getInt64Ty(mLLVMContext));
  llvm::StructType* subStruct = llvm::StructType::get(mLLVMContext, dimentionTypes);
  
  llvm::PointerType* arrayLLVMType = mArrayType->getLLVMType(mContext);
  ASSERT_TRUE(arrayLLVMType->getPointerElementType()->isStructTy());
  llvm::StructType* arrayStruct = (llvm::StructType*) arrayLLVMType->getPointerElementType();

  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(0));
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(1));
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(2));
  EXPECT_EQ(subStruct, arrayStruct->getElementType(3));
  EXPECT_EQ(llvm::ArrayType::get(llvm::Type::getInt64Ty(mLLVMContext), 5u),
            arrayStruct->getElementType(4));
}

TEST_F(ArrayOwnerTypeTest, getTypeKindTest) {
  EXPECT_EQ(ARRAY_OWNER_TYPE, mArrayOwnerType->getTypeKind());
}

TEST_F(ArrayOwnerTypeTest, canCastToTest) {
  EXPECT_FALSE(mArrayOwnerType->canCastTo(PrimitiveTypes::STRING_TYPE));
  EXPECT_TRUE(mArrayOwnerType->canCastTo(mArrayOwnerType));
  EXPECT_TRUE(mArrayOwnerType->canCastTo(mArrayType));
}

TEST_F(ArrayOwnerTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mArrayOwnerType->canAutoCastTo(PrimitiveTypes::STRING_TYPE));
  EXPECT_TRUE(mArrayOwnerType->canAutoCastTo(mArrayOwnerType));
  EXPECT_TRUE(mArrayOwnerType->canAutoCastTo(mArrayType));
}

TEST_F(ArrayOwnerTypeTest, isOwnerTest) {
  EXPECT_TRUE(mArrayOwnerType->isOwner());
}

