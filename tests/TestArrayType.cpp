//
//  TestArrayType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 12/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ArrayType}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "wisey/ArrayOwnerType.hpp"
#include "wisey/ArrayType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace std;
using namespace wisey;

using ::testing::Test;

struct ArrayTypeTest : public Test {
  IRGenerationContext mContext;
  llvm::LLVMContext& mLLVMContext;
  ArrayType* mArrayType;
  ArrayType* mMultiDimentionalArrayType;
  
  ArrayTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    mArrayType = new ArrayType(PrimitiveTypes::LONG_TYPE, 5u);
    mMultiDimentionalArrayType = new ArrayType(mArrayType, 10u);
  }
};

TEST_F(ArrayTypeTest, getOwnerTest) {
  const ArrayOwnerType* arrayOwnerType = mArrayType->getOwner();
  EXPECT_EQ(ARRAY_OWNER_TYPE, arrayOwnerType->getTypeKind());
  EXPECT_EQ(mArrayType, arrayOwnerType->getArrayType());
}

TEST_F(ArrayTypeTest, getBaseTypeTest) {
  EXPECT_EQ(PrimitiveTypes::LONG_TYPE, mArrayType->getBaseType());
}

TEST_F(ArrayTypeTest, getScalarTypeTest) {
  ArrayType* outerArray = new ArrayType(mArrayType, 3u);
  EXPECT_EQ(mArrayType, outerArray->getBaseType());
  EXPECT_EQ(PrimitiveTypes::LONG_TYPE, outerArray->getScalarType());
}

TEST_F(ArrayTypeTest, getSizeTest) {
  EXPECT_EQ(5u, mArrayType->getSize());
}

TEST_F(ArrayTypeTest, getNameTest) {
  EXPECT_STREQ("long[5]", mArrayType->getTypeName().c_str());
}

TEST_F(ArrayTypeTest, getLLVMTypeTest) {
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

TEST_F(ArrayTypeTest, getTypeKindTest) {
  EXPECT_EQ(ARRAY_TYPE, mArrayType->getTypeKind());
}

TEST_F(ArrayTypeTest, canCastToTest) {
  EXPECT_FALSE(mArrayType->canCastTo(PrimitiveTypes::STRING_TYPE));
  EXPECT_TRUE(mArrayType->canCastTo(mArrayType));
}

TEST_F(ArrayTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mArrayType->canAutoCastTo(PrimitiveTypes::STRING_TYPE));
  EXPECT_TRUE(mArrayType->canAutoCastTo(mArrayType));
}

TEST_F(ArrayTypeTest, getDimensionsSizeTest) {
  EXPECT_EQ(2u, mMultiDimentionalArrayType->getDimentionsSize());
}

TEST_F(ArrayTypeTest, getLinearSizeTest) {
  EXPECT_EQ(50u, mMultiDimentionalArrayType->getLinearSize());
}

TEST_F(ArrayTypeTest, isOwnerTest) {
  EXPECT_FALSE(mArrayType->isOwner());
}
