//
//  TestLLVMi32Type.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/18/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMi32Type}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMi32Type.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LLVMi32TypeTest : public Test {
  
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  LLVMi32Type mLLVMi32Type;
  
  LLVMi32TypeTest() : mLLVMContext(mContext.getLLVMContext()) {
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

TEST_F(LLVMi32TypeTest, getTypeNameTest) {
  ASSERT_STREQ("::llvm::i32", mLLVMi32Type.getTypeName().c_str());
}

TEST_F(LLVMi32TypeTest, getLLVMTypeTest) {
  ASSERT_EQ(Type::getInt32Ty(mLLVMContext), mLLVMi32Type.getLLVMType(mContext));
}

TEST_F(LLVMi32TypeTest, isTypeKindTest) {
  EXPECT_FALSE(mLLVMi32Type.isPrimitive());
  EXPECT_FALSE(mLLVMi32Type.isOwner());
  EXPECT_FALSE(mLLVMi32Type.isReference());
  EXPECT_FALSE(mLLVMi32Type.isArray());
  EXPECT_FALSE(mLLVMi32Type.isFunction());
  EXPECT_FALSE(mLLVMi32Type.isPackage());
  EXPECT_TRUE(mLLVMi32Type.isNative());
}

TEST_F(LLVMi32TypeTest, isObjectTest) {
  EXPECT_FALSE(mLLVMi32Type.isController());
  EXPECT_FALSE(mLLVMi32Type.isInterface());
  EXPECT_FALSE(mLLVMi32Type.isModel());
  EXPECT_FALSE(mLLVMi32Type.isNode());
  EXPECT_FALSE(mLLVMi32Type.isThread());
}

TEST_F(LLVMi32TypeTest, getPointerTypeTest) {
  const IType* pointerType = mLLVMi32Type.getPointerType();
  EXPECT_EQ(Type::getInt32Ty(mLLVMContext)->getPointerTo(), pointerType->getLLVMType(mContext));
}

TEST_F(LLVMi32TypeTest, printToStreamTest) {
  stringstream stringStream;
  mLLVMi32Type.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::llvm::i32", stringStream.str().c_str());
}