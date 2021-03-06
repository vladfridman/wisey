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
#include "BindActionGlobalStatement.hpp"
#include "ControllerTypeSpecifier.hpp"
#include "InterfaceTypeSpecifier.hpp"
#include "IRGenerationContext.hpp"

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
  runFile("tests/samples/test_inject_interface_with_arguments.yz", 7);
}

TEST_F(TestFileRunner, injectInterfaceWithArgumentsInlineRunTest) {
  runFile("tests/samples/test_inject_interface_with_arguments_inline.yz", 7);
}

TEST_F(TestFileRunner, injectInterfaceReferenceBoundRunTest) {
  runFile("tests/samples/test_inject_interface_reference_bound.yz", 3);
}

TEST_F(TestFileRunner, injectInterfaceReferenceBoundInlineRunTest) {
  runFile("tests/samples/test_inject_interface_reference_bound_inline.yz", 5);
}

TEST_F(TestFileRunner, injectInterfaceOwnerBoundToScopedControllerRunDeathTest) {
  expectFailCompile("tests/samples/test_inject_interface_owner_bound_to_scoped_controller.yz",
                    1,
                    "tests/samples/test_inject_interface_owner_bound_to_scoped_controller.yz\\(18\\): Error: "
                    "Incompatible types: can not cast from type systems.vos.wisey.compiler.tests.IMyInterface to systems.vos.wisey.compiler.tests.IMyInterface\\*");
}

TEST_F(TestFileRunner, injectInterfaceWithMissingArgumentsRunDeathTest) {
  expectFailCompile("tests/samples/test_inject_interface_with_missing_arguments.yz",
                    1,
                    "tests/samples/test_inject_interface_with_missing_arguments.yz\\(18\\): Error: "
                    "Received field mValue is not initialized");
}

TEST_F(TestFileRunner, injectInterfaceWithMissingArgumentsInlineRunDeathTest) {
  expectFailCompile("tests/samples/test_inject_interface_with_missing_arguments_inline.yz",
                    1,
                    "tests/samples/test_inject_interface_with_missing_arguments_inline.yz\\(20\\): Error: "
                    "Received field mValue is not initialized");
}

TEST_F(TestFileRunner, bindIncompatableControllerRunDeathTest) {
  expectFailCompile("tests/samples/test_bind_incompatable_controller.yz",
                    1,
                    "tests/samples/test_bind_incompatable_controller.yz\\(10\\): Error: "
                    "Can not bind interface systems.vos.wisey.compiler.tests.IMyInterface to systems.vos.wisey.compiler.tests.CService because it does not implement the interface");
}

TEST_F(TestFileRunner, bindExternalInterfaceToControllerWithReceivedFieldsRunDeathTest) {
  expectFailCompile("tests/samples/test_bind_external_interface_to_controller_with_receieved_fields.yz",
                    1,
                    "tests/samples/test_bind_external_interface_to_controller_with_receieved_fields.yz\\(25\\): Error: "
                    "Can not bind external interface systems.vos.wisey.compiler.tests.IMyInterface to systems.vos.wisey.compiler.tests.CService because it receives arguments");
}

TEST_F(TestFileRunner, bindExternalInterfaceToScopeInjectedControllerRunDeathTest) {
  expectFailCompile("tests/samples/test_bind_external_interface_to_scope_injected_controller.yz",
                    1,
                    "tests/samples/test_bind_external_interface_to_scope_injected_controller.yz\\(23\\): Error: "
                    "Can not bind external interface systems.vos.wisey.compiler.tests.IMyInterface to systems.vos.wisey.compiler.tests.CService because it is scope injected");
}

TEST_F(TestFileRunner, injectInterfaceOwnerNotBoundRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_inject_interface_owner_not_bound.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MInterfaceNotBoundException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_inject_interface_owner_not_bound.yz:13)\n"
                               "Details: Interface systems.vos.wisey.compiler.tests.IMyInterface is not bound to a controller\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, injectInterfaceReferenceNotBoundRunDeathTest) {
  expectFailCompile("tests/samples/test_inject_interface_reference_not_bound.yz",
                    1,
                    "tests/samples/test_inject_interface_reference_not_bound.yz\\(10\\): Error: "
                    "Injecting unbound interface reference is not allowed, either inject an owner or bind the interface");
}

TEST_F(TestFileRunner, injectInterfaceReferenceNotBoundInlineRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_inject_interface_reference_not_bound_inline.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MInterfaceNotBoundException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_inject_interface_reference_not_bound_inline.yz:11)\n"
                               "Details: Interface systems.vos.wisey.compiler.tests.IMyInterface is not bound to a controller\n"
                               "Main thread ended without a result\n");
}
