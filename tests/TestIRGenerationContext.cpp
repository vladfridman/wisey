//
//  TestIRGenerationContext.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/15/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IRGenerationContext}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/node.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

TEST(IRGenerationContextTest, TestBlockStack) {
  IRGenerationContext context;
  LLVMContext &llvmContext = context.getLLVMContext();
  BasicBlock* block1 = BasicBlock::Create(llvmContext, "block1");
  BasicBlock* block2 = BasicBlock::Create(llvmContext, "block2");
  
  EXPECT_EQ(context.currentBlock() == NULL, true);
  context.pushBlock(block1);
  EXPECT_EQ(context.currentBlock(), block1);
  context.pushBlock(block2);
  EXPECT_EQ(context.currentBlock(), block2);
  context.popBlock();
  EXPECT_EQ(context.currentBlock(), block1);
  context.replaceBlock(block2);
  EXPECT_EQ(context.currentBlock(), block2);
  context.popBlock();
  EXPECT_EQ(context.currentBlock() == NULL, true);
}

TEST(IRGenerationTest, TestMainFunction) {
  IRGenerationContext context;
  FunctionType* functionType = FunctionType::get(Type::getInt32Ty(context.getLLVMContext()), false);
  Function* function = Function::Create(functionType, GlobalValue::InternalLinkage, "main");

  EXPECT_EQ(context.getMainFunction() == NULL, true);
  context.setMainFunction(function);
  EXPECT_EQ(context.getMainFunction(), function);
}

TEST(IRGenerationTest, TestLocalVariables) {
  IRGenerationContext context;
  LLVMContext &llvmContext = context.getLLVMContext();
  BasicBlock* block1 = BasicBlock::Create(llvmContext, "block1");
  BasicBlock* block2 = BasicBlock::Create(llvmContext, "block2");
  Value* fooValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 3);
  Value* barValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 5);

  context.pushBlock(block1);
  context.locals()["foo"] = fooValue;
  context.pushBlock(block2);
  context.locals()["bar"] = barValue;
  
  EXPECT_EQ(context.locals()["bar"], barValue);
  EXPECT_EQ(context.locals()["foo"] == NULL, true);

  context.popBlock();
  EXPECT_EQ(context.locals()["foo"], fooValue);
  EXPECT_EQ(context.locals()["bar"] == NULL, true);
  
  context.replaceBlock(block2);
  context.locals()["bar"] = barValue;
  EXPECT_EQ(context.locals()["foo"], fooValue);
  EXPECT_EQ(context.locals()["bar"], barValue);
}

TEST(IRGenerationContextTest, TestModuleIsNotNull) {
  IRGenerationContext context;
  
  EXPECT_EQ(context.getModule() == NULL, false);
}

TEST(IRGenerationContextTest, RunCodeFailsWhenMainIsNullDeathTest) {
  IRGenerationContext context;
  
  EXPECT_EXIT(context.runCode(),
              ::testing::ExitedWithCode(1),
              "Function main is not defined. Exiting.");
}

struct IRGenerationContextRunTest : public ::testing::Test {
  IRGenerationContext mContext;
  
public:
  
  IRGenerationContextRunTest() {
    LLVMContext &llvmContext = mContext.getLLVMContext();
    FunctionType* functionType =
      FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mContext.setMainFunction(function);
    BasicBlock* block = BasicBlock::Create(llvmContext, "entry", function);
    mContext.pushBlock(block);
    Value* returnValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 5);
    ReturnInst::Create(llvmContext, returnValue, block);
  }
  
  ~IRGenerationContextRunTest() {
  }
};

TEST_F(IRGenerationContextRunTest, RunCodeTest) {
  GenericValue result = mContext.runCode();
  
  EXPECT_EQ(result.IntVal.toString(10, true), "5");
}

TEST_F(IRGenerationContextRunTest, TestPrintAssembly) {
  string mStringBuffer;
  raw_string_ostream mStringStream(mStringBuffer);
  
  mContext.printAssembly(mStringStream);
  string expected = string() +
    "; ModuleID = 'yazyk'\n" +
    "source_filename = \"yazyk\"\n" +
    "\n" +
    "define internal i32 @main() {\n" +
    "entry:\n" +
    "  ret i32 5\n" +
    "}\n";
  
  EXPECT_STREQ(expected.c_str(), mStringBuffer.c_str());
}