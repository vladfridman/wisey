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

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

TEST(IRGenerationContextTest, TestBlockStack) {
  IRGenerationContext context;
  LLVMContext &llvmContext = context.getLLVMContext();
  BasicBlock* block1 = BasicBlock::Create(llvmContext, "block1");
  BasicBlock* block2 = BasicBlock::Create(llvmContext, "block2");
  
  EXPECT_EQ(context.getBasicBlock() == NULL, true);
  context.setBasicBlock(block1);
  EXPECT_EQ(context.getBasicBlock(), block1);
  context.setBasicBlock(block2);
  EXPECT_EQ(context.getBasicBlock(), block2);
}

TEST(IRGenerationContextTest, TestMainFunction) {
  IRGenerationContext context;
  FunctionType* functionType = FunctionType::get(Type::getInt32Ty(context.getLLVMContext()), false);
  Function* function = Function::Create(functionType, GlobalValue::InternalLinkage, "main");

  EXPECT_EQ(context.getMainFunction() == NULL, true);
  context.setMainFunction(function);
  EXPECT_EQ(context.getMainFunction(), function);
}

TEST(IRGenerationContextTest, TestScopes) {
  IRGenerationContext context;
  context.pushScope();
  LLVMContext &llvmContext = context.getLLVMContext();
  Value* fooValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 3);
  Value* barValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 5);

  context.setVariable("foo", fooValue);
  context.pushScope();
  context.setVariable("bar", barValue);
  
  EXPECT_EQ(context.getVariable("bar"), barValue);
  EXPECT_EQ(context.getVariable("foo"), fooValue);

  context.popScope();
  EXPECT_EQ(context.getVariable("foo"), fooValue);
  EXPECT_EQ(context.getVariable("bar") == NULL, true);
  
  context.setVariable("bar", barValue);
  EXPECT_EQ(context.getVariable("foo"), fooValue);
  EXPECT_EQ(context.getVariable("bar"), barValue);
  
  context.popScope();
  EXPECT_EQ(context.getVariable("foo") == NULL, true);
  EXPECT_EQ(context.getVariable("bar") == NULL, true);
}

TEST(IRGenerationContextTest, TestScopesCorrectlyOrdered) {
  IRGenerationContext context;
  context.pushScope();
  LLVMContext &llvmContext = context.getLLVMContext();
  Value* outerValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 3);
  Value* innerValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 5);
  
  context.setVariable("foo", outerValue);
  context.pushScope();
  context.setVariable("foo", innerValue);
  
  EXPECT_EQ(context.getVariable("foo"), innerValue);
  
  context.popScope();
  
  EXPECT_EQ(context.getVariable("foo"), outerValue);
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

TEST(IRGenerationContextTest, ModelTypeRegistryTest) {
  IRGenerationContext context;
  
  StructType* model = StructType::create(context.getLLVMContext(), "mymodel");
  context.addModelType("mymodel", model);
  
  EXPECT_EQ(context.getModelType("mymodel"), model);
}

TEST(IRGenerationContextTest, ModelTypeRedefinedDeathTest) {
  IRGenerationContext context;
  
  StructType* model = StructType::create(context.getLLVMContext(), "mymodel");
  context.addModelType("mymodel", model);
  EXPECT_EXIT(context.addModelType("mymodel", model),
              ::testing::ExitedWithCode(1),
              "Redefinition of MODEL mymodel");
}

TEST(IRGenerationContextTest, ModelTypeDoesNotExistDeathTest) {
  IRGenerationContext context;
  
  EXPECT_EXIT(context.getModelType("mymodel"),
              ::testing::ExitedWithCode(1),
              "MODEL mymodel is not defined");
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
    mContext.setBasicBlock(block);
    mContext.pushScope();
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
