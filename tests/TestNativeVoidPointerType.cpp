//
//  TestNativeVoidPointerType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/13/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link NativeVoidPointerType}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/NativeVoidPointerType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct NativeVoidPointerTypeTest : public Test {
  
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  string mPackage = "systems.vos.wisey.compiler.tests";
  NativeVoidPointerType* mNativeVoidPointerType;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;

  NativeVoidPointerTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    mNativeVoidPointerType = new NativeVoidPointerType();
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
};

TEST_F(NativeVoidPointerTypeTest, getTypeNameTest) {
  ASSERT_STREQ("native:i8*", mNativeVoidPointerType->getTypeName().c_str());
}

TEST_F(NativeVoidPointerTypeTest, getLLVMTypeTest) {
  ASSERT_EQ(Type::getInt8Ty(mLLVMContext)->getPointerTo(),
            mNativeVoidPointerType->getLLVMType(mContext));
}

TEST_F(NativeVoidPointerTypeTest, isPrimitiveTest) {
  EXPECT_FALSE(mNativeVoidPointerType->isPrimitive());
}

TEST_F(NativeVoidPointerTypeTest, isOwnerTest) {
  EXPECT_FALSE(mNativeVoidPointerType->isOwner());
}

TEST_F(NativeVoidPointerTypeTest, isReferenceTest) {
  EXPECT_TRUE(mNativeVoidPointerType->isReference());
}

TEST_F(NativeVoidPointerTypeTest, isArrayTest) {
  EXPECT_FALSE(mNativeVoidPointerType->isArray());
}

TEST_F(NativeVoidPointerTypeTest, isFunctionTest) {
  EXPECT_FALSE(mNativeVoidPointerType->isFunction());
}

TEST_F(NativeVoidPointerTypeTest, isPackageTest) {
  EXPECT_FALSE(mNativeVoidPointerType->isPackage());
}

TEST_F(NativeVoidPointerTypeTest, isObjectTest) {
  EXPECT_FALSE(mNativeVoidPointerType->isController());
  EXPECT_FALSE(mNativeVoidPointerType->isInterface());
  EXPECT_FALSE(mNativeVoidPointerType->isModel());
  EXPECT_FALSE(mNativeVoidPointerType->isNode());
  EXPECT_TRUE(mNativeVoidPointerType->isNative());
}

TEST_F(NativeVoidPointerTypeTest, createLocalVariableTest) {
  mNativeVoidPointerType->createLocalVariable(mContext, "variable");
  IVariable* variable = mContext.getScopes().getVariable("variable");
  
  EXPECT_NE(variable, nullptr);
  EXPECT_EQ(mNativeVoidPointerType, variable->getType());

  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %nativeVariable = alloca i8*\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

