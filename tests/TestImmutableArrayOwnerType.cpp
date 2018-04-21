//
//  TestImmutableArrayOwnerType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ImmutableArrayOwnerType}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Constants.h>

#include "MockConcreteObjectType.hpp"
#include "TestPrefix.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/ImmutableArrayOwnerType.hpp"
#include "wisey/ImmutableArrayType.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ImmutableArrayOwnerTypeTest : public Test {
  IRGenerationContext mContext;
  llvm::LLVMContext& mLLVMContext;
  ArrayType* mArrayType;
  ImmutableArrayType* mImmutableArrayType;
  ImmutableArrayOwnerType* mImmutableArrayOwnerType;
  llvm::BasicBlock* mBasicBlock;
  string mStringBuffer;
  llvm::raw_string_ostream* mStringStream;
  
  ImmutableArrayOwnerTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    mArrayType = new ArrayType(PrimitiveTypes::LONG_TYPE, 1u);
    mImmutableArrayType = new ImmutableArrayType(mArrayType);
    mImmutableArrayOwnerType = new ImmutableArrayOwnerType(mImmutableArrayType);
    
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

TEST_F(ImmutableArrayOwnerTypeTest, getArrayTypeTest) {
  EXPECT_EQ(mArrayType, mImmutableArrayOwnerType->getArrayType(mContext));
}

TEST_F(ImmutableArrayOwnerTypeTest, getNameTest) {
  EXPECT_STREQ("immutable long[]*", mImmutableArrayOwnerType->getTypeName().c_str());
}

TEST_F(ImmutableArrayOwnerTypeTest, getLLVMTypeTest) {
  llvm::PointerType* arrayLLVMType = mImmutableArrayOwnerType->getLLVMType(mContext);
  ASSERT_TRUE(arrayLLVMType->getPointerElementType()->isStructTy());
  llvm::StructType* arrayStruct = (llvm::StructType*) arrayLLVMType->getPointerElementType();
  
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(0));
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(1));
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(2));
  EXPECT_EQ(llvm::ArrayType::get(llvm::Type::getInt64Ty(mLLVMContext), 0u),
            arrayStruct->getElementType(ArrayType::ARRAY_ELEMENTS_START_INDEX));
}

TEST_F(ImmutableArrayOwnerTypeTest, canCastToTest) {
  EXPECT_FALSE(mImmutableArrayOwnerType->canCastTo(mContext, PrimitiveTypes::STRING_TYPE));
  EXPECT_TRUE(mImmutableArrayOwnerType->canCastTo(mContext, mImmutableArrayOwnerType));
  EXPECT_TRUE(mImmutableArrayOwnerType->canCastTo(mContext, mImmutableArrayType));
  EXPECT_FALSE(mImmutableArrayOwnerType->canCastTo(mContext, mArrayType));
}

TEST_F(ImmutableArrayOwnerTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mImmutableArrayOwnerType->canAutoCastTo(mContext, PrimitiveTypes::STRING_TYPE));
  EXPECT_TRUE(mImmutableArrayOwnerType->canAutoCastTo(mContext, mImmutableArrayOwnerType));
  EXPECT_TRUE(mImmutableArrayOwnerType->canAutoCastTo(mContext, mImmutableArrayType));
  EXPECT_FALSE(mImmutableArrayOwnerType->canAutoCastTo(mContext, mArrayType));
}

TEST_F(ImmutableArrayOwnerTypeTest, isTypeKindTest) {
  EXPECT_FALSE(mImmutableArrayOwnerType->isPrimitive());
  EXPECT_TRUE(mImmutableArrayOwnerType->isOwner());
  EXPECT_FALSE(mImmutableArrayOwnerType->isReference());
  EXPECT_TRUE(mImmutableArrayOwnerType->isArray());
  EXPECT_FALSE(mImmutableArrayOwnerType->isFunction());
  EXPECT_FALSE(mImmutableArrayOwnerType->isPackage());
  EXPECT_FALSE(mImmutableArrayOwnerType->isNative());
  EXPECT_FALSE(mImmutableArrayOwnerType->isPointer());
  EXPECT_TRUE(mImmutableArrayOwnerType->isImmutable());
}

TEST_F(ImmutableArrayOwnerTypeTest, isObjectTest) {
  EXPECT_FALSE(mImmutableArrayOwnerType->isController());
  EXPECT_FALSE(mImmutableArrayOwnerType->isInterface());
  EXPECT_FALSE(mImmutableArrayOwnerType->isModel());
  EXPECT_FALSE(mImmutableArrayOwnerType->isNode());
  EXPECT_FALSE(mImmutableArrayOwnerType->isThread());
}

TEST_F(ImmutableArrayOwnerTypeTest, createLocalVariableTest) {
  mImmutableArrayOwnerType->createLocalVariable(mContext, "temp");
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

TEST_F(ImmutableArrayOwnerTypeTest, injectDeathTest) {
  InjectionArgumentList arguments;
  EXPECT_EXIT(mImmutableArrayOwnerType->inject(mContext, arguments, 3),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(3\\): Error: type immutable long\\[\\]\\* is not injectable");
}
