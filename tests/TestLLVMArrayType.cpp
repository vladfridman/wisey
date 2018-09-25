//
//  TestLLVMArrayType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMArrayType}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>

#include "TestPrefix.hpp"
#include "IRGenerationContext.hpp"
#include "LLVMArrayType.hpp"
#include "LLVMPrimitiveTypes.hpp"

using namespace std;
using namespace wisey;

using ::testing::Test;

struct LLVMArrayTypeTest : public Test {
  IRGenerationContext mContext;
  llvm::LLVMContext& mLLVMContext;
  LLVMArrayType* mLLVMArrayType;
  LLVMArrayType* mMultiDimentionalLLVMArrayType;
  
  LLVMArrayTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    list<unsigned long> dimensions;
    dimensions.push_back(5u);
    mLLVMArrayType = mContext.getLLVMArrayType(LLVMPrimitiveTypes::I8, dimensions);
    dimensions.push_back(10u);
    mMultiDimentionalLLVMArrayType = mContext.getLLVMArrayType(LLVMPrimitiveTypes::I64, dimensions);
  }
};

TEST_F(LLVMArrayTypeTest, getArrayTest) {
  const LLVMArrayType* arrayType = mContext.getLLVMArrayType(mLLVMArrayType->getElementType(),
                                                             mLLVMArrayType->getDimensions());
  EXPECT_EQ(arrayType, mLLVMArrayType);
}

TEST_F(LLVMArrayTypeTest, getElementTypeTest) {
  EXPECT_EQ(LLVMPrimitiveTypes::I64, mMultiDimentionalLLVMArrayType->getElementType());
}

TEST_F(LLVMArrayTypeTest, getDimensionsTest) {
  list<unsigned long> dimensions;
  dimensions.push_back(5u);
  dimensions.push_back(10u);
  
  EXPECT_EQ(dimensions.size(), mMultiDimentionalLLVMArrayType->getDimensions().size());
  EXPECT_EQ(dimensions.front(), mMultiDimentionalLLVMArrayType->getDimensions().front());
  EXPECT_EQ(dimensions.back(), mMultiDimentionalLLVMArrayType->getDimensions().back());
}

TEST_F(LLVMArrayTypeTest, getNameTest) {
  EXPECT_STREQ("::llvm::array(::llvm::i8, 5)", mLLVMArrayType->getTypeName().c_str());
}

TEST_F(LLVMArrayTypeTest, getLLVMTypeTest) {
  llvm::ArrayType* arrayLLVMType = mLLVMArrayType->getLLVMType(mContext);
  ASSERT_TRUE(arrayLLVMType->isArrayTy());
  
  EXPECT_EQ(llvm::ArrayType::get(llvm::Type::getInt8Ty(mLLVMContext), 5u), arrayLLVMType);
}

TEST_F(LLVMArrayTypeTest, canCastToTest) {
  EXPECT_FALSE(mLLVMArrayType->canCastTo(mContext, LLVMPrimitiveTypes::I8));
  EXPECT_TRUE(mLLVMArrayType->canCastTo(mContext, mLLVMArrayType));
}

TEST_F(LLVMArrayTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mLLVMArrayType->canAutoCastTo(mContext, LLVMPrimitiveTypes::I8));
  EXPECT_TRUE(mLLVMArrayType->canAutoCastTo(mContext, mLLVMArrayType));
}

TEST_F(LLVMArrayTypeTest, getNumberOfDimensionsTest) {
  EXPECT_EQ(2u, mMultiDimentionalLLVMArrayType->getNumberOfDimensions());
}

TEST_F(LLVMArrayTypeTest, isTypeKindTest) {
  EXPECT_FALSE(mLLVMArrayType->isPrimitive());
  EXPECT_FALSE(mLLVMArrayType->isOwner());
  EXPECT_FALSE(mLLVMArrayType->isReference());
  EXPECT_TRUE(mLLVMArrayType->isArray());
  EXPECT_FALSE(mLLVMArrayType->isFunction());
  EXPECT_FALSE(mLLVMArrayType->isPackage());
  EXPECT_TRUE(mLLVMArrayType->isNative());
  EXPECT_FALSE(mLLVMArrayType->isPointer());
  EXPECT_FALSE(mLLVMArrayType->isImmutable());
}

TEST_F(LLVMArrayTypeTest, isObjectTest) {
  EXPECT_FALSE(mLLVMArrayType->isController());
  EXPECT_FALSE(mLLVMArrayType->isInterface());
  EXPECT_FALSE(mLLVMArrayType->isModel());
  EXPECT_FALSE(mLLVMArrayType->isNode());
}

TEST_F(LLVMArrayTypeTest, getPointerTypeTest) {
  llvm::ArrayType* arrayType = llvm::ArrayType::get(llvm::Type::getInt8Ty(mLLVMContext), 5u);
  EXPECT_EQ(mLLVMArrayType->getLLVMType(mContext)->getPointerTo(),
            arrayType->getPointerTo());
}

TEST_F(LLVMArrayTypeTest, printToStreamTest) {
  stringstream stringStream;
  mLLVMArrayType->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::llvm::array(::llvm::i8, 5)", stringStream.str().c_str());
}

TEST_F(LLVMArrayTypeTest, injectDeathTest) {
  InjectionArgumentList arguments;
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mLLVMArrayType->inject(mContext, arguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: type ::llvm::array(::llvm::i8, 5) is not injectable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}
