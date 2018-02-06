//
//  TestArrayExactType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 2/6/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ArrayExactType}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>

#include "wisey/ArrayExactType.hpp"
#include "wisey/ArrayOwnerType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace std;
using namespace wisey;

using ::testing::Test;

struct ArrayExactTypeTest : public Test {
  IRGenerationContext mContext;
  llvm::LLVMContext& mLLVMContext;
  ArrayExactType* mArrayExactType;
  ArrayExactType* mMultiDimentionalArrayExactType;
  
  ArrayExactTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    list<unsigned long> dimensions;
    dimensions.push_back(5u);
    mArrayExactType = new ArrayExactType(PrimitiveTypes::LONG_TYPE, dimensions);
    dimensions.push_back(10u);
    mMultiDimentionalArrayExactType = new ArrayExactType(PrimitiveTypes::LONG_TYPE, dimensions);
  }
};

TEST_F(ArrayExactTypeTest, getArrayTest) {
  const ArrayType* arrayType = mContext.getArrayType(mArrayExactType->getElementType(),
                                                     mArrayExactType->getDimensions().size());
  EXPECT_EQ(arrayType, mArrayExactType->getArrayType(mContext));
}

TEST_F(ArrayExactTypeTest, getElementTypeTest) {
  EXPECT_EQ(PrimitiveTypes::LONG_TYPE, mMultiDimentionalArrayExactType->getElementType());
}

TEST_F(ArrayExactTypeTest, getDimensionsTest) {
  list<unsigned long> dimensions;
  dimensions.push_back(5u);
  dimensions.push_back(10u);
  
  EXPECT_EQ(dimensions.size(), mMultiDimentionalArrayExactType->getDimensions().size());
  EXPECT_EQ(dimensions.front(), mMultiDimentionalArrayExactType->getDimensions().front());
  EXPECT_EQ(dimensions.back(), mMultiDimentionalArrayExactType->getDimensions().back());
}

TEST_F(ArrayExactTypeTest, getNameTest) {
  EXPECT_STREQ("long[5]", mArrayExactType->getTypeName().c_str());
}

TEST_F(ArrayExactTypeTest, getLLVMTypeTest) {
  llvm::PointerType* arrayLLVMType = mArrayExactType->getLLVMType(mContext);
  ASSERT_TRUE(arrayLLVMType->getPointerElementType()->isStructTy());
  llvm::StructType* arrayStruct = (llvm::StructType*) arrayLLVMType->getPointerElementType();
  
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(0));
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(1));
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(2));
  EXPECT_EQ(llvm::ArrayType::get(llvm::Type::getInt64Ty(mLLVMContext), 5u),
            arrayStruct->getElementType(ArrayType::ARRAY_ELEMENTS_START_INDEX));
}

TEST_F(ArrayExactTypeTest, computeSizeTest) {
  llvm::PointerType* arrayLLVMType = mArrayExactType->getLLVMType(mContext);
  llvm::StructType* arrayStruct = (llvm::StructType*) arrayLLVMType->getPointerElementType();
  llvm::Value* expected = llvm::ConstantExpr::getSizeOf(arrayStruct);
  EXPECT_EQ(expected, mArrayExactType->computeSize(mContext));
}

TEST_F(ArrayExactTypeTest, getTypeKindTest) {
  EXPECT_EQ(ARRAY_TYPE, mArrayExactType->getTypeKind());
}

TEST_F(ArrayExactTypeTest, canCastToTest) {
  EXPECT_FALSE(mArrayExactType->canCastTo(PrimitiveTypes::STRING_TYPE));
  EXPECT_TRUE(mArrayExactType->canCastTo(mArrayExactType));
}

TEST_F(ArrayExactTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mArrayExactType->canAutoCastTo(PrimitiveTypes::STRING_TYPE));
  EXPECT_TRUE(mArrayExactType->canAutoCastTo(mArrayExactType));
}

TEST_F(ArrayExactTypeTest, getNumberOfDimensionsTest) {
  EXPECT_EQ(2u, mMultiDimentionalArrayExactType->getNumberOfDimensions());
}

TEST_F(ArrayExactTypeTest, isOwnerTest) {
  EXPECT_FALSE(mArrayExactType->isOwner());
}
