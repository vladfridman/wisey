//
//  TestArraySpecificType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 1/28/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ArraySpecificType}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "wisey/ArrayOwnerType.hpp"
#include "wisey/ArraySpecificType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace std;
using namespace wisey;

using ::testing::Test;

struct ArraySpecificTypeTest : public Test {
  IRGenerationContext mContext;
  llvm::LLVMContext& mLLVMContext;
  ArraySpecificType* mArraySpecificType;
  ArraySpecificType* mMultiDimentionalArraySpecificType;
  
  ArraySpecificTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    list<unsigned long> dimensions;
    dimensions.push_back(5u);
    mArraySpecificType = new ArraySpecificType(PrimitiveTypes::LONG_TYPE, dimensions);
    dimensions.push_back(10u);
    mMultiDimentionalArraySpecificType = new ArraySpecificType(PrimitiveTypes::LONG_TYPE,
                                                               dimensions);
  }
};

TEST_F(ArraySpecificTypeTest, getArrayTest) {
  const ArrayType* arrayType = mContext.getArrayType(mArraySpecificType->getElementType(),
                                                     mArraySpecificType->getNumberOfDimensions());
  EXPECT_EQ(arrayType, mArraySpecificType->getArrayType(mContext));
}

TEST_F(ArraySpecificTypeTest, getElementTypeTest) {
  EXPECT_EQ(PrimitiveTypes::LONG_TYPE, mMultiDimentionalArraySpecificType->getElementType());
}

TEST_F(ArraySpecificTypeTest, getNameTest) {
  EXPECT_STREQ("long[5]", mArraySpecificType->getTypeName().c_str());
}

TEST_F(ArraySpecificTypeTest, getLLVMTypeTest) {
  llvm::PointerType* arrayLLVMType = mArraySpecificType->getLLVMType(mContext);
  ASSERT_TRUE(arrayLLVMType->getPointerElementType()->isStructTy());
  llvm::StructType* arrayStruct = (llvm::StructType*) arrayLLVMType->getPointerElementType();
  
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(0));
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(1));
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(2));
  EXPECT_EQ(llvm::ArrayType::get(llvm::Type::getInt64Ty(mLLVMContext), 0u),
            arrayStruct->getElementType(ArrayType::ARRAY_ELEMENTS_START_INDEX));
}

TEST_F(ArraySpecificTypeTest, getTypeKindTest) {
  EXPECT_EQ(ARRAY_TYPE, mArraySpecificType->getTypeKind());
}

TEST_F(ArraySpecificTypeTest, canCastToTest) {
  EXPECT_FALSE(mArraySpecificType->canCastTo(PrimitiveTypes::STRING_TYPE));
  EXPECT_TRUE(mArraySpecificType->canCastTo(mArraySpecificType));
}

TEST_F(ArraySpecificTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mArraySpecificType->canAutoCastTo(PrimitiveTypes::STRING_TYPE));
  EXPECT_TRUE(mArraySpecificType->canAutoCastTo(mArraySpecificType));
}

TEST_F(ArraySpecificTypeTest, getNumberOfDimensionsTest) {
  EXPECT_EQ(2u, mMultiDimentionalArraySpecificType->getNumberOfDimensions());
}

TEST_F(ArraySpecificTypeTest, isOwnerTest) {
  EXPECT_FALSE(mArraySpecificType->isOwner());
}
