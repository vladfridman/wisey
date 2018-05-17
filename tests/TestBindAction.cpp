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
  InterfaceTypeSpecifier* mInterfaceTypeSpecifier;
  ControllerTypeSpecifier* mControllerTypeSpecifier;

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
 
    mContext.addInterface(mInterface, 0);
    mContext.addController(mController, 0);

    mInterfaceTypeSpecifier = new InterfaceTypeSpecifier(NULL, "IMyInterface", 0);
    mControllerTypeSpecifier = new ControllerTypeSpecifier(NULL, "CMyController", 0);
    InjectionArgumentList injectionArgumentList;
    mInjectionArgument = new InjectionArgument("withField", NULL);
    injectionArgumentList.push_back(mInjectionArgument);
    mBindAction = new BindAction(mInterfaceTypeSpecifier,
                                 mControllerTypeSpecifier,
                                 injectionArgumentList);
  }
  
  ~BindActionTest() { }
};

TEST_F(BindActionTest, getInterfaceTest) {
  EXPECT_EQ(mInterfaceTypeSpecifier, mBindAction->getInterface());
}

TEST_F(BindActionTest, getControllerTest) {
  EXPECT_EQ(mControllerTypeSpecifier, mBindAction->getController());
}

TEST_F(BindActionTest, getInjectionArgumentsTest) {
  InjectionArgumentList list = mBindAction->getInjectionArguments();
  
  EXPECT_EQ(1u, list.size());
  EXPECT_EQ(mInjectionArgument, list.front());
}
