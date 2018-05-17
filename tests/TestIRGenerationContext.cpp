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
#include "TestFileRunner.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IInterfaceTypeSpecifier.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/Log.hpp"
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
  LLVMStructType* mLLVMStructType;

  IRGenerationContextTest() : mLLVMContext(mContext.getLLVMContext()) {
    ImportProfile* importProfile = new ImportProfile("systems.vos.wisey.compiler.tests");
    importProfile->setSourceFileName(mContext, "/tmp/source.yz");
    mContext.setImportProfile(importProfile);

    string interfaceFullName = "systems.vos.wisey.compiler.tests.IMyInterface";
    StructType* interfaceStructType = StructType::create(mLLVMContext, interfaceFullName);
    vector<IInterfaceTypeSpecifier*> parentInterfaces;
    vector<IObjectElementDefinition*> interfaceElements;
    mInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                         interfaceFullName,
                                         interfaceStructType,
                                         parentInterfaces,
                                         interfaceElements,
                                         mContext.getImportProfile(),
                                         0);
    
    string controllerFullName = "systems.vos.wisey.compiler.tests.CMyController";
    StructType* controllerStructType = StructType::create(mLLVMContext, controllerFullName);
    vector<IField*> controllerFields;
    vector<IMethod*> controllerMethods;
    vector<Interface*> controllerInterfaces;
    controllerInterfaces.push_back(mInterface);
    mController = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                            controllerFullName,
                                            controllerStructType,
                                            mContext.getImportProfile(),
                                            0);
    mController->setFields(mContext, controllerFields, 1u);
    mController->setMethods(controllerMethods);
    mController->setInterfaces(controllerInterfaces);

    string modelFullName = "systems.vos.wisey.compiler.tests.MMyModel";
    StructType* modelStructType = StructType::create(mLLVMContext, "MMyModel");
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                             modelFullName,
                             modelStructType,
                             mContext.getImportProfile(),
                             0);

    string nodeFullName = "systems.vos.wisey.compiler.tests.NMyNode";
    StructType* nodeStructType = StructType::create(mLLVMContext, "NMyNode");
    mNode = Node::newNode(AccessLevel::PUBLIC_ACCESS,
                          nodeFullName,
                          nodeStructType,
                          mContext.getImportProfile(),
                          0);

    StructType* llvmStructType = StructType::create(mLLVMContext, "mystructtype");
    mLLVMStructType = LLVMStructType::newLLVMStructType(llvmStructType);
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
  EXPECT_EXIT(mContext.runCode(0, NULL),
              ::testing::ExitedWithCode(1),
              "Error: Function main is not defined. Exiting.\n");
}

TEST_F(IRGenerationContextTest, addNodeTest) {
  mContext.addNode(mNode, 0);
  
  Node* resultNode = mContext.getNode("systems.vos.wisey.compiler.tests.NMyNode", 0);
  
  EXPECT_EQ(resultNode, mNode);
}

TEST_F(IRGenerationContextTest, addNodeAlreadyDefinedDeathTest) {
  mContext.addNode(mNode, 1);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mContext.addNode(mNode, 5));
  EXPECT_STREQ("/tmp/source.yz(5): Error: Redefinition of node systems.vos.wisey.compiler.tests.NMyNode\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(IRGenerationContextTest, getNodeDoesNotExistDeathTest) {
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mContext.getNode("systems.vos.wisey.compiler.tests.NMyNode", 10));
  EXPECT_STREQ("/tmp/source.yz(10): Error: Node systems.vos.wisey.compiler.tests.NMyNode is not defined\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(IRGenerationContextTest, addModelTest) {
  mContext.addModel(mModel, 0);
  
  Model* resultModel = mContext.getModel("systems.vos.wisey.compiler.tests.MMyModel", 0);
  
  EXPECT_EQ(resultModel, mModel);
}

TEST_F(IRGenerationContextTest, addModelAlreadyDefinedDeathTest) {
  mContext.addModel(mModel, 0);
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mContext.addModel(mModel, 1));
  EXPECT_STREQ("/tmp/source.yz(1): Error: Redefinition of model systems.vos.wisey.compiler.tests.MMyModel\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(IRGenerationContextTest, getModelDoesNotExistDeathTest) {
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mContext.getModel("systems.vos.wisey.compiler.tests.MMyModel", 10));
  EXPECT_STREQ("/tmp/source.yz(10): Error: Model systems.vos.wisey.compiler.tests.MMyModel is not defined\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(IRGenerationContextTest, addControllerTest) {
  mContext.addController(mController, 0);
  Controller* resultController =
    mContext.getController("systems.vos.wisey.compiler.tests.CMyController", 0);
  
  EXPECT_EQ(resultController, mController);
}

TEST_F(IRGenerationContextTest, addControllerAlreadyDefinedDeathTest) {
  mContext.addController(mController, 1);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mContext.addController(mController, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Redefinition of controller systems.vos.wisey.compiler.tests.CMyController\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(IRGenerationContextTest, getControllerDoesNotExistDeathTest) {
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mContext.getController("systems.vos.wisey.compiler.tests.CMyController", 10));
  EXPECT_STREQ("/tmp/source.yz(10): Error: Controller systems.vos.wisey.compiler.tests.CMyController is not defined\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(IRGenerationContextTest, addLLVMStructTypeTest) {
  mContext.addLLVMStructType(mLLVMStructType, 0);
  LLVMStructType* resultLLVMStructType = mContext.getLLVMStructType("mystructtype", 0);
  
  EXPECT_EQ(mLLVMStructType, resultLLVMStructType);
}

TEST_F(IRGenerationContextTest, addLLVMStructTypeAlreadyDefinedDeathTest) {
  mContext.addLLVMStructType(mLLVMStructType, 0);

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mContext.addLLVMStructType(mLLVMStructType, 1));
  EXPECT_STREQ("/tmp/source.yz(1): Error: Redefinition of llvm struct type ::llvm::struct::mystructtype\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(IRGenerationContextTest, getLLVMStructTypeDoesNotExistDeathTest) {
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mContext.getLLVMStructType("mystructtype", 10));
  EXPECT_STREQ("/tmp/source.yz(10): Error: llvm struct type ::llvm::struct::mystructtype is not defined\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
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
                                                 interfaceElements,
                                                 mContext.getImportProfile(),
                                                 0);
  mContext.addInterface(interface, 0);
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
                                                 interfaceElements,
                                                 mContext.getImportProfile(),
                                                 0);
  mContext.addInterface(interface, 1);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mContext.addInterface(interface, 5));
  EXPECT_STREQ("/tmp/source.yz(5): Error: Redefinition of interface systems.vos.wisey.compiler.tests.IMyInterface\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(IRGenerationContextTest, setObjectTypeTest) {
  Model* model = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                                 "MModel",
                                 NULL,
                                 mContext.getImportProfile(),
                                 0);
  
  mContext.setObjectType(model);
  
  ASSERT_EQ(mContext.getObjectType(), model);
}

TEST_F(IRGenerationContextTest, getInterfaceDoesNotExistDeathTest) {
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mContext.getInterface("systems.vos.wisey.compiler.tests.IMyInterface", 10));
  EXPECT_STREQ("/tmp/source.yz(10): Error: Interface systems.vos.wisey.compiler.tests.IMyInterface is not defined\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(IRGenerationContextTest, getBoundControllerDeathTest) {
  mContext.addController(mController, 1);
  mContext.addInterface(mInterface, 3);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mContext.getBoundController(mInterface, 5));
  EXPECT_STREQ("/tmp/source.yz(5): Error: No controller is bound to interface systems.vos.wisey.compiler.tests.IMyInterface\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(IRGenerationContextTest, bindInterfaceToControllerTest) {
  mContext.addController(mController, 1);
  mContext.addInterface(mInterface, 3);
  
  mContext.bindInterfaceToController(mInterface, mController, 0);
  
  EXPECT_EQ(mContext.getBoundController(mInterface, 0), mController);
}

TEST_F(IRGenerationContextTest, bindInterfaceToControllerRepeatedlyDeathTest) {
  mContext.addController(mController, 1);
  mContext.addInterface(mInterface, 3);
  
  mContext.bindInterfaceToController(mInterface, mController, 0);

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mContext.bindInterfaceToController(mInterface, mController, 5));
  EXPECT_STREQ("/tmp/source.yz(5): Error: Interface systems.vos.wisey.compiler.tests.IMyInterface is already bound to systems.vos.wisey.compiler.tests.CMyController and can not be bound to systems.vos.wisey.compiler.tests.CMyController\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(IRGenerationContextTest, bindInterfaceToIncompatableControllerDeathTest) {
  string controllerFullName = "systems.vos.wisey.compiler.tests.CController";
  StructType* controllerStructType = StructType::create(mLLVMContext, controllerFullName);
  Controller* controller = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                                     controllerFullName,
                                                     controllerStructType,
                                                     mContext.getImportProfile(),
                                                     0);
  mContext.addController(controller, 1);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mContext.bindInterfaceToController(mInterface, controller, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Can not bind interface systems.vos.wisey.compiler.tests.IMyInterface to systems.vos.wisey.compiler.tests.CController because it does not implement the interface\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
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
  mContext.getScopes().setVariable(mContext, &mockVariable);
  mContext.getScopes().pushScope();
  mContext.getScopes().pushScope();

  EXPECT_EQ(mContext.getThis(), &mockVariable);
}

TEST_F(IRGenerationContextTest, printToStreamTest) {
  mContext.addLLVMStructType(mLLVMStructType, 0);
  mContext.setLLVMGlobalVariable(mLLVMStructType->getPointerType(mContext, 0), "myglobal");
  mContext.addInterface(mInterface, 0);
  mContext.addNode(mNode, 0);
  mContext.addController(mController, 0);
  mContext.addModel(mModel, 0);
  
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
               "external controller systems.vos.wisey.compiler.tests.CMyController\n"
               "  implements\n"
               "    systems.vos.wisey.compiler.tests.IMyInterface {\n"
               "}\n"
               "\n"
               "/* Nodes */\n"
               "\n"
               "external node systems.vos.wisey.compiler.tests.NMyNode {\n"
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
    vector<Type*> mainArguments;
    mainArguments.push_back(Type::getInt32Ty(llvmContext));
    mainArguments.push_back(Type::getInt8Ty(llvmContext)->getPointerTo()->getPointerTo());
    FunctionType* functionType =
      FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), mainArguments, false);
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
  GenericValue result = mContext.runCode(0, nullptr);
  
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
    "define internal i32 @main(i32, i8**) {\n" +
    "entry:\n" +
    "  ret i32 5\n" +
    "}\n";
  
  EXPECT_STREQ(expected.c_str(), stringBuffer.c_str());
}

TEST_F(IRGenerationContextTest, getArrayTypeTest) {
  wisey::ArrayType* arrayType = mContext.getArrayType(PrimitiveTypes::INT, 1u);
  
  EXPECT_EQ(arrayType, mContext.getArrayType(PrimitiveTypes::INT, 1u));
}

TEST_F(IRGenerationContextTest, getArrayExactTypeTest) {
  list<unsigned long> dimensions;
  dimensions.push_back(5);
  dimensions.push_back(3);
  ArrayExactType* arrayExactType = mContext.getArrayExactType(PrimitiveTypes::INT, dimensions);
  
  EXPECT_EQ(arrayExactType, mContext.getArrayExactType(PrimitiveTypes::INT, dimensions));
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
  argumentTypes.push_back(LLVMPrimitiveTypes::I64->getPointerType(mContext, 0));
  LLVMFunctionType* llvmFunctionType = mContext.getLLVMFunctionType(LLVMPrimitiveTypes::I8,
                                                                    argumentTypes);
  
  EXPECT_EQ(llvmFunctionType, mContext.getLLVMFunctionType(LLVMPrimitiveTypes::I8, argumentTypes));
}

TEST_F(IRGenerationContextTest, registerLLVMInternalFunctionNamedTypeTest) {
  vector<const IType*> arguments;
  arguments.push_back(LLVMPrimitiveTypes::I16);
  LLVMFunctionType* functionType = LLVMFunctionType::create(LLVMPrimitiveTypes::I8, arguments);
  mContext.registerLLVMInternalFunctionNamedType("myfunction", functionType, 0);
  
  EXPECT_EQ(functionType, mContext.lookupLLVMFunctionNamedType("myfunction", 0));
}

TEST_F(IRGenerationContextTest, registerLLVMExternalFunctionNamedTypeTest) {
  vector<const IType*> arguments;
  arguments.push_back(LLVMPrimitiveTypes::I16);
  LLVMFunctionType* functionType = LLVMFunctionType::create(LLVMPrimitiveTypes::I8, arguments);
  mContext.registerLLVMExternalFunctionNamedType("myfunction", functionType, 0);
  
  EXPECT_EQ(functionType, mContext.lookupLLVMFunctionNamedType("myfunction", 0));
}

TEST_F(IRGenerationContextTest, registerLLVMFunctionNamedTypeDeathTest) {
  vector<const IType*> arguments;
  arguments.push_back(LLVMPrimitiveTypes::I16);
  LLVMFunctionType* functionType = LLVMFunctionType::create(LLVMPrimitiveTypes::I8, arguments);
  mContext.registerLLVMExternalFunctionNamedType("myfunction", functionType, 0);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mContext.registerLLVMInternalFunctionNamedType("myfunction", functionType, 1));
  EXPECT_STREQ("/tmp/source.yz(1): Error: Can not register llvm function named myfunction because it is already registered\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(IRGenerationContextTest, lookupLLVMFunctionNamedTypeDeathTest) {
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mContext.lookupLLVMFunctionNamedType("myfunction", 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Can not find llvm function named myfunction\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, bindIncompatableControllerRunDeathTest) {
  expectFailCompile("tests/samples/test_bind_incompatable_controller.yz",
                    1,
                    "tests/samples/test_bind_incompatable_controller.yz\\(10\\): Error: Can not bind interface systems.vos.wisey.compiler.tests.IMyInterface to systems.vos.wisey.compiler.tests.CService because it does not implement the interface");
}
