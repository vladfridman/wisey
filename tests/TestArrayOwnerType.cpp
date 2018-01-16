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

TEST_F(ArrayOwnerTypeTest, getBaseTypeTest) {
  EXPECT_EQ(PrimitiveTypes::LONG_TYPE, mArrayOwnerType->getBaseType());
}

TEST_F(ArrayOwnerTypeTest, getScalarTypeTest) {
  ArrayOwnerType* outerArray = new ArrayOwnerType(new ArrayType(mArrayType, 3u));
  EXPECT_EQ(PrimitiveTypes::LONG_TYPE, outerArray->getScalarType());
}

TEST_F(ArrayOwnerTypeTest, getSizeTest) {
  EXPECT_EQ(5u, mArrayOwnerType->getSize());
}

TEST_F(ArrayOwnerTypeTest, getNameTest) {
  EXPECT_STREQ("long[5]*", mArrayOwnerType->getTypeName().c_str());
}

TEST_F(ArrayOwnerTypeTest, getLLVMTypeTest) {
  EXPECT_EQ(llvm::ArrayType::get(llvm::Type::getInt64Ty(mLLVMContext), 5u),
            mArrayOwnerType->getLLVMType(mContext));
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
