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

#include "MockVariable.hpp"
#include "TestPrefix.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IInterfaceTypeSpecifier.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct IRGenerationContextTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Interface* mInterface;
  Controller* mController;
  Model* mModel;
  Node* mNode;
  Thread* mThread;

  IRGenerationContextTest() : mLLVMContext(mContext.getLLVMContext()) {
    string interfaceFullName = "systems.vos.wisey.compiler.tests.IMyInterface";
    StructType* interfaceStructType = StructType::create(mLLVMContext, interfaceFullName);
    vector<IInterfaceTypeSpecifier*> parentInterfaces;
    vector<IObjectElementDeclaration*> interfaceElements;
    mInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                         interfaceFullName,
                                         interfaceStructType,
                                         parentInterfaces,
                                         interfaceElements);
    
    string controllerFullName = "systems.vos.wisey.compiler.tests.CMyController";
    StructType* controllerStructType = StructType::create(mLLVMContext, controllerFullName);
    vector<Field*> controllerFields;
    vector<IMethod*> controllerMethods;
    vector<Interface*> controllerInterfaces;
    mController = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                            controllerFullName,
                                            controllerStructType);
    mController->setFields(controllerFields, controllerInterfaces.size() + 1);
    mController->setMethods(controllerMethods);
    mController->setInterfaces(controllerInterfaces);

    string modelFullName = "systems.vos.wisey.compiler.tests.MMyModel";
    StructType* modelStructType = StructType::create(mLLVMContext, "MMyModel");
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS, modelFullName, modelStructType);

    string nodeFullName = "systems.vos.wisey.compiler.tests.NMyNode";
    StructType* nodeStructType = StructType::create(mLLVMContext, "NMyNode");
    mNode = Node::newNode(AccessLevel::PUBLIC_ACCESS, nodeFullName, nodeStructType);

    string threadFullName = "systems.vos.wisey.compiler.tests.TMyThread";
    StructType* threadStructType = StructType::create(mLLVMContext, "TMyThread");
    mThread = Thread::newThread(AccessLevel::PUBLIC_ACCESS, threadFullName, threadStructType);
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

TEST_F(IRGenerationContextTest, addNodeTest) {
  mContext.addNode(mNode);
  
  Node* resultNode = mContext.getNode("systems.vos.wisey.compiler.tests.NMyNode");
  
  EXPECT_EQ(resultNode, mNode);
}

TEST_F(IRGenerationContextTest, addNodeAlreadyDefinedDeathTest) {
  mContext.addNode(mNode);
  
  EXPECT_EXIT(mContext.addNode(mNode),
              ::testing::ExitedWithCode(1),
              "Redefinition of node systems.vos.wisey.compiler.tests.NMyNode");
}

TEST_F(IRGenerationContextTest, getNodeDoesNotExistDeathTest) {
  EXPECT_EXIT(mContext.getNode("systems.vos.wisey.compiler.tests.NMyNode"),
              ::testing::ExitedWithCode(1),
              "Node systems.vos.wisey.compiler.tests.NMyNode is not defined");
}

TEST_F(IRGenerationContextTest, addModelTest) {
  mContext.addModel(mModel);
  
  Model* resultModel = mContext.getModel("systems.vos.wisey.compiler.tests.MMyModel");
  
  EXPECT_EQ(resultModel, mModel);
}

TEST_F(IRGenerationContextTest, addModelAlreadyDefinedDeathTest) {
  mContext.addModel(mModel);
  
  EXPECT_EXIT(mContext.addModel(mModel),
              ::testing::ExitedWithCode(1),
              "Redefinition of model systems.vos.wisey.compiler.tests.MMyModel");
}

TEST_F(IRGenerationContextTest, getModelDoesNotExistDeathTest) {
  EXPECT_EXIT(mContext.getModel("systems.vos.wisey.compiler.tests.MMyModel"),
              ::testing::ExitedWithCode(1),
              "Model systems.vos.wisey.compiler.tests.MMyModel is not defined");
}

TEST_F(IRGenerationContextTest, addControllerTest) {
  mContext.addController(mController);
  Controller* resultController =
    mContext.getController("systems.vos.wisey.compiler.tests.CMyController");
  
  EXPECT_EQ(resultController, mController);
}

TEST_F(IRGenerationContextTest, addControllerAlreadyDefinedDeathTest) {
  mContext.addController(mController);
  
  EXPECT_EXIT(mContext.addController(mController),
              ::testing::ExitedWithCode(1),
              "Redefinition of controller systems.vos.wisey.compiler.tests.CMyController");
}

TEST_F(IRGenerationContextTest, getControllerDoesNotExistDeathTest) {
  EXPECT_EXIT(mContext.getController("systems.vos.wisey.compiler.tests.CMyController"),
              ::testing::ExitedWithCode(1),
              "Controller systems.vos.wisey.compiler.tests.CMyController is not defined");
}

TEST_F(IRGenerationContextTest, addThreadTest) {
  mContext.addThread(mThread);

  Thread* resultThread = mContext.getThread("systems.vos.wisey.compiler.tests.TMyThread");
  
  EXPECT_EQ(mThread, resultThread);
}

TEST_F(IRGenerationContextTest, addThreadAlreadyDefinedDeathTest) {
  mContext.addThread(mThread);
  
  EXPECT_EXIT(mContext.addThread(mThread),
              ::testing::ExitedWithCode(1),
              "Redefinition of thread systems.vos.wisey.compiler.tests.TMyThread");
}

TEST_F(IRGenerationContextTest, getThreadDoesNotExistDeathTest) {
  EXPECT_EXIT(mContext.getThread("systems.vos.wisey.compiler.tests.TMyThread"),
              ::testing::ExitedWithCode(1),
              "Thread systems.vos.wisey.compiler.tests.TMyThread is not defined");
}

TEST_F(IRGenerationContextTest, addInterfaceTest) {
  string interfaceFullName = "systems.vos.wisey.compiler.tests.IMyInterface";
  StructType* structType = StructType::create(mLLVMContext, interfaceFullName);
  vector<IInterfaceTypeSpecifier*> parentInterfaces;
  vector<IObjectElementDeclaration*> interfaceElements;
  Interface* interface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                 interfaceFullName,
                                                 structType,
                                                 parentInterfaces,
                                                 interfaceElements);
  mContext.addInterface(interface);
  Interface* resultInterface =
    mContext.getInterface("systems.vos.wisey.compiler.tests.IMyInterface");
  
  ASSERT_NE(resultInterface, nullptr);
  EXPECT_EQ(resultInterface->getLLVMType(mContext)->getPointerElementType(),
            structType);
}

TEST_F(IRGenerationContextTest, addInterfaceAlreadyDefinedDeathTest) {
  string interfaceFullName = "systems.vos.wisey.compiler.tests.IMyInterface";
  StructType* structType = StructType::create(mLLVMContext, interfaceFullName);
  vector<IInterfaceTypeSpecifier*> parentInterfaces;
  vector<IObjectElementDeclaration*> interfaceElements;
  Interface* interface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                 interfaceFullName,
                                                 structType,
                                                 parentInterfaces,
                                                 interfaceElements);
  mContext.addInterface(interface);
  
  EXPECT_EXIT(mContext.addInterface(interface),
              ::testing::ExitedWithCode(1),
              "Redefinition of interface systems.vos.wisey.compiler.tests.IMyInterface");
}

TEST_F(IRGenerationContextTest, setObjectTypeTest) {
  Model* model = Model::newModel(AccessLevel::PUBLIC_ACCESS, "MModel", NULL);
  
  mContext.setObjectType(model);
  
  ASSERT_EQ(mContext.getObjectType(), model);
}

TEST_F(IRGenerationContextTest, getInterfaceDoesNotExistDeathTest) {
  EXPECT_EXIT(mContext.getInterface("systems.vos.wisey.compiler.tests.IMyInterface"),
              ::testing::ExitedWithCode(1),
              "Interface systems.vos.wisey.compiler.tests.IMyInterface is not defined");
}

TEST_F(IRGenerationContextTest, getBoundControllerDeathTest) {
  mContext.addController(mController);
  mContext.addInterface(mInterface);
  
  EXPECT_EXIT(mContext.getBoundController(mInterface),
              ::testing::ExitedWithCode(1),
              "Error: No controller is bound to "
              "interface systems.vos.wisey.compiler.tests.IMyInterface");
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
              "Error: Interface systems.vos.wisey.compiler.tests.IMyInterface "
              "is already bound to systems.vos.wisey.compiler.tests.CMyController "
              "and can not be bound to systems.vos.wisey.compiler.tests.CMyController");
}

TEST_F(IRGenerationContextTest, getPackageTypeTest) {
  string package = "systems.vos.wisey.compiler.tests";
  PackageType* packageType = mContext.getPackageType(package);
  
  ASSERT_NE(nullptr, packageType);
  EXPECT_EQ(packageType, mContext.getPackageType(package));
}

TEST_F(IRGenerationContextTest, getThisTest) {
  mContext.getScopes().pushScope();
  NiceMock<MockVariable> mockVariable;
  ON_CALL(mockVariable, getName()).WillByDefault(Return("this"));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(mModel));
  mContext.getScopes().setVariable(&mockVariable);
  mContext.getScopes().pushScope();
  mContext.getScopes().pushScope();

  EXPECT_EQ(mContext.getThis(), &mockVariable);
}

TEST_F(IRGenerationContextTest, printToStreamTest) {
  mContext.addInterface(mInterface);
  mContext.addNode(mNode);
  mContext.addController(mController);
  mContext.addModel(mModel);
  
  stringstream stringStream;
  mContext.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("/* Interfaces */\n"
               "\n"
               "external interface systems.vos.wisey.compiler.tests.IMyInterface {\n"
               "}\n"
               "\n"
               "/* Models */\n"
               "\n"
               "external model systems.vos.wisey.compiler.tests.MMyModel {\n"
               "}\n"
               "\n"
               "/* Controllers */\n"
               "\n"
               "external controller systems.vos.wisey.compiler.tests.CMyController {\n"
               "}\n"
               "\n"
               "/* Nodes */\n"
               "\n"
               "external node systems.vos.wisey.compiler.tests.NMyNode {\n"
               "}\n"
               "\n"
               "/* Bindings */\n"
               "\n",
               stringStream.str().c_str());
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
  string stringBuffer;
  raw_string_ostream stringStream(stringBuffer);
  
  mContext.printAssembly(stringStream);
  string expected = string() +
    "; ModuleID = 'wisey'\n" +
    "source_filename = \"wisey\"\n" +
    "\n" +
    "define internal i32 @main() {\n" +
    "entry:\n" +
    "  ret i32 5\n" +
    "}\n";
  
  EXPECT_STREQ(expected.c_str(), stringBuffer.c_str());
}

TEST_F(IRGenerationContextTest, getArrayTypeTest) {
  wisey::ArrayType* arrayType = mContext.getArrayType(PrimitiveTypes::INT_TYPE, 1u);
  
  EXPECT_EQ(arrayType, mContext.getArrayType(PrimitiveTypes::INT_TYPE, 1u));
}

TEST_F(IRGenerationContextTest, getArrayExactTypeTest) {
  list<unsigned long> dimensions;
  dimensions.push_back(5);
  dimensions.push_back(3);
  ArrayExactType* arrayExactType = mContext.getArrayExactType(PrimitiveTypes::INT_TYPE, dimensions);
  
  EXPECT_EQ(arrayExactType, mContext.getArrayExactType(PrimitiveTypes::INT_TYPE, dimensions));
}

