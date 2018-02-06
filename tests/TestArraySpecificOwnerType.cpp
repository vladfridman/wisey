//
//  TestArraySpecificOwnerType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 1/31/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ArraySpecificOwnerType}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>

#include "MockExpression.hpp"
#include "wisey/ArraySpecificOwnerType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ArraySpecificOwnerTypeTest : public Test {
  IRGenerationContext mContext;
  llvm::LLVMContext& mLLVMContext;
  ArrayType* mArrayType;
  ArraySpecificType* mArraySpecificType;
  ArraySpecificOwnerType* mArraySpecificOwnerType;
  NiceMock<MockExpression> mFiveExpression;
  NiceMock<MockExpression> mTenExpression;

  ArraySpecificOwnerTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    llvm::Constant* five = llvm::ConstantInt::get(llvm::Type::getInt64Ty(mLLVMContext), 5);
    ON_CALL(mFiveExpression, generateIR(_, _)).WillByDefault(Return(five));
    ON_CALL(mFiveExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    list<const IExpression*> dimensions;
    dimensions.push_back(&mFiveExpression);
    mArraySpecificType = new ArraySpecificType(PrimitiveTypes::LONG_TYPE, dimensions);
    mArrayType = new ArrayType(PrimitiveTypes::LONG_TYPE, 1u);
    mArraySpecificOwnerType = new ArraySpecificOwnerType(mArraySpecificType);
  }
};

TEST_F(ArraySpecificOwnerTypeTest, getArraySpecificTypeTest) {
  EXPECT_EQ(mArraySpecificType, mArraySpecificOwnerType->getArraySpecificType());
}

TEST_F(ArraySpecificOwnerTypeTest, getNameTest) {
  EXPECT_STREQ("long[]*", mArraySpecificOwnerType->getTypeName().c_str());
}

TEST_F(ArraySpecificOwnerTypeTest, getLLVMTypeTest) {
  llvm::PointerType* arrayLLVMType = mArraySpecificOwnerType->getLLVMType(mContext);
  ASSERT_TRUE(arrayLLVMType->getPointerElementType()->isStructTy());
  llvm::StructType* arrayStruct = (llvm::StructType*) arrayLLVMType->getPointerElementType();
  
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(0));
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(1));
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(2));
  EXPECT_EQ(llvm::ArrayType::get(llvm::Type::getInt64Ty(mLLVMContext), 0u),
            arrayStruct->getElementType(ArrayType::ARRAY_ELEMENTS_START_INDEX));
}

TEST_F(ArraySpecificOwnerTypeTest, getTypeKindTest) {
  EXPECT_EQ(ARRAY_OWNER_TYPE, mArraySpecificOwnerType->getTypeKind());
}

TEST_F(ArraySpecificOwnerTypeTest, canCastToTest) {
  EXPECT_FALSE(mArraySpecificOwnerType->canCastTo(PrimitiveTypes::STRING_TYPE));
  EXPECT_TRUE(mArraySpecificOwnerType->canCastTo(mArraySpecificOwnerType));
  EXPECT_TRUE(mArraySpecificOwnerType->canCastTo(mArrayType));
}

TEST_F(ArraySpecificOwnerTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mArraySpecificOwnerType->canAutoCastTo(PrimitiveTypes::STRING_TYPE));
  EXPECT_TRUE(mArraySpecificOwnerType->canAutoCastTo(mArraySpecificOwnerType));
  EXPECT_TRUE(mArraySpecificOwnerType->canAutoCastTo(mArrayType));
}

TEST_F(ArraySpecificOwnerTypeTest, isOwnerTest) {
  EXPECT_TRUE(mArraySpecificOwnerType->isOwner());
}
