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

TEST_F(ArrayExactOwnerTypeTest, computeSizeTest) {
  llvm::PointerType* arrayLLVMType = mArrayExactOwnerType->getLLVMType(mContext);
  llvm::StructType* arrayStruct = (llvm::StructType*) arrayLLVMType->getPointerElementType();
  llvm::Value* expected = llvm::ConstantExpr::getSizeOf(arrayStruct);
  EXPECT_EQ(expected, mArrayExactOwnerType->computeSize(mContext));
}

TEST_F(ArrayExactOwnerTypeTest, getTypeKindTest) {
  EXPECT_EQ(ARRAY_OWNER_TYPE, mArrayExactOwnerType->getTypeKind());
}

TEST_F(ArrayExactOwnerTypeTest, canCastToTest) {
  EXPECT_FALSE(mArrayExactOwnerType->canCastTo(PrimitiveTypes::STRING_TYPE));
  EXPECT_TRUE(mArrayExactOwnerType->canCastTo(mArrayExactOwnerType));
  EXPECT_TRUE(mArrayExactOwnerType->canCastTo(mArrayType));
}

TEST_F(ArrayExactOwnerTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mArrayExactOwnerType->canAutoCastTo(PrimitiveTypes::STRING_TYPE));
  EXPECT_TRUE(mArrayExactOwnerType->canAutoCastTo(mArrayExactOwnerType));
  EXPECT_TRUE(mArrayExactOwnerType->canAutoCastTo(mArrayType));
}

TEST_F(ArrayExactOwnerTypeTest, isOwnerTest) {
  EXPECT_TRUE(mArrayExactOwnerType->isOwner());
}
