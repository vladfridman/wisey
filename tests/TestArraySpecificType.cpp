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

#include <llvm/IR/Constants.h>

#include "MockExpression.hpp"
#include "TestPrefix.hpp"
#include "wisey/ArrayOwnerType.hpp"
#include "wisey/ArraySpecificType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ArraySpecificTypeTest : public Test {
  IRGenerationContext mContext;
  llvm::LLVMContext& mLLVMContext;
  ArraySpecificType* mArraySpecificType;
  ArraySpecificType* mMultiDimentionalArraySpecificType;
  NiceMock<MockExpression> mFiveExpression;
  NiceMock<MockExpression> mTenExpression;

  ArraySpecificTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    llvm::Constant* five = llvm::ConstantInt::get(llvm::Type::getInt64Ty(mLLVMContext), 5);
    ON_CALL(mFiveExpression, generateIR(_, _)).WillByDefault(Return(five));
    ON_CALL(mFiveExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
    ON_CALL(mFiveExpression, printToStream(_, _)).WillByDefault(Invoke(printFive));
    llvm::Constant* ten = llvm::ConstantInt::get(llvm::Type::getInt64Ty(mLLVMContext), 10);
    ON_CALL(mTenExpression, generateIR(_, _)).WillByDefault(Return(ten));
    ON_CALL(mTenExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
    ON_CALL(mTenExpression, printToStream(_, _)).WillByDefault(Invoke(printTen));
    list<const IExpression*> dimensions;
    dimensions.push_back(&mFiveExpression);
    mArraySpecificType = new ArraySpecificType(PrimitiveTypes::LONG, dimensions);
    dimensions.push_back(&mTenExpression);
    mMultiDimentionalArraySpecificType = new ArraySpecificType(PrimitiveTypes::LONG,
                                                               dimensions);
  }
  
  static void printFive(IRGenerationContext& context, iostream& stream) {
    stream << "5";
  }
  
  static void printTen(IRGenerationContext& context, iostream& stream) {
    stream << "10";
  }
};

TEST_F(ArraySpecificTypeTest, getArrayTypeTest) {
  const ArrayType* arrayType = mContext.getArrayType(mArraySpecificType->getElementType(),
                                                     mArraySpecificType->getNumberOfDimensions());
  EXPECT_EQ(arrayType, mArraySpecificType->getArrayType(mContext, 0));
}

TEST_F(ArraySpecificTypeTest, getElementTypeTest) {
  EXPECT_EQ(PrimitiveTypes::LONG, mMultiDimentionalArraySpecificType->getElementType());
}

TEST_F(ArraySpecificTypeTest, getNameTest) {
  EXPECT_STREQ("long[]", mArraySpecificType->getTypeName().c_str());
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

TEST_F(ArraySpecificTypeTest, canCastToTest) {
  EXPECT_FALSE(mArraySpecificType->canCastTo(mContext, PrimitiveTypes::STRING));
  EXPECT_TRUE(mArraySpecificType->canCastTo(mContext, mArraySpecificType));
}

TEST_F(ArraySpecificTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mArraySpecificType->canAutoCastTo(mContext, PrimitiveTypes::STRING));
  EXPECT_TRUE(mArraySpecificType->canAutoCastTo(mContext, mArraySpecificType));
}

TEST_F(ArraySpecificTypeTest, getNumberOfDimensionsTest) {
  EXPECT_EQ(2u, mMultiDimentionalArraySpecificType->getNumberOfDimensions());
}

TEST_F(ArraySpecificTypeTest, isTypeKindTest) {
  EXPECT_FALSE(mArraySpecificType->isPrimitive());
  EXPECT_FALSE(mArraySpecificType->isOwner());
  EXPECT_TRUE(mArraySpecificType->isReference());
  EXPECT_TRUE(mArraySpecificType->isArray());
  EXPECT_FALSE(mArraySpecificType->isFunction());
  EXPECT_FALSE(mArraySpecificType->isPackage());
  EXPECT_FALSE(mArraySpecificType->isNative());
  EXPECT_FALSE(mArraySpecificType->isPointer());
  EXPECT_FALSE(mArraySpecificType->isImmutable());
}

TEST_F(ArraySpecificTypeTest, isObjectTest) {
  EXPECT_FALSE(mArraySpecificType->isController());
  EXPECT_FALSE(mArraySpecificType->isInterface());
  EXPECT_FALSE(mArraySpecificType->isModel());
  EXPECT_FALSE(mArraySpecificType->isNode());
}

TEST_F(ArraySpecificTypeTest, printToStreamTest) {
  stringstream stringStream;
  mMultiDimentionalArraySpecificType->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("long[5][10]", stringStream.str().c_str());
}

TEST_F(ArraySpecificTypeTest, injectDeathTest) {
  Mock::AllowLeak(&mFiveExpression);
  Mock::AllowLeak(&mTenExpression);
  InjectionArgumentList arguments;
  EXPECT_EXIT(mMultiDimentionalArraySpecificType->inject(mContext, arguments, 3),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(3\\): Error: type long\\[\\]\\[\\] is not injectable");
}
