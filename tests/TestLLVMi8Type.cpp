//
//  TestLLVMi8Type.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/18/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMi8Type}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMi8Type.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LLVMi8TypeTest : public Test {
  
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  LLVMi8Type mLLVMi8Type;
  
  LLVMi8TypeTest() : mLLVMContext(mContext.getLLVMContext()) {
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

TEST_F(LLVMi8TypeTest, getTypeNameTest) {
  ASSERT_STREQ("::llvm::i8", mLLVMi8Type.getTypeName().c_str());
}

TEST_F(LLVMi8TypeTest, getLLVMTypeTest) {
  ASSERT_EQ(Type::getInt8Ty(mLLVMContext), mLLVMi8Type.getLLVMType(mContext));
}

TEST_F(LLVMi8TypeTest, isTypeKindTest) {
  EXPECT_FALSE(mLLVMi8Type.isPrimitive());
  EXPECT_FALSE(mLLVMi8Type.isOwner());
  EXPECT_FALSE(mLLVMi8Type.isReference());
  EXPECT_FALSE(mLLVMi8Type.isArray());
  EXPECT_FALSE(mLLVMi8Type.isFunction());
  EXPECT_FALSE(mLLVMi8Type.isPackage());
  EXPECT_TRUE(mLLVMi8Type.isNative());
}

TEST_F(LLVMi8TypeTest, isObjectTest) {
  EXPECT_FALSE(mLLVMi8Type.isController());
  EXPECT_FALSE(mLLVMi8Type.isInterface());
  EXPECT_FALSE(mLLVMi8Type.isModel());
  EXPECT_FALSE(mLLVMi8Type.isNode());
  EXPECT_FALSE(mLLVMi8Type.isThread());
}

TEST_F(LLVMi8TypeTest, getPointerTypeTest) {
  const IType* pointerType = mLLVMi8Type.getPointerType();
  EXPECT_EQ(Type::getInt8Ty(mLLVMContext)->getPointerTo(), pointerType->getLLVMType(mContext));
}

TEST_F(LLVMi8TypeTest, printToStreamTest) {
  stringstream stringStream;
  mLLVMi8Type.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::llvm::i8", stringStream.str().c_str());
}
