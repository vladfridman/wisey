//
//  TestNativeType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/8/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link NativeType}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/NativeType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct NativeTypeTest : public Test {
  
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NativeType* mNativeType;
  BasicBlock* mBasicBlock;
  
  NativeTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    mNativeType = new NativeType(Type::getInt8Ty(mLLVMContext)->getPointerTo());

    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                                      GlobalValue::InternalLinkage,
                                                      "main",
                                                      mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
}
};

TEST_F(NativeTypeTest, getTypeNameTest) {
  ASSERT_STREQ("native", mNativeType->getTypeName().c_str());
}

TEST_F(NativeTypeTest, getLLVMTypeTest) {
  ASSERT_EQ(Type::getInt8Ty(mLLVMContext)->getPointerTo(), mNativeType->getLLVMType(mContext));
}

TEST_F(NativeTypeTest, isTypeKindTest) {
  EXPECT_FALSE(mNativeType->isPrimitive());
  EXPECT_FALSE(mNativeType->isOwner());
  EXPECT_TRUE(mNativeType->isReference());
  EXPECT_FALSE(mNativeType->isArray());
  EXPECT_FALSE(mNativeType->isFunction());
  EXPECT_FALSE(mNativeType->isPackage());
  EXPECT_TRUE(mNativeType->isNative());
}

TEST_F(NativeTypeTest, isObjectTest) {
  EXPECT_FALSE(mNativeType->isController());
  EXPECT_FALSE(mNativeType->isInterface());
  EXPECT_FALSE(mNativeType->isModel());
  EXPECT_FALSE(mNativeType->isNode());
  EXPECT_FALSE(mNativeType->isThread());
}

TEST_F(NativeTypeTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  IField* field = new FixedField(mNativeType, "mField");
  ON_CALL(concreteObjectType, findField(_)).WillByDefault(Return(field));
  mNativeType->createFieldVariable(mContext, "mField", &concreteObjectType);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(NativeTypeTest, canCastToTest) {
  NativeType* anotherType = new NativeType(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  
  EXPECT_TRUE(mNativeType->canCastTo(mContext, anotherType));
}

TEST_F(NativeTypeTest, canAutoCastToTest) {
  NativeType* anotherType = new NativeType(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  
  EXPECT_TRUE(mNativeType->canAutoCastTo(mContext, anotherType));
}

TEST_F(NativeTypeTest, castToTest) {
  llvm::PointerType* int8PointerType = Type::getInt8Ty(mLLVMContext)->getPointerTo();
  NativeType* anotherType = new NativeType(int8PointerType);
  Value* null = ConstantPointerNull::get(int8PointerType);
  
  EXPECT_EQ(null, mNativeType->castTo(mContext, null, anotherType, 0));
}

TEST_F(NativeTypeTest, isReferenceTest) {
  NativeType* anotherType = new NativeType(Type::getInt8Ty(mLLVMContext));

  EXPECT_TRUE(mNativeType->isReference());
  EXPECT_FALSE(anotherType->isReference());
}

TEST_F(NativeTypeTest, getPointerTypeTest) {
  const IType* pointerType = mNativeType->getPointerType();
  EXPECT_EQ(Type::getInt8Ty(mLLVMContext)->getPointerTo()->getPointerTo(),
            pointerType->getLLVMType(mContext));
}
