//
//  TestLLVMi16Type.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/18/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMi16Type}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMi16Type.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LLVMi16TypeTest : public Test {
  
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  LLVMi16Type mLLVMi16Type;
  
  LLVMi16TypeTest() : mLLVMContext(mContext.getLLVMContext()) {
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

TEST_F(LLVMi16TypeTest, getTypeNameTest) {
  ASSERT_STREQ("::llvm::i16", mLLVMi16Type.getTypeName().c_str());
}

TEST_F(LLVMi16TypeTest, getLLVMTypeTest) {
  ASSERT_EQ(Type::getInt16Ty(mLLVMContext), mLLVMi16Type.getLLVMType(mContext));
}

TEST_F(LLVMi16TypeTest, isTypeKindTest) {
  EXPECT_FALSE(mLLVMi16Type.isPrimitive());
  EXPECT_FALSE(mLLVMi16Type.isOwner());
  EXPECT_FALSE(mLLVMi16Type.isReference());
  EXPECT_FALSE(mLLVMi16Type.isArray());
  EXPECT_FALSE(mLLVMi16Type.isFunction());
  EXPECT_FALSE(mLLVMi16Type.isPackage());
  EXPECT_TRUE(mLLVMi16Type.isNative());
}

TEST_F(LLVMi16TypeTest, isObjectTest) {
  EXPECT_FALSE(mLLVMi16Type.isController());
  EXPECT_FALSE(mLLVMi16Type.isInterface());
  EXPECT_FALSE(mLLVMi16Type.isModel());
  EXPECT_FALSE(mLLVMi16Type.isNode());
  EXPECT_FALSE(mLLVMi16Type.isThread());
}

TEST_F(LLVMi16TypeTest, getPointerTypeTest) {
  const IType* pointerType = mLLVMi16Type.getPointerType();
  EXPECT_EQ(Type::getInt16Ty(mLLVMContext)->getPointerTo(), pointerType->getLLVMType(mContext));
}

