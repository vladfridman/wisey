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

#include <llvm/Support/raw_ostream.h>

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
  llvm::BasicBlock* mBasicBlock;
  string mStringBuffer;
  llvm::raw_string_ostream* mStringStream;

  ArrayTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    vector<unsigned long> dimensions;
    mArrayType = new ArrayType(PrimitiveTypes::LONG_TYPE, 1u);
    mMultiDimentionalArrayType = new ArrayType(PrimitiveTypes::LONG_TYPE, 2u);
 
    llvm::FunctionType* functionType =
    llvm::FunctionType::get(llvm::Type::getInt32Ty(mContext.getLLVMContext()), false);
    llvm::Function* function = llvm::Function::Create(functionType,
                                                      llvm::GlobalValue::InternalLinkage,
                                                      "main",
                                                      mContext.getModule());
    mBasicBlock = llvm::BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new llvm::raw_string_ostream(mStringBuffer);
}
};

TEST_F(ArrayTypeTest, getOwnerTest) {
  const ArrayOwnerType* arrayOwnerType = mArrayType->getOwner();
  EXPECT_EQ(ARRAY_OWNER_TYPE, arrayOwnerType->getTypeKind());
  EXPECT_EQ(mArrayType, arrayOwnerType->getArrayType(mContext));
}

TEST_F(ArrayTypeTest, getElementTypeTest) {
  EXPECT_EQ(PrimitiveTypes::LONG_TYPE, mMultiDimentionalArrayType->getElementType());
}

TEST_F(ArrayTypeTest, getNumberOfDimensionsTest) {
  EXPECT_EQ(2u, mMultiDimentionalArrayType->getNumberOfDimensions());
}

TEST_F(ArrayTypeTest, getNameTest) {
  EXPECT_STREQ("long[]", mArrayType->getTypeName().c_str());
}

TEST_F(ArrayTypeTest, getLLVMTypeTest) {
  llvm::PointerType* arrayLLVMType = mArrayType->getLLVMType(mContext);
  ASSERT_TRUE(arrayLLVMType->getPointerElementType()->isStructTy());
  llvm::StructType* arrayStruct = (llvm::StructType*) arrayLLVMType->getPointerElementType();

  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(0));
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(1));
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(2));
  EXPECT_EQ(llvm::ArrayType::get(llvm::Type::getInt64Ty(mLLVMContext), 0u),
            arrayStruct->getElementType(ArrayType::ARRAY_ELEMENTS_START_INDEX));
}

TEST_F(ArrayTypeTest, getTypeKindTest) {
  EXPECT_EQ(ARRAY_TYPE, mArrayType->getTypeKind());
}

TEST_F(ArrayTypeTest, canCastToTest) {
  EXPECT_FALSE(mArrayType->canCastTo(mContext, PrimitiveTypes::STRING_TYPE));
  EXPECT_TRUE(mArrayType->canCastTo(mContext, mArrayType));
}

TEST_F(ArrayTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mArrayType->canAutoCastTo(mContext, PrimitiveTypes::STRING_TYPE));
  EXPECT_TRUE(mArrayType->canAutoCastTo(mContext, mArrayType));
}

TEST_F(ArrayTypeTest, isOwnerTest) {
  EXPECT_FALSE(mArrayType->isOwner());
}

TEST_F(ArrayTypeTest, isReferenceTest) {
  EXPECT_TRUE(mArrayType->isReference());
}

TEST_F(ArrayTypeTest, allocateVariableTest) {
  mArrayType->allocateVariable(mContext, "temp");
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = alloca { i64, i64, i64, [0 x i64] }*"
  "\n  store { i64, i64, i64, [0 x i64] }* null, { i64, i64, i64, [0 x i64] }** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}
