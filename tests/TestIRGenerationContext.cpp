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
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

TEST(IRGenerationContextTest, blockStackTest) {
  IRGenerationContext context;
  LLVMContext &llvmContext = context.getLLVMContext();
  BasicBlock* block1 = BasicBlock::Create(llvmContext, "block1");
  BasicBlock* block2 = BasicBlock::Create(llvmContext, "block2");
  
  EXPECT_EQ(context.getBasicBlock(), nullptr);
  context.setBasicBlock(block1);
  EXPECT_EQ(context.getBasicBlock(), block1);
  context.setBasicBlock(block2);
  EXPECT_EQ(context.getBasicBlock(), block2);
}

TEST(IRGenerationContextTest, mainFunctionTest) {
  IRGenerationContext context;
  FunctionType* functionType = FunctionType::get(Type::getInt32Ty(context.getLLVMContext()), false);
  Function* function = Function::Create(functionType, GlobalValue::InternalLinkage, "main");

  EXPECT_EQ(context.getMainFunction(), nullptr);
  context.setMainFunction(function);
  EXPECT_EQ(context.getMainFunction(), function);
}

TEST(IRGenerationContextTest, moduleIsNotNullTest) {
  IRGenerationContext context;
  
  EXPECT_NE(context.getModule(), nullptr);
}

TEST(IRGenerationContextTest, runCodeFailsWhenMainIsNullDeathTest) {
  IRGenerationContext context;
  
  EXPECT_EXIT(context.runCode(),
              ::testing::ExitedWithCode(1),
              "Function main is not defined. Exiting.");
}

TEST(IRGenerationContextTest, modelTypeRegistryTest) {
  IRGenerationContext context;
  
  StructType* structType = StructType::create(context.getLLVMContext(), "mymodel");
  map<string, ModelField*> fields;
  vector<Method*> methods;
  vector<Interface*> interfaces;
  Model* model = new Model("mymodel", structType, fields, methods, interfaces);
  context.addModel(model);
  Model* resultModel = context.getModel("mymodel");
  
  ASSERT_NE(resultModel, nullptr);
  EXPECT_EQ(context.getModel("mymodel")->getLLVMType(context.getLLVMContext()),
            structType->getPointerTo());
}

TEST(IRGenerationContextTest, modelTypeRedefinedDeathTest) {
  IRGenerationContext context;
  
  StructType* structType = StructType::create(context.getLLVMContext(), "mymodel");
  map<string, ModelField*> fields;
  vector<Method*> methods;
  vector<Interface*> interfaces;
  Model* model = new Model("mymodel", structType, fields, methods, interfaces);
  context.addModel(model);
  
  EXPECT_EXIT(context.addModel(model),
              ::testing::ExitedWithCode(1),
              "Redefinition of MODEL mymodel");
}

TEST(IRGenerationContextTest, modelTypeDoesNotExistDeathTest) {
  IRGenerationContext context;
  
  EXPECT_EXIT(context.getModel("mymodel"),
              ::testing::ExitedWithCode(1),
              "MODEL mymodel is not defined");
}

TEST(IRGenerationContextTest, interfaceTypeRegistryTest) {
  IRGenerationContext context;
  LLVMContext& llvmContext = context.getLLVMContext();
  
  StructType* structType = StructType::create(llvmContext, "myinterface");
  vector<MethodSignature*> methodSignatures;
  vector<Interface*> parentInterfaces;
  Interface* interface = new Interface("myinterface",
                                       structType,
                                       parentInterfaces,
                                       methodSignatures);
  context.addInterface(interface);
  Interface* resultInterface = context.getInterface("myinterface");
  
  ASSERT_NE(resultInterface, nullptr);
  EXPECT_EQ(context.getInterface("myinterface")->getLLVMType(llvmContext)->getPointerElementType(),
            structType);
}

TEST(IRGenerationContextTest, interfaceTypeRedefinedDeathTest) {
  IRGenerationContext context;
  
  StructType* structType = StructType::create(context.getLLVMContext(), "myinterface");
  vector<MethodSignature*> methodSignatures;
  vector<Interface*> parentInterfaces;
  Interface* interface = new Interface("myinterface",
                                       structType,
                                       parentInterfaces,
                                       methodSignatures);
  context.addInterface(interface);
  
  EXPECT_EXIT(context.addInterface(interface),
              ::testing::ExitedWithCode(1),
              "Redefinition of Interface myinterface");
}

TEST(IRGenerationContextTest, interfaceTypeDoesNotExistDeathTest) {
  IRGenerationContext context;
  
  EXPECT_EXIT(context.getInterface("myinterface"),
              ::testing::ExitedWithCode(1),
              "Interface myinterface is not defined");
}

TEST(IRGenerationContextTest, globalFunctionDefinitionTest) {
  IRGenerationContext context;

  context.addGlobalFunction(PrimitiveTypes::VOID_TYPE, "foo");
  
  EXPECT_EQ(context.getGlobalFunctionType("foo"), PrimitiveTypes::VOID_TYPE);
}

TEST(IRGenerationContextTest, globalFunctionRedefinitionDeathTest) {
  IRGenerationContext context;
  
  context.addGlobalFunction(PrimitiveTypes::VOID_TYPE, "foo");
  
  EXPECT_EXIT(context.addGlobalFunction(PrimitiveTypes::FLOAT_TYPE, "foo"),
              ::testing::ExitedWithCode(1),
              "Error: Redefinition of a global function foo");
}

TEST(IRGenerationContextTest, globalFunctionUndefinedDeathTest) {
  IRGenerationContext context;
  
  EXPECT_EXIT(context.getGlobalFunctionType("foo"),
              ::testing::ExitedWithCode(1),
              "Global function foo is not defined");
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
    mContext.getScopes().pushScope();
    Value* returnValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 5);
    ReturnInst::Create(llvmContext, returnValue, block);
  }
  
  ~IRGenerationContextRunTest() {
  }
};

TEST_F(IRGenerationContextRunTest, runCodeTest) {
  GenericValue result = mContext.runCode();
  
  EXPECT_EQ(result.IntVal.toString(10, true), "5");
}

TEST_F(IRGenerationContextRunTest, printAssemblyTest) {
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
