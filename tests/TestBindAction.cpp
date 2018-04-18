//
//  TestBindAction.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/5/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
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
  InjectionArgument* mInjectionArgument;
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
                                         interfaceElements,
                                         0);
    
    string controllerFullName = "systems.vos.wisey.compiler.tests.CMyController";
    StructType* controllerStructType = StructType::create(mLLVMContext, controllerFullName);
    vector<IField*> controllerFields;
    vector<IMethod*> controllerMethods;
    vector<Interface*> controllerInterfaces;
    mController = Controller::newPublicController(controllerFullName, controllerStructType, 0);
    mController->setFields(controllerFields, 1u);
    mController->setMethods(controllerMethods);
    mController->setInterfaces(controllerInterfaces);
 
    mContext.addInterface(mInterface);
    mContext.addController(mController);

    InterfaceTypeSpecifier* interfaceTypeSpecifier =
      new InterfaceTypeSpecifier(NULL, "IMyInterface", 0);
    ControllerTypeSpecifier* controllerTypeSpecifier =
      new ControllerTypeSpecifier(NULL, "CMyController", 0);
    InjectionArgumentList injectionArgumentList;
    mInjectionArgument = new InjectionArgument("withField", NULL);
    injectionArgumentList.push_back(mInjectionArgument);
    mBindAction = new BindAction(interfaceTypeSpecifier,
                                 controllerTypeSpecifier,
                                 injectionArgumentList);
  }
  
  ~BindActionTest() { }
};

TEST_F(BindActionTest, getInterfaceTest) {
  EXPECT_EQ(mInterface, mBindAction->getInterface(mContext));
}

TEST_F(BindActionTest, getControllerTest) {
  EXPECT_EQ(mController, mBindAction->getController(mContext));
}

TEST_F(BindActionTest, getInjectionArgumentsTest) {
  InjectionArgumentList list = mBindAction->getInjectionArguments(mContext);
  
  EXPECT_EQ(1u, list.size());
  EXPECT_EQ(mInjectionArgument, list.front());
}
