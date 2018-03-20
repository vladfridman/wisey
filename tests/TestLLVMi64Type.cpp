//
//  TestLLVMi64Type.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/18/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMi64Type}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMi64Type.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LLVMi64TypeTest : public Test {
  
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  LLVMi64Type mLLVMi64Type;
  
  LLVMi64TypeTest() : mLLVMContext(mContext.getLLVMContext()) {
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

TEST_F(LLVMi64TypeTest, getTypeNameTest) {
  ASSERT_STREQ("::llvm::i64", mLLVMi64Type.getTypeName().c_str());
}

TEST_F(LLVMi64TypeTest, getLLVMTypeTest) {
  ASSERT_EQ(Type::getInt64Ty(mLLVMContext), mLLVMi64Type.getLLVMType(mContext));
}

TEST_F(LLVMi64TypeTest, isTypeKindTest) {
  EXPECT_FALSE(mLLVMi64Type.isPrimitive());
  EXPECT_FALSE(mLLVMi64Type.isOwner());
  EXPECT_FALSE(mLLVMi64Type.isReference());
  EXPECT_FALSE(mLLVMi64Type.isArray());
  EXPECT_FALSE(mLLVMi64Type.isFunction());
  EXPECT_FALSE(mLLVMi64Type.isPackage());
  EXPECT_TRUE(mLLVMi64Type.isNative());
}

TEST_F(LLVMi64TypeTest, isObjectTest) {
  EXPECT_FALSE(mLLVMi64Type.isController());
  EXPECT_FALSE(mLLVMi64Type.isInterface());
  EXPECT_FALSE(mLLVMi64Type.isModel());
  EXPECT_FALSE(mLLVMi64Type.isNode());
  EXPECT_FALSE(mLLVMi64Type.isThread());
}

TEST_F(LLVMi64TypeTest, getPointerTypeTest) {
  const IType* pointerType = mLLVMi64Type.getPointerType();
  EXPECT_EQ(Type::getInt64Ty(mLLVMContext)->getPointerTo(), pointerType->getLLVMType(mContext));
}

TEST_F(LLVMi64TypeTest, printToStreamTest) {
  stringstream stringStream;
  mLLVMi64Type.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::llvm::i64", stringStream.str().c_str());
}