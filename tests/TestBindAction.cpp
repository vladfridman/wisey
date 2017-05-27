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
  
  BindActionTest() : mLLVMContext(mContext.getLLVMContext()) {
    string interfaceFullName = "systems.vos.wisey.compiler.tests.IMyInterface";
    StructType* interfaceStructType = StructType::create(mLLVMContext, interfaceFullName);
    mInterface = new Interface(interfaceFullName, interfaceStructType);
    
    string controllerFullName = "systems.vos.wisey.compiler.tests.CMyController";
    StructType* controllerStructType = StructType::create(mLLVMContext, controllerFullName);
    vector<Field*> controllerReceivedFields;
    vector<Field*> controllerInjectedFields;
    vector<Field*> controllerStateFields;
    vector<Method*> controllerMethods;
    vector<Interface*> controllerInterfaces;
    mController = new Controller(controllerFullName, controllerStructType);
    mController->setFields(controllerReceivedFields,
                           controllerInjectedFields,
                           controllerStateFields);
    mController->setMethods(controllerMethods);
    mController->setInterfaces(controllerInterfaces);
    
    vector<string> package;
    ControllerTypeSpecifier* controllerTypeSpecifier =
      new ControllerTypeSpecifier(package, "CMyController");
    InterfaceTypeSpecifier* interfaceTypeSpecifier =
      new InterfaceTypeSpecifier(package, "IMyInterface");
    mBindAction = new BindAction(controllerTypeSpecifier, interfaceTypeSpecifier);
}
  
  ~BindActionTest() { }
};

TEST_F(BindActionTest, bindInterfaceToControllerMissingControllerDeathTest) {
  mContext.addInterface(mInterface);
  
  EXPECT_EXIT(mBindAction->generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Could not find definition for CMyController");
}

TEST_F(BindActionTest, bindControllerToInterfaceMissingInterfaceDeathTest) {
  mContext.addController(mController);
  
  EXPECT_EXIT(mBindAction->generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Could not find definition for IMyInterface");
}

TEST_F(BindActionTest, generateIRTest) {
  mContext.addController(mController);
  mContext.addInterface(mInterface);

  mBindAction->generateIR(mContext);
  
  EXPECT_EQ(mContext.getBoundController(mInterface), mController);
}
