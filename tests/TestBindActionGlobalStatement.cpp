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
    mBindActionGlobalStatement = new BindActionGlobalStatement(bindAction, 0);
}
  
  ~BindActionGlobalStatementTest() { }
};

TEST_F(BindActionGlobalStatementTest, bindInterfaceToControllerMissingControllerDeathTest) {
  mContext.addInterface(mInterface, 1);
  
  EXPECT_EXIT(mBindActionGlobalStatement->prototypeMethods(mContext),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(1\\): Error: Controller systems.vos.wisey.compiler.tests.CMyController is not defined");
}

TEST_F(BindActionGlobalStatementTest, bindControllerToInterfaceMissingInterfaceDeathTest) {
  mContext.addController(mController, 3);
  
  EXPECT_EXIT(mBindActionGlobalStatement->prototypeMethods(mContext),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(3\\): Error: Interface systems.vos.wisey.compiler.tests.IMyInterface is not defined");
}

TEST_F(BindActionGlobalStatementTest, generateIRTest) {
  mContext.addController(mController, 0);
  mContext.addInterface(mInterface, 0);

  mBindActionGlobalStatement->prototypeMethods(mContext);
  
  EXPECT_EQ(mContext.getBoundController(mInterface, 0), mController);
}

