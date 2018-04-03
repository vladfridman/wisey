//
//  TestLLVMi1Type.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/18/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMi1Type}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMi1Type.hpp"
#include "wisey/LLVMi8Type.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LLVMi1TypeTest : public Test {
  
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  LLVMi1Type mLLVMi1Type;

  LLVMi1TypeTest() : mLLVMContext(mContext.getLLVMContext()) {
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

TEST_F(LLVMi1TypeTest, getTypeNameTest) {
  ASSERT_STREQ("::llvm::i1", mLLVMi1Type.getTypeName().c_str());
}

TEST_F(LLVMi1TypeTest, getLLVMTypeTest) {
  ASSERT_EQ(Type::getInt1Ty(mLLVMContext), mLLVMi1Type.getLLVMType(mContext));
}

TEST_F(LLVMi1TypeTest, isTypeKindTest) {
  EXPECT_FALSE(mLLVMi1Type.isPrimitive());
  EXPECT_FALSE(mLLVMi1Type.isOwner());
  EXPECT_FALSE(mLLVMi1Type.isReference());
  EXPECT_FALSE(mLLVMi1Type.isArray());
  EXPECT_FALSE(mLLVMi1Type.isFunction());
  EXPECT_FALSE(mLLVMi1Type.isPackage());
  EXPECT_TRUE(mLLVMi1Type.isNative());
  EXPECT_FALSE(mLLVMi1Type.isPointer());
}

TEST_F(LLVMi1TypeTest, isObjectTest) {
  EXPECT_FALSE(mLLVMi1Type.isController());
  EXPECT_FALSE(mLLVMi1Type.isInterface());
  EXPECT_FALSE(mLLVMi1Type.isModel());
  EXPECT_FALSE(mLLVMi1Type.isNode());
  EXPECT_FALSE(mLLVMi1Type.isThread());
}

TEST_F(LLVMi1TypeTest, canCastToTest) {
  EXPECT_TRUE(mLLVMi1Type.canCastTo(mContext, &mLLVMi1Type));
  LLVMi8Type i8Type;
  EXPECT_FALSE(mLLVMi1Type.canCastTo(mContext, &i8Type));
  EXPECT_TRUE(mLLVMi1Type.canCastTo(mContext, PrimitiveTypes::BOOLEAN_TYPE));
}

TEST_F(LLVMi1TypeTest, canAutoCastToTest) {
  EXPECT_TRUE(mLLVMi1Type.canAutoCastTo(mContext, &mLLVMi1Type));
  LLVMi8Type i8Type;
  EXPECT_FALSE(mLLVMi1Type.canAutoCastTo(mContext, &i8Type));
  EXPECT_TRUE(mLLVMi1Type.canAutoCastTo(mContext, PrimitiveTypes::BOOLEAN_TYPE));
}

TEST_F(LLVMi1TypeTest, castToTest) {
  Value* value = ConstantInt::get(Type::getInt1Ty(mLLVMContext), 1);
  EXPECT_EQ(value, mLLVMi1Type.castTo(mContext, value, PrimitiveTypes::BOOLEAN_TYPE, 0));
}

TEST_F(LLVMi1TypeTest, getPointerTypeTest) {
  const IType* pointerType = mLLVMi1Type.getPointerType();
  EXPECT_EQ(Type::getInt1Ty(mLLVMContext)->getPointerTo(), pointerType->getLLVMType(mContext));
}

TEST_F(LLVMi1TypeTest, printToStreamTest) {
  stringstream stringStream;
  mLLVMi1Type.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::llvm::i1", stringStream.str().c_str());
}
