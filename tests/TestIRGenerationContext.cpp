//
//  TestIRGenerationContext.cpp
//  Wisey
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

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct IRGenerationContextTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Interface* mInterface;
  Controller* mController;
  Model* mModel;
  
  IRGenerationContextTest() : mLLVMContext(mContext.getLLVMContext()) {
    StructType* interfaceStructType = StructType::create(mLLVMContext, "IMyInterface");
    vector<Interface*> interfaceParentInterfaces;
    vector<MethodSignature*> interfaceMethodSignatures;
    mInterface = new Interface("IMyInterface",
                               interfaceStructType,
                               interfaceParentInterfaces,
                               interfaceMethodSignatures);
    
    StructType* controllerStructType = StructType::create(mLLVMContext, "CMyController");
    vector<Field*> controllerReceivedFields;
    vector<Field*> controllerInjectedFields;
    vector<Field*> controllerStateFields;
    vector<Method*> controllerMethods;
    vector<Interface*> controllerInterfaces;
    mController = new Controller("CMyController",
                                 controllerStructType,
                                 controllerReceivedFields,
                                 controllerInjectedFields,
                                 controllerStateFields,
                                 controllerMethods,
                                 controllerInterfaces);

    StructType* modelStructType = StructType::create(mLLVMContext, "MMyModel");
    map<string, Field*> modelFields;
    vector<Method*> modelMethods;
    vector<Interface*> modelInterfaces;
    mModel = new Model("MMyModel", modelStructType, modelFields, modelMethods, modelInterfaces);

  }
  
  ~IRGenerationContextTest() { }
};

TEST_F(IRGenerationContextTest, blockStackTest) {
  BasicBlock* block1 = BasicBlock::Create(mLLVMContext, "block1");
  BasicBlock* block2 = BasicBlock::Create(mLLVMContext, "block2");
  
  EXPECT_EQ(mContext.getBasicBlock(), nullptr);
  mContext.setBasicBlock(block1);
  EXPECT_EQ(mContext.getBasicBlock(), block1);
  mContext.setBasicBlock(block2);
  EXPECT_EQ(mContext.getBasicBlock(), block2);
}

TEST_F(IRGenerationContextTest, mainFunctionTest) {
  FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
  Function* function = Function::Create(functionType, GlobalValue::InternalLinkage, "main");

  EXPECT_EQ(mContext.getMainFunction(), nullptr);
  mContext.setMainFunction(function);
  EXPECT_EQ(mContext.getMainFunction(), function);
}

TEST_F(IRGenerationContextTest, moduleIsNotNullTest) {
  EXPECT_NE(mContext.getModule(), nullptr);
}

TEST_F(IRGenerationContextTest, runCodeFailsWhenMainIsNullDeathTest) {
  EXPECT_EXIT(mContext.runCode(),
              ::testing::ExitedWithCode(1),
              "Function main is not defined. Exiting.");
}

TEST_F(IRGenerationContextTest, modelTypeRegistryTest) {
  mContext.addModel(mModel);
  
  Model* resultModel = mContext.getModel("MMyModel");
  
  EXPECT_EQ(resultModel, mModel);
}

TEST_F(IRGenerationContextTest, modelTypeRedefinedDeathTest) {
  mContext.addModel(mModel);
  
  EXPECT_EXIT(mContext.addModel(mModel),
              ::testing::ExitedWithCode(1),
              "Redefinition of MODEL MMyModel");
}

TEST_F(IRGenerationContextTest, modelTypeDoesNotExistDeathTest) {
  EXPECT_EXIT(mContext.getModel("MMyModel"),
              ::testing::ExitedWithCode(1),
              "MODEL MMyModel is not defined");
}

TEST_F(IRGenerationContextTest, controllerTypeRegistryTest) {
  mContext.addController(mController);
  Controller* resultController = mContext.getController("CMyController");
  
  EXPECT_EQ(resultController, mController);
}

TEST_F(IRGenerationContextTest, controllerTypeRedefinedDeathTest) {
  mContext.addController(mController);
  
  EXPECT_EXIT(mContext.addController(mController),
              ::testing::ExitedWithCode(1),
              "Redefinition of Controller CMyController");
}

TEST_F(IRGenerationContextTest, controllerTypeDoesNotExistDeathTest) {
  EXPECT_EXIT(mContext.getController("CMyController"),
              ::testing::ExitedWithCode(1),
              "Controller CMyController is not defined");
}

TEST_F(IRGenerationContextTest, interfaceTypeRegistryTest) {
  StructType* structType = StructType::create(mLLVMContext, "IMyInterface");
  vector<MethodSignature*> methodSignatures;
  vector<Interface*> parentInterfaces;
  Interface* interface = new Interface("IMyInterface",
                                       structType,
                                       parentInterfaces,
                                       methodSignatures);
  mContext.addInterface(interface);
  Interface* resultInterface = mContext.getInterface("IMyInterface");
  
  ASSERT_NE(resultInterface, nullptr);
  EXPECT_EQ(mContext.getInterface("IMyInterface")->getLLVMType(mLLVMContext)->
            getPointerElementType(),
            structType);
}

TEST_F(IRGenerationContextTest, interfaceTypeRedefinedDeathTest) {
  StructType* structType = StructType::create(mLLVMContext, "IMyInterface");
  vector<MethodSignature*> methodSignatures;
  vector<Interface*> parentInterfaces;
  Interface* interface = new Interface("IMyInterface",
                                       structType,
                                       parentInterfaces,
                                       methodSignatures);
  mContext.addInterface(interface);
  
  EXPECT_EXIT(mContext.addInterface(interface),
              ::testing::ExitedWithCode(1),
              "Redefinition of Interface IMyInterface");
}

TEST_F(IRGenerationContextTest, interfaceTypeDoesNotExistDeathTest) {
  EXPECT_EXIT(mContext.getInterface("myinterface"),
              ::testing::ExitedWithCode(1),
              "Interface myinterface is not defined");
}

TEST_F(IRGenerationContextTest, getBoundControllerDeathTest) {
  mContext.addController(mController);
  mContext.addInterface(mInterface);
  
  EXPECT_EXIT(mContext.getBoundController(mInterface),
              ::testing::ExitedWithCode(1),
              "Error: No controller is bound to interface IMyInterface");
}

TEST_F(IRGenerationContextTest, bindInterfaceToControllerTest) {
  mContext.addController(mController);
  mContext.addInterface(mInterface);
  
  mContext.bindInterfaceToController(mInterface, mController);
  
  EXPECT_EQ(mContext.getBoundController(mInterface), mController);
}

TEST_F(IRGenerationContextTest, bindInterfaceToControllerRepeatedlyDeathTest) {
  mContext.addController(mController);
  mContext.addInterface(mInterface);
  
  mContext.bindInterfaceToController(mInterface, mController);

  EXPECT_EXIT(mContext.bindInterfaceToController(mInterface, mController),
              ::testing::ExitedWithCode(1),
              "Error: Interface IMyInterface is already bound to CMyController "
              "and can not be bound to CMyController");
}

TEST_F(IRGenerationContextTest, setPackageDeathTest) {
  EXPECT_EXIT(mContext.setPackage("with.Uppercase.letters"),
              ::testing::ExitedWithCode(1),
              "Error: Package names should only conain lowercase characters");
}

TEST_F(IRGenerationContextTest, setPackageTest) {
  string package = "some.wierd_name.of1package";
  mContext.setPackage(package);
  
  EXPECT_EQ(mContext.getPackage(), package);
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
    BasicBlock* basicBlock = BasicBlock::Create(llvmContext, "entry", function);
    mContext.setBasicBlock(basicBlock);
    mContext.getScopes().pushScope();
    Value* returnValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 5);
    IRWriter::createReturnInst(mContext, returnValue);
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
