//
//  TestLLVMFunctionDeclaration.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMFunctionDeclaration}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileRunner.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMFunctionDeclaration.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LLVMFunctionDeclarationTest : public Test {
  
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  LLVMFunctionDeclaration* mLLVMFunctionDeclaration;
  BasicBlock* mBasicBlock;
  
  LLVMFunctionDeclarationTest() : mLLVMContext(mContext.getLLVMContext()) {
    vector<const ITypeSpecifier*> argumentSpecifiers;
    argumentSpecifiers.push_back(LLVMPrimitiveTypes::I8->newTypeSpecifier());
    argumentSpecifiers.push_back(LLVMPrimitiveTypes::I64->newTypeSpecifier());
    const ITypeSpecifier* returnSpecifier = LLVMPrimitiveTypes::VOID->newTypeSpecifier();
    mLLVMFunctionDeclaration = new LLVMFunctionDeclaration("myfunction",
                                                           PUBLIC_ACCESS,
                                                           returnSpecifier,
                                                           argumentSpecifiers);
    
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

TEST_F(LLVMFunctionDeclarationTest, prototypeMethodsTest) {
  mLLVMFunctionDeclaration->prototypeMethods(mContext);
  
  Function* function = mContext.getModule()->getFunction("myfunction");
  
  ASSERT_NE(nullptr, function);
  
  vector<Type*> argumentTypes;
  argumentTypes.push_back(Type::getInt8Ty(mLLVMContext));
  argumentTypes.push_back(Type::getInt64Ty(mLLVMContext));
  FunctionType* expected = FunctionType::get(Type::getVoidTy(mLLVMContext), argumentTypes, false);
  
  EXPECT_EQ(expected, function->getFunctionType());
}

TEST_F(TestFileRunner, LLVMFunctionDeclarationTest) {
  compileFile("tests/samples/test_llvm_function_declaration.yz");
}
