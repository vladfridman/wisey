//
//  TestArrayExactOwnerType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 2/6/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ArrayExactOwnerType}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>

#include "wisey/ArrayExactOwnerType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace std;
using namespace wisey;

using ::testing::Test;

struct ArrayExactOwnerTypeTest : public Test {
  IRGenerationContext mContext;
  llvm::LLVMContext& mLLVMContext;
  ArrayType* mArrayType;
  ArrayExactType* mArrayExactType;
  ArrayExactOwnerType* mArrayExactOwnerType;
  
  ArrayExactOwnerTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    list<unsigned long> dimensions;
    dimensions.push_back(5u);
    mArrayExactType = new ArrayExactType(PrimitiveTypes::LONG_TYPE, dimensions);
    mArrayType = new ArrayType(PrimitiveTypes::LONG_TYPE, 1u);
    mArrayExactOwnerType = new ArrayExactOwnerType(mArrayExactType);
  }
};

TEST_F(ArrayExactOwnerTypeTest, getArrayExactTypeTest) {
  EXPECT_EQ(mArrayExactType, mArrayExactOwnerType->getArrayExactType());
}

TEST_F(ArrayExactOwnerTypeTest, getNameTest) {
  EXPECT_STREQ("long[5]*", mArrayExactOwnerType->getTypeName().c_str());
}

TEST_F(ArrayExactOwnerTypeTest, getLLVMTypeTest) {
  llvm::PointerType* arrayLLVMType = mArrayExactOwnerType->getLLVMType(mContext);
  ASSERT_TRUE(arrayLLVMType->getPointerElementType()->isStructTy());
  llvm::StructType* arrayStruct = (llvm::StructType*) arrayLLVMType->getPointerElementType();
  
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(0));
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(1));
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(2));
  EXPECT_EQ(llvm::ArrayType::get(llvm::Type::getInt64Ty(mLLVMContext), 5u),
            arrayStruct->getElementType(ArrayType::ARRAY_ELEMENTS_START_INDEX));
}

TEST_F(ArrayExactOwnerTypeTest, canCastToTest) {
  EXPECT_FALSE(mArrayExactOwnerType->canCastTo(mContext, PrimitiveTypes::STRING_TYPE));
  EXPECT_TRUE(mArrayExactOwnerType->canCastTo(mContext, mArrayExactOwnerType));
  EXPECT_TRUE(mArrayExactOwnerType->canCastTo(mContext, mArrayType));
}

TEST_F(ArrayExactOwnerTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mArrayExactOwnerType->canAutoCastTo(mContext, PrimitiveTypes::STRING_TYPE));
  EXPECT_TRUE(mArrayExactOwnerType->canAutoCastTo(mContext, mArrayExactOwnerType));
  EXPECT_TRUE(mArrayExactOwnerType->canAutoCastTo(mContext, mArrayType));
}

TEST_F(ArrayExactOwnerTypeTest, isPrimitiveTest) {
  EXPECT_FALSE(mArrayExactOwnerType->isPrimitive());
}

TEST_F(ArrayExactOwnerTypeTest, isOwnerTest) {
  EXPECT_TRUE(mArrayExactOwnerType->isOwner());
}

TEST_F(ArrayExactOwnerTypeTest, isReferenceTest) {
  EXPECT_FALSE(mArrayExactOwnerType->isReference());
}

TEST_F(ArrayExactOwnerTypeTest, isArrayTest) {
  EXPECT_TRUE(mArrayExactOwnerType->isArray());
}

TEST_F(ArrayExactOwnerTypeTest, isFunctionTest) {
  EXPECT_FALSE(mArrayExactOwnerType->isFunction());
}

TEST_F(ArrayExactOwnerTypeTest, isPackageTest) {
  EXPECT_FALSE(mArrayExactOwnerType->isPackage());
}

TEST_F(ArrayExactOwnerTypeTest, isObjectTest) {
  EXPECT_FALSE(mArrayExactOwnerType->isController());
  EXPECT_FALSE(mArrayExactOwnerType->isInterface());
  EXPECT_FALSE(mArrayExactOwnerType->isModel());
  EXPECT_FALSE(mArrayExactOwnerType->isNode());
  EXPECT_FALSE(mArrayExactOwnerType->isThread());
  EXPECT_FALSE(mArrayExactOwnerType->isNative());
}

TEST_F(ArrayExactOwnerTypeTest, getArrayTypeTest) {
  EXPECT_EQ(mContext.getArrayType(PrimitiveTypes::LONG_TYPE, 1u),
            mArrayExactOwnerType->getArrayType(mContext));
}
