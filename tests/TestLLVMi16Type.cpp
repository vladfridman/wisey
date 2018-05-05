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
#include "TestPrefix.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMi16Type.hpp"
#include "wisey/LLVMi8Type.hpp"
#include "wisey/PrimitiveTypes.hpp"

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
    TestPrefix::generateIR(mContext);
    
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
  EXPECT_FALSE(mLLVMi16Type.isPointer());
  EXPECT_FALSE(mLLVMi16Type.isImmutable());
}

TEST_F(LLVMi16TypeTest, isObjectTest) {
  EXPECT_FALSE(mLLVMi16Type.isController());
  EXPECT_FALSE(mLLVMi16Type.isInterface());
  EXPECT_FALSE(mLLVMi16Type.isModel());
  EXPECT_FALSE(mLLVMi16Type.isNode());
}

TEST_F(LLVMi16TypeTest, canCastToTest) {
  EXPECT_TRUE(mLLVMi16Type.canCastTo(mContext, &mLLVMi16Type));
  LLVMi8Type i8Type;
  EXPECT_FALSE(mLLVMi16Type.canCastTo(mContext, &i8Type));
  EXPECT_TRUE(mLLVMi16Type.canCastTo(mContext, PrimitiveTypes::CHAR));
}

TEST_F(LLVMi16TypeTest, canAutoCastToTest) {
  EXPECT_TRUE(mLLVMi16Type.canAutoCastTo(mContext, &mLLVMi16Type));
  LLVMi8Type i8Type;
  EXPECT_FALSE(mLLVMi16Type.canAutoCastTo(mContext, &i8Type));
  EXPECT_TRUE(mLLVMi16Type.canAutoCastTo(mContext, PrimitiveTypes::CHAR));
}

TEST_F(LLVMi16TypeTest, castToTest) {
  Value* value = ConstantInt::get(Type::getInt16Ty(mLLVMContext), 5);
  EXPECT_EQ(value, mLLVMi16Type.castTo(mContext, value, PrimitiveTypes::CHAR, 0));
}

TEST_F(LLVMi16TypeTest, getPointerTypeTest) {
  const IType* pointerType = mLLVMi16Type.getPointerType(mContext, 0);
  EXPECT_EQ(Type::getInt16Ty(mLLVMContext)->getPointerTo(), pointerType->getLLVMType(mContext));
}

TEST_F(LLVMi16TypeTest, injectDeathTest) {
  InjectionArgumentList arguments;
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mLLVMi16Type.inject(mContext, arguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: type ::llvm::i16 is not injectable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}
