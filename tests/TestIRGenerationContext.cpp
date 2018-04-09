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
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm-c/Target.h>

#include "MockVariable.hpp"
#include "TestPrefix.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IInterfaceTypeSpecifier.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
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
  LLVMStructType* mLLVMStructType;

  IRGenerationContextTest() : mLLVMContext(mContext.getLLVMContext()) {
    string interfaceFullName = "systems.vos.wisey.compiler.tests.IMyInterface";
    StructType* interfaceStructType = StructType::create(mLLVMContext, interfaceFullName);
    vector<IInterfaceTypeSpecifier*> parentInterfaces;
    vector<IObjectElementDefinition*> interfaceElements;
    mInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                         interfaceFullName,
                                         interfaceStructType,
                                         parentInterfaces,
                                         interfaceElements);
    
    string controllerFullName = "systems.vos.wisey.compiler.tests.CMyController";
    StructType* controllerStructType = StructType::create(mLLVMContext, controllerFullName);
    vector<IField*> controllerFields;
    vector<IMethod*> controllerMethods;
    vector<Interface*> controllerInterfaces;
    mController = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                            controllerFullName,
                                            controllerStructType);
    mController->setFields(controllerFields, 1u);
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
    
    StructType* llvmStructType = StructType::create(mLLVMContext, "mystructtype");
    mLLVMStructType = LLVMStructType::newLLVMStructType(llvmStructType);
    
    ImportProfile* importProfile = new ImportProfile("systems.vos.wisey.compiler.tests");
    importProfile->setSourceFileName(mContext, "/sources/sourcefile.yz");
    
    mContext.setImportProfile(importProfile);
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
  
  Node* resultNode = mContext.getNode("systems.vos.wisey.compiler.tests.NMyNode", 0);
  
  EXPECT_EQ(resultNode, mNode);
}

TEST_F(IRGenerationContextTest, addNodeAlreadyDefinedDeathTest) {
  mContext.addNode(mNode);
  
  EXPECT_EXIT(mContext.addNode(mNode),
              ::testing::ExitedWithCode(1),
              "Redefinition of node systems.vos.wisey.compiler.tests.NMyNode");
}

TEST_F(IRGenerationContextTest, getNodeDoesNotExistDeathTest) {
  EXPECT_EXIT(mContext.getNode("systems.vos.wisey.compiler.tests.NMyNode", 10),
              ::testing::ExitedWithCode(1),
              "/sources/sourcefile.yz\\(10\\): Error: Node systems.vos.wisey.compiler.tests.NMyNode is not defined");
}

TEST_F(IRGenerationContextTest, addModelTest) {
  mContext.addModel(mModel);
  
  Model* resultModel = mContext.getModel("systems.vos.wisey.compiler.tests.MMyModel", 0);
  
  EXPECT_EQ(resultModel, mModel);
}

TEST_F(IRGenerationContextTest, addModelAlreadyDefinedDeathTest) {
  mContext.addModel(mModel);
  
  EXPECT_EXIT(mContext.addModel(mModel),
              ::testing::ExitedWithCode(1),
              "Redefinition of model systems.vos.wisey.compiler.tests.MMyModel");
}

TEST_F(IRGenerationContextTest, getModelDoesNotExistDeathTest) {
  EXPECT_EXIT(mContext.getModel("systems.vos.wisey.compiler.tests.MMyModel", 10),
              ::testing::ExitedWithCode(1),
              "/sources/sourcefile.yz\\(10\\): Error: Model systems.vos.wisey.compiler.tests.MMyModel is not defined");
}

TEST_F(IRGenerationContextTest, addControllerTest) {
  mContext.addController(mController);
  Controller* resultController =
    mContext.getController("systems.vos.wisey.compiler.tests.CMyController", 0);
  
  EXPECT_EQ(resultController, mController);
}

TEST_F(IRGenerationContextTest, addControllerAlreadyDefinedDeathTest) {
  mContext.addController(mController);
  
  EXPECT_EXIT(mContext.addController(mController),
              ::testing::ExitedWithCode(1),
              "Redefinition of controller systems.vos.wisey.compiler.tests.CMyController");
}

TEST_F(IRGenerationContextTest, getControllerDoesNotExistDeathTest) {
  EXPECT_EXIT(mContext.getController("systems.vos.wisey.compiler.tests.CMyController", 10),
              ::testing::ExitedWithCode(1),
              "/sources/sourcefile.yz\\(10\\): Error: Controller systems.vos.wisey.compiler.tests.CMyController is not defined");
}

TEST_F(IRGenerationContextTest, addThreadTest) {
  mContext.addThread(mThread);

  Thread* resultThread = mContext.getThread("systems.vos.wisey.compiler.tests.TMyThread", 0);
  
  EXPECT_EQ(mThread, resultThread);
}

TEST_F(IRGenerationContextTest, addThreadAlreadyDefinedDeathTest) {
  mContext.addThread(mThread);
  
  EXPECT_EXIT(mContext.addThread(mThread),
              ::testing::ExitedWithCode(1),
              "Redefinition of thread systems.vos.wisey.compiler.tests.TMyThread");
}

TEST_F(IRGenerationContextTest, getThreadDoesNotExistDeathTest) {
  EXPECT_EXIT(mContext.getThread("systems.vos.wisey.compiler.tests.TMyThread", 10),
              ::testing::ExitedWithCode(1),
              "/sources/sourcefile.yz\\(10\\): Error: Thread systems.vos.wisey.compiler.tests.TMyThread is not defined");
}

TEST_F(IRGenerationContextTest, addLLVMStructTypeTest) {
  mContext.addLLVMStructType(mLLVMStructType);
  LLVMStructType* resultLLVMStructType = mContext.getLLVMStructType("mystructtype", 0);
  
  EXPECT_EQ(mLLVMStructType, resultLLVMStructType);
}

TEST_F(IRGenerationContextTest, addLLVMStructTypeAlreadyDefinedDeathTest) {
  mContext.addLLVMStructType(mLLVMStructType);

  EXPECT_EXIT(mContext.addLLVMStructType(mLLVMStructType),
              ::testing::ExitedWithCode(1),
              "Redefinition of llvm struct type ::llvm::struct::mystructtype");
}

TEST_F(IRGenerationContextTest, getLLVMStructTypeDoesNotExistDeathTest) {
  EXPECT_EXIT(mContext.getLLVMStructType("mystructtype", 10),
              ::testing::ExitedWithCode(1),
              "/sources/sourcefile.yz\\(10\\): Error: llvm struct type ::llvm::struct::mystructtype is not defined");
}

TEST_F(IRGenerationContextTest, addInterfaceTest) {
  string interfaceFullName = "systems.vos.wisey.compiler.tests.IMyInterface";
  StructType* structType = StructType::create(mLLVMContext, interfaceFullName);
  vector<IInterfaceTypeSpecifier*> parentInterfaces;
  vector<IObjectElementDefinition*> interfaceElements;
  Interface* interface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                 interfaceFullName,
                                                 structType,
                                                 parentInterfaces,
                                                 interfaceElements);
  mContext.addInterface(interface);
  Interface* resultInterface =
    mContext.getInterface("systems.vos.wisey.compiler.tests.IMyInterface", 0);
  
  ASSERT_NE(resultInterface, nullptr);
  EXPECT_EQ(resultInterface->getLLVMType(mContext)->getPointerElementType(),
            structType);
}

TEST_F(IRGenerationContextTest, addInterfaceAlreadyDefinedDeathTest) {
  string interfaceFullName = "systems.vos.wisey.compiler.tests.IMyInterface";
  StructType* structType = StructType::create(mLLVMContext, interfaceFullName);
  vector<IInterfaceTypeSpecifier*> parentInterfaces;
  vector<IObjectElementDefinition*> interfaceElements;
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
  EXPECT_EXIT(mContext.getInterface("systems.vos.wisey.compiler.tests.IMyInterface", 10),
              ::testing::ExitedWithCode(1),
              "/sources/sourcefile.yz\\(10\\): Error: Interface systems.vos.wisey.compiler.tests.IMyInterface is not defined");
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
  ON_CALL(mockVariable, getName()).WillByDefault(Return(IObjectType::THIS));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(mModel));
  mContext.getScopes().setVariable(&mockVariable);
  mContext.getScopes().pushScope();
  mContext.getScopes().pushScope();

  EXPECT_EQ(mContext.getThis(), &mockVariable);
}

TEST_F(IRGenerationContextTest, printToStreamTest) {
  mContext.addLLVMStructType(mLLVMStructType);
  mContext.setLLVMGlobalVariable(mLLVMStructType->getPointerType(), "myglobal");
  mContext.addInterface(mInterface);
  mContext.addNode(mNode);
  mContext.addController(mController);
  mContext.addModel(mModel);
  mContext.addThread(mThread);
  
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
               "/* Threads */\n"
               "\n"
               "external thread systems.vos.wisey.compiler.tests.TMyThread {\n"
               "}\n"
               "\n"
               "/* Bindings */\n"
               "\n"
               "/* llvm Structs */\n"
               "\n"
               "external ::llvm::struct mystructtype {\n"
               "}\n"
               "\n"
               "/* llvm Functions */\n"
               "\n"
               "/* llvm Globals */\n"
               "\n"
               "::llvm::struct::mystructtype::pointer myglobal;"
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
  InitializeNativeTarget();
  LLVMInitializeNativeAsmPrinter();
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

TEST_F(IRGenerationContextTest, getLLVMArrayTypeTest) {
  list<unsigned long> dimensions;
  dimensions.push_back(5);
  dimensions.push_back(3);
  LLVMArrayType* llvmArrayType = mContext.getLLVMArrayType(LLVMPrimitiveTypes::I16, dimensions);
  
  EXPECT_EQ(llvmArrayType, mContext.getLLVMArrayType(LLVMPrimitiveTypes::I16, dimensions));
}

TEST_F(IRGenerationContextTest, getLLVMFunctionTypeTest) {
  vector<const IType*> argumentTypes;
  argumentTypes.push_back(LLVMPrimitiveTypes::I16);
  argumentTypes.push_back(LLVMPrimitiveTypes::I64->getPointerType());
  LLVMFunctionType* llvmFunctionType = mContext.getLLVMFunctionType(LLVMPrimitiveTypes::I8,
                                                                    argumentTypes);
  
  EXPECT_EQ(llvmFunctionType, mContext.getLLVMFunctionType(LLVMPrimitiveTypes::I8, argumentTypes));
}

TEST_F(IRGenerationContextTest, registerLLVMFunctionNamedTypeTest) {
  vector<const IType*> arguments;
  arguments.push_back(LLVMPrimitiveTypes::I16);
  LLVMFunctionType* functionType = new LLVMFunctionType(LLVMPrimitiveTypes::I8, arguments);
  mContext.registerLLVMFunctionNamedType("myfunction", functionType);
  
  EXPECT_EQ(functionType, mContext.lookupLLVMFunctionNamedType("myfunction"));
}

TEST_F(IRGenerationContextTest, registerLLVMFunctionNamedTypeDeathTest) {
  vector<const IType*> arguments;
  arguments.push_back(LLVMPrimitiveTypes::I16);
  LLVMFunctionType* functionType = new LLVMFunctionType(LLVMPrimitiveTypes::I8, arguments);
  mContext.registerLLVMFunctionNamedType("myfunction", functionType);
  
  EXPECT_EXIT(mContext.registerLLVMFunctionNamedType("myfunction", functionType),
              ::testing::ExitedWithCode(1),
              "Can not register llvm function named myfunction because it is already registered");
}

TEST_F(IRGenerationContextTest, lookupLLVMFunctionNamedTypeDeathTest) {
  EXPECT_EXIT(mContext.lookupLLVMFunctionNamedType("myfunction"),
              ::testing::ExitedWithCode(1),
              "Can not find llvm function named myfunction");
}
