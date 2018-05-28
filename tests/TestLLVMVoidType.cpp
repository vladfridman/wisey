//
//  TestLLVMVoidType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMVoidType}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "TestPrefix.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMVoidType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LLVMVoidTypeTest : public Test {
  
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  LLVMVoidType mLLVMVoidType;
  
  LLVMVoidTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    BasicBlock* declareBlock = BasicBlock::Create(mLLVMContext, "declare", function);
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setDeclarationsBlock(declareBlock);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
  }
};

TEST_F(LLVMVoidTypeTest, getTypeNameTest) {
  ASSERT_STREQ("::llvm::void", mLLVMVoidType.getTypeName().c_str());
}

TEST_F(LLVMVoidTypeTest, getLLVMTypeTest) {
  ASSERT_EQ(Type::getVoidTy(mLLVMContext), mLLVMVoidType.getLLVMType(mContext));
}

TEST_F(LLVMVoidTypeTest, isTypeKindTest) {
  EXPECT_FALSE(mLLVMVoidType.isPrimitive());
  EXPECT_FALSE(mLLVMVoidType.isOwner());
  EXPECT_FALSE(mLLVMVoidType.isReference());
  EXPECT_FALSE(mLLVMVoidType.isArray());
  EXPECT_FALSE(mLLVMVoidType.isFunction());
  EXPECT_FALSE(mLLVMVoidType.isPackage());
  EXPECT_TRUE(mLLVMVoidType.isNative());
  EXPECT_FALSE(mLLVMVoidType.isPointer());
  EXPECT_FALSE(mLLVMVoidType.isImmutable());
}

TEST_F(LLVMVoidTypeTest, isObjectTest) {
  EXPECT_FALSE(mLLVMVoidType.isController());
  EXPECT_FALSE(mLLVMVoidType.isInterface());
  EXPECT_FALSE(mLLVMVoidType.isModel());
  EXPECT_FALSE(mLLVMVoidType.isNode());
}

TEST_F(LLVMVoidTypeTest, printToStreamTest) {
  stringstream stringStream;
  mLLVMVoidType.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::llvm::void", stringStream.str().c_str());
}

TEST_F(LLVMVoidTypeTest, injectDeathTest) {
  InjectionArgumentList arguments;
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mLLVMVoidType.inject(mContext, arguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: type ::llvm::void is not injectable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}
