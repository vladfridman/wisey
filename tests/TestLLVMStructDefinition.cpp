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

#include "TestFileSampleRunner.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/LLVMStructDefinition.hpp"

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
  string mPackage = "systems.vos.wisey.compiler.tests";
  LLVMStructDefinition* mLLVMStructDefinition;
  BasicBlock* mBasicBlock;
  
  LLVMStructDefinitionTest() : mLLVMContext(mContext.getLLVMContext()) {
    vector<const ILLVMTypeSpecifier*> typeSpecifiers;
    typeSpecifiers.push_back(LLVMPrimitiveTypes::I8->newTypeSpecifier());
    typeSpecifiers.push_back(LLVMPrimitiveTypes::I64->newTypeSpecifier());
    mLLVMStructDefinition = new LLVMStructDefinition("mystruct", typeSpecifiers);
    
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

TEST_F(LLVMStructDefinitionTest, prototypeObjectTest) {
  mLLVMStructDefinition->prototypeObject(mContext);
  
  LLVMStructType* llvmStructType = mContext.getLLVMStructType("mystruct");
  
  ASSERT_NE(nullptr, llvmStructType);
  
  EXPECT_STREQ(llvmStructType->getTypeName().c_str(), "mystruct");
  EXPECT_EQ(0u, llvmStructType->getLLVMType(mContext)->getNumElements());
}

TEST_F(LLVMStructDefinitionTest, prototypeMethodsTest) {
  mLLVMStructDefinition->prototypeObject(mContext);
  mLLVMStructDefinition->prototypeMethods(mContext);

  LLVMStructType* llvmStructType = mContext.getLLVMStructType("mystruct");
  
  ASSERT_NE(nullptr, llvmStructType);
  EXPECT_STREQ(llvmStructType->getTypeName().c_str(), "mystruct");
  EXPECT_EQ(2u, llvmStructType->getLLVMType(mContext)->getNumElements());
  EXPECT_EQ(Type::getInt8Ty(mLLVMContext),
            llvmStructType->getLLVMType(mContext)->getElementType(0));
  EXPECT_EQ(Type::getInt64Ty(mLLVMContext),
            llvmStructType->getLLVMType(mContext)->getElementType(1));
}

TEST_F(TestFileSampleRunner, llvmStructDefinitionTest) {
  compileFile("tests/samples/llvm_struct_definition.yz");
}
