//
//  TestBindAction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link BindAction}
//

#include <gtest/gtest.h>

#include "wisey/BindAction.hpp"
#include "wisey/ControllerTypeSpecifier.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct BindActionTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Interface* mInterface;
  Controller* mController;
  BindAction* mBindAction;
  ImportProfile* mImportProfile;
  string mPackage = "systems.vos.wisey.compiler.tests";
  
  BindActionTest() : mLLVMContext(mContext.getLLVMContext()) {
    mImportProfile = new ImportProfile(mPackage);
    mContext.setImportProfile(mImportProfile);

    string interfaceFullName = "systems.vos.wisey.compiler.tests.IMyInterface";
    StructType* interfaceStructType = StructType::create(mLLVMContext, interfaceFullName);
    vector<IObjectElementDefinition*> interfaceElements;
    vector<IInterfaceTypeSpecifier*> parentInterfaces;
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
    
    InterfaceTypeSpecifier* interfaceTypeSpecifier =
    new InterfaceTypeSpecifier(NULL, "IMyInterface");
    ControllerTypeSpecifier* controllerTypeSpecifier =
      new ControllerTypeSpecifier(NULL, "CMyController");
    mBindAction = new BindAction(interfaceTypeSpecifier, controllerTypeSpecifier);
}
  
  ~BindActionTest() { }
};

TEST_F(BindActionTest, bindInterfaceToControllerMissingControllerDeathTest) {
  mContext.addInterface(mInterface);
  
  EXPECT_EXIT(mBindAction->prototypeMethods(mContext),
              ::testing::ExitedWithCode(1),
              "Controller systems.vos.wisey.compiler.tests.CMyController is not defined");
}

TEST_F(BindActionTest, bindControllerToInterfaceMissingInterfaceDeathTest) {
  mContext.addController(mController);
  
  EXPECT_EXIT(mBindAction->prototypeMethods(mContext),
              ::testing::ExitedWithCode(1),
              "Interface systems.vos.wisey.compiler.tests.IMyInterface is not defined");
}

TEST_F(BindActionTest, generateIRTest) {
  mContext.addController(mController);
  mContext.addInterface(mInterface);

  mBindAction->prototypeMethods(mContext);
  
  EXPECT_EQ(mContext.getBoundController(mInterface), mController);
}

