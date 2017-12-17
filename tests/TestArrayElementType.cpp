//
//  TestArrayElementType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 12/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ArrayType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "wisey/ArrayElementType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct ArrayElementTypeTest : public Test {
  IRGenerationContext mContext;
  llvm::LLVMContext& mLLVMContext;
  ArrayElementType* mArrayElementType;
  BasicBlock* mBasicBlock;
  Function* mFunction;
  string mStringBuffer;
  raw_string_ostream* mStringStream;

  ArrayElementTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    mArrayElementType = new ArrayElementType(PrimitiveTypes::LONG_TYPE);

    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "main",
                                 mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
}
};

TEST_F(ArrayElementTypeTest, getBaseTypeTest) {
  EXPECT_EQ(PrimitiveTypes::LONG_TYPE, mArrayElementType->getBaseType());
}

TEST_F(ArrayElementTypeTest, getTypeNameTest) {
  EXPECT_STREQ("ArrayElement of type long", mArrayElementType->getTypeName().c_str());
}

TEST_F(ArrayElementTypeTest, getLLVMTypeTest) {
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext)->getPointerTo(),
            mArrayElementType->getLLVMType(mContext));
}

TEST_F(ArrayElementTypeTest, getTypeKindTest) {
  EXPECT_EQ(ARRAY_ELEMENT_TYPE, mArrayElementType->getTypeKind());
}

TEST_F(ArrayElementTypeTest, canCastToTest) {
  mArrayElementType->canCastTo(PrimitiveTypes::STRING_TYPE);
  EXPECT_FALSE(mArrayElementType->canCastTo(PrimitiveTypes::VOID_TYPE));
  EXPECT_TRUE(mArrayElementType->canCastTo(PrimitiveTypes::INT_TYPE));
}

TEST_F(ArrayElementTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mArrayElementType->canAutoCastTo(PrimitiveTypes::VOID_TYPE));
  EXPECT_TRUE(mArrayElementType->canAutoCastTo(PrimitiveTypes::LONG_TYPE));
}

TEST_F(ArrayElementTypeTest, castToTest) {
  Value* store = IRWriter::newAllocaInst(mContext, Type::getInt64Ty(mLLVMContext), "");
  PrimitiveTypes::LONG_TYPE->getArrayElementType()->castTo(mContext,
                                                           store,
                                                           PrimitiveTypes::LONG_TYPE,
                                                           0);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = alloca i64"
  "\n  %1 = load i64, i64* %0\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ArrayElementTypeTest, getArrayElementTypeDeathTest) {
  EXPECT_EXIT(mArrayElementType->getArrayElementType(),
              ::testing::ExitedWithCode(1),
              "Error: Can not get an array element of another array element");
}

