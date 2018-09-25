//
//  TestLLVMStructDefinition.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/17/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMStructDefinition}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileRunner.hpp"
#include "IRGenerationContext.hpp"
#include "LLVMPrimitiveTypes.hpp"
#include "LLVMStructDefinition.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LLVMStructDefinitionTest : public Test {
  
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  LLVMStructDefinition* mLLVMStructDefinition;
  BasicBlock* mBasicBlock;
  
  LLVMStructDefinitionTest() : mLLVMContext(mContext.getLLVMContext()) {
    vector<const ITypeSpecifier*> typeSpecifiers;
    typeSpecifiers.push_back(LLVMPrimitiveTypes::I8->newTypeSpecifier(0));
    typeSpecifiers.push_back(LLVMPrimitiveTypes::I64->newTypeSpecifier(0));
    mLLVMStructDefinition = new LLVMStructDefinition("mystruct", typeSpecifiers, 0);
    
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

TEST_F(LLVMStructDefinitionTest, prototypeObjectTest) {
  mLLVMStructDefinition->prototypeObject(mContext, mContext.getImportProfile());
  
  LLVMStructType* llvmStructType = mContext.getLLVMStructType("mystruct", 0);
  
  ASSERT_NE(nullptr, llvmStructType);
  EXPECT_FALSE(llvmStructType->isExternal());

  EXPECT_STREQ(llvmStructType->getTypeName().c_str(), "::llvm::struct::mystruct");
  EXPECT_EQ(0u, llvmStructType->getLLVMType(mContext)->getNumElements());
}

TEST_F(LLVMStructDefinitionTest, prototypeMethodsTest) {
  mLLVMStructDefinition->prototypeObject(mContext, mContext.getImportProfile());
  mLLVMStructDefinition->prototypeMethods(mContext);

  LLVMStructType* llvmStructType = mContext.getLLVMStructType("mystruct", 0);
  
  ASSERT_NE(nullptr, llvmStructType);
  EXPECT_FALSE(llvmStructType->isExternal());
  EXPECT_STREQ(llvmStructType->getTypeName().c_str(), "::llvm::struct::mystruct");
  EXPECT_EQ(2u, llvmStructType->getLLVMType(mContext)->getNumElements());
  EXPECT_EQ(Type::getInt8Ty(mLLVMContext),
            llvmStructType->getLLVMType(mContext)->getElementType(0));
  EXPECT_EQ(Type::getInt64Ty(mLLVMContext),
            llvmStructType->getLLVMType(mContext)->getElementType(1));
}

TEST_F(TestFileRunner, llvmStructDefinitionTest) {
  compileFile("tests/samples/test_llvm_struct_definition.yz");
}
