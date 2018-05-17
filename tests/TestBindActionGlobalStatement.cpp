//
//  TestBindActionGlobalStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link BindActionGlobalStatement}
//

#include <gtest/gtest.h>

#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/BindActionGlobalStatement.hpp"
#include "wisey/ControllerTypeSpecifier.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct BindActionGlobalStatementTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Interface* mInterface;
  Controller* mController;
  BindActionGlobalStatement* mBindActionGlobalStatement;
  ImportProfile* mImportProfile;
  string mPackage = "systems.vos.wisey.compiler.tests";
  
  BindActionGlobalStatementTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);

    string interfaceFullName = "systems.vos.wisey.compiler.tests.IMyInterface";
    StructType* interfaceStructType = StructType::create(mLLVMContext, interfaceFullName);
    vector<IObjectElementDefinition*> interfaceElements;
    vector<IInterfaceTypeSpecifier*> parentInterfaces;
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
    mController = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                            controllerFullName,
                                            controllerStructType,
                                            mContext.getImportProfile(),
                                            0);
    mController->setFields(mContext, controllerFields, 1u);
    mController->setMethods(controllerMethods);
    mController->setInterfaces(controllerInterfaces);
    
    InterfaceTypeSpecifier* interfaceTypeSpecifier =
      new InterfaceTypeSpecifier(NULL, "IMyInterface", 3);
    ControllerTypeSpecifier* controllerTypeSpecifier =
      new ControllerTypeSpecifier(NULL, "CMyController", 1);
    InjectionArgumentList injectionArgumentList;
    BindAction* bindAction = new BindAction(interfaceTypeSpecifier,
                                            controllerTypeSpecifier,
                                            injectionArgumentList);
    mBindActionGlobalStatement = new BindActionGlobalStatement(bindAction, 3);
}
  
  ~BindActionGlobalStatementTest() { }
};

TEST_F(BindActionGlobalStatementTest, bindInterfaceToControllerMissingControllerDeathTest) {
  mBindActionGlobalStatement->prototypeObject(mContext, mContext.getImportProfile());
  mContext.addInterface(mInterface, 1);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(mBindActionGlobalStatement->generateIR(mContext));
  EXPECT_STREQ("/tmp/source.yz(1): Error: Controller systems.vos.wisey.compiler.tests.CMyController is not defined\n",
              buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(BindActionGlobalStatementTest, bindControllerToInterfaceMissingInterfaceDeathTest) {
  mBindActionGlobalStatement->prototypeObject(mContext, mContext.getImportProfile());
  mContext.addController(mController, 3);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(mBindActionGlobalStatement->generateIR(mContext));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Interface systems.vos.wisey.compiler.tests.IMyInterface is not defined\n",
              buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(BindActionGlobalStatementTest, prototypeObjectTest) {
  mContext.addController(mController, 0);
  mContext.addInterface(mInterface, 0);

  mBindActionGlobalStatement->prototypeObject(mContext, mContext.getImportProfile());
  
  EXPECT_EQ(mContext.getBoundController(mInterface, 0), mController);
}

TEST_F(BindActionGlobalStatementTest, bindInterfaceToIncompatableControllerDeathTest) {
  mContext.addController(mController, 0);
  mContext.addInterface(mInterface, 0);

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mBindActionGlobalStatement->generateIR(mContext));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Can not bind interface systems.vos.wisey.compiler.tests.IMyInterface to systems.vos.wisey.compiler.tests.CMyController because it does not implement the interface\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, injectInterfaceWithArgumentsRunTest) {
  runFile("tests/samples/test_inject_interface_with_arguments.yz", "7");
}

TEST_F(TestFileRunner, bindIncompatableControllerRunDeathTest) {
  expectFailCompile("tests/samples/test_bind_incompatable_controller.yz",
                    1,
                    "tests/samples/test_bind_incompatable_controller.yz\\(10\\): Error: Can not bind interface systems.vos.wisey.compiler.tests.IMyInterface to systems.vos.wisey.compiler.tests.CService because it does not implement the interface");
}

TEST_F(TestFileRunner, bindExternalInterfaceToControllerWithReceivedFieldsRunDeathTest) {
  expectFailCompile("tests/samples/test_bind_external_interface_to_controller_with_receieved_fields.yz",
                    1,
                    "tests/samples/test_bind_external_interface_to_controller_with_receieved_fields.yz\\(25\\): Error: "
                    "Can not bind external interface systems.vos.wisey.compiler.tests.IMyInterface to systems.vos.wisey.compiler.tests.CService because it receives arguments");
}
