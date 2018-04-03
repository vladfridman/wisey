//
//  TestLLVMFunctionType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMFunctionType}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMFunctionType.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct LLVMFunctionTypeTest : public Test {
  IRGenerationContext mContext;
  llvm::LLVMContext& mLLVMContext;
  LLVMFunctionType* mLLVMFunctionType;
  
  LLVMFunctionTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    vector<const IType*> argumentTypes;
    argumentTypes.push_back(LLVMPrimitiveTypes::I16);
    argumentTypes.push_back(LLVMPrimitiveTypes::I64->getPointerType());
    mLLVMFunctionType = mContext.getLLVMFunctionType(LLVMPrimitiveTypes::I8, argumentTypes);
  }
};

TEST_F(LLVMFunctionTypeTest, getLLVMFunctionTypeTest) {
  const LLVMFunctionType* functionType =
  mContext.getLLVMFunctionType(mLLVMFunctionType->getReturnType(),
                               mLLVMFunctionType->getArgumentTypes());
  EXPECT_EQ(functionType, mLLVMFunctionType);
}

TEST_F(LLVMFunctionTypeTest, getReturnTypeTest) {
  EXPECT_EQ(LLVMPrimitiveTypes::I8, mLLVMFunctionType->getReturnType());
}

TEST_F(LLVMFunctionTypeTest, getArgumentTypesTest) {
  EXPECT_EQ(2u, mLLVMFunctionType->getArgumentTypes().size());
  EXPECT_EQ(LLVMPrimitiveTypes::I16, mLLVMFunctionType->getArgumentTypes().front());
  EXPECT_EQ(LLVMPrimitiveTypes::I64->getPointerType(),
            mLLVMFunctionType->getArgumentTypes().back());
}

TEST_F(LLVMFunctionTypeTest, getNameTest) {
  EXPECT_STREQ("::llvm::i8 (::llvm::i16, ::llvm::i64::pointer)",
               mLLVMFunctionType->getTypeName().c_str());
}

TEST_F(LLVMFunctionTypeTest, getLLVMTypeTest) {
  vector<Type*> argTypesArray;
  argTypesArray.push_back(Type::getInt16Ty(mLLVMContext));
  argTypesArray.push_back(Type::getInt64Ty(mLLVMContext)->getPointerTo());
  FunctionType* expected = FunctionType::get(Type::getInt8Ty(mLLVMContext), argTypesArray, false);

  EXPECT_EQ(expected, mLLVMFunctionType->getLLVMType(mContext));
}

TEST_F(LLVMFunctionTypeTest, canCastToTest) {
  EXPECT_FALSE(mLLVMFunctionType->canCastTo(mContext, LLVMPrimitiveTypes::I8));
  EXPECT_TRUE(mLLVMFunctionType->canCastTo(mContext, mLLVMFunctionType));
}

TEST_F(LLVMFunctionTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mLLVMFunctionType->canAutoCastTo(mContext, LLVMPrimitiveTypes::I8));
  EXPECT_TRUE(mLLVMFunctionType->canAutoCastTo(mContext, mLLVMFunctionType));
}

TEST_F(LLVMFunctionTypeTest, isTypeKindTest) {
  EXPECT_FALSE(mLLVMFunctionType->isPrimitive());
  EXPECT_FALSE(mLLVMFunctionType->isOwner());
  EXPECT_FALSE(mLLVMFunctionType->isReference());
  EXPECT_FALSE(mLLVMFunctionType->isArray());
  EXPECT_TRUE(mLLVMFunctionType->isFunction());
  EXPECT_FALSE(mLLVMFunctionType->isPackage());
  EXPECT_TRUE(mLLVMFunctionType->isNative());
  EXPECT_FALSE(mLLVMFunctionType->isPointer());
}

TEST_F(LLVMFunctionTypeTest, isObjectTest) {
  EXPECT_FALSE(mLLVMFunctionType->isController());
  EXPECT_FALSE(mLLVMFunctionType->isInterface());
  EXPECT_FALSE(mLLVMFunctionType->isModel());
  EXPECT_FALSE(mLLVMFunctionType->isNode());
  EXPECT_FALSE(mLLVMFunctionType->isThread());
}

TEST_F(LLVMFunctionTypeTest, getPointerTypeTest) {
  vector<Type*> argTypesArray;
  argTypesArray.push_back(Type::getInt16Ty(mLLVMContext));
  argTypesArray.push_back(Type::getInt64Ty(mLLVMContext)->getPointerTo());
  FunctionType* expected = FunctionType::get(Type::getInt8Ty(mLLVMContext), argTypesArray, false);

  EXPECT_EQ(expected->getPointerTo(), mLLVMFunctionType->getPointerType()->getLLVMType(mContext));
}

TEST_F(LLVMFunctionTypeTest, printToStreamTest) {
  stringstream stringStream;
  mLLVMFunctionType->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::llvm::i8 (::llvm::i16, ::llvm::i64::pointer)", stringStream.str().c_str());
}
