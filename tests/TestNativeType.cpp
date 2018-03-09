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
  string mPackage = "systems.vos.wisey.compiler.tests";
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

TEST_F(NativeTypeTest, isPrimitiveTest) {
  EXPECT_FALSE(mNativeType->isPrimitive());
}

TEST_F(NativeTypeTest, isOwnerTest) {
  EXPECT_FALSE(mNativeType->isOwner());
}

TEST_F(NativeTypeTest, isReferenceTest) {
  EXPECT_FALSE(mNativeType->isReference());
}

TEST_F(NativeTypeTest, isArrayTest) {
  EXPECT_FALSE(mNativeType->isArray());
}

TEST_F(NativeTypeTest, isFunctionTest) {
  EXPECT_FALSE(mNativeType->isFunction());
}

TEST_F(NativeTypeTest, isPackageTest) {
  EXPECT_FALSE(mNativeType->isPackage());
}

TEST_F(NativeTypeTest, isObjectTest) {
  EXPECT_FALSE(mNativeType->isController());
  EXPECT_FALSE(mNativeType->isInterface());
  EXPECT_FALSE(mNativeType->isModel());
  EXPECT_FALSE(mNativeType->isNode());
  EXPECT_TRUE(mNativeType->isNative());
}

TEST_F(NativeTypeTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  IField* field = new FixedField(mNativeType, "mField");
  ON_CALL(concreteObjectType, findField(_)).WillByDefault(Return(field));
  mNativeType->createFieldVariable(mContext, "mField", &concreteObjectType);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}
