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
  
  ArrayTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    mArrayType = new ArrayType(PrimitiveTypes::LONG_TYPE, 5u);
  }
};

TEST_F(ArrayTypeTest, getBaseTypeTest) {
  EXPECT_EQ(PrimitiveTypes::LONG_TYPE, mArrayType->getBaseType());
}

TEST_F(ArrayTypeTest, getSizeTest) {
  EXPECT_EQ(5u, mArrayType->getSize());
}

TEST_F(ArrayTypeTest, getNameTest) {
  EXPECT_STREQ("long[5]", mArrayType->getTypeName().c_str());
}

TEST_F(ArrayTypeTest, getLLVMTypeTest) {
  EXPECT_EQ(llvm::ArrayType::get(llvm::Type::getInt64Ty(mLLVMContext), 5u),
            mArrayType->getLLVMType(mLLVMContext));
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

