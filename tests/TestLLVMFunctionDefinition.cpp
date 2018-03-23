//
//  TestLLVMFunctionDefinition.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMFunctionDefinition}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMFunctionDefinition.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LLVMFunctionDefinitionTest : public Test {
  
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  LLVMFunctionDefinition* mLLVMFunctionDefinition;
  BasicBlock* mBasicBlock;
  
  LLVMFunctionDefinitionTest() : mLLVMContext(mContext.getLLVMContext()) {
    LLVMVariableList arguments;
    LLVMVariableDeclaration* argument =
    LLVMVariableDeclaration::create(LLVMPrimitiveTypes::I8->newTypeSpecifier(),
                                    new Identifier("input"),
                                    0);
    arguments.push_back(argument);
    const ILLVMTypeSpecifier* returnSpecifier = LLVMPrimitiveTypes::VOID->newTypeSpecifier();
    Block* block = new Block();
    CompoundStatement* compoundStatement = new CompoundStatement(block, 0);
    mLLVMFunctionDefinition = new LLVMFunctionDefinition("myfunction",
                                                         returnSpecifier,
                                                         arguments,
                                                         compoundStatement,
                                                         0);

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

TEST_F(LLVMFunctionDefinitionTest, objectElementTypeTest) {
  EXPECT_FALSE(mLLVMFunctionDefinition->isConstant());
  EXPECT_FALSE(mLLVMFunctionDefinition->isField());
  EXPECT_FALSE(mLLVMFunctionDefinition->isMethod());
  EXPECT_FALSE(mLLVMFunctionDefinition->isStaticMethod());
  EXPECT_FALSE(mLLVMFunctionDefinition->isMethodSignature());
  EXPECT_TRUE(mLLVMFunctionDefinition->isLLVMFunction());
}

TEST_F(LLVMFunctionDefinitionTest, defineTest) {
  LLVMFunction* llvmFunction = mLLVMFunctionDefinition->define(mContext, NULL);
  
  EXPECT_FALSE(llvmFunction->isConstant());
  EXPECT_FALSE(llvmFunction->isField());
  EXPECT_FALSE(llvmFunction->isMethod());
  EXPECT_FALSE(llvmFunction->isStaticMethod());
  EXPECT_FALSE(llvmFunction->isMethodSignature());
  EXPECT_TRUE(llvmFunction->isLLVMFunction());
}

TEST_F(TestFileSampleRunner, llvmFunctionDefinitionTest) {
  compileFile("tests/samples/test_llvm_function_definition.yz");
}
