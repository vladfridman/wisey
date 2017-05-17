//
//  TestBindAction.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 5/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link BindAction}
//

#include <gtest/gtest.h>

#include "wisey/BindAction.hpp"
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
    StructType* interfaceStructType = StructType::create(mLLVMContext, "IMyInterface");
    vector<Interface*> interfaceParentInterfaces;
    vector<MethodSignature*> interfaceMethodSignatures;
    mInterface = new Interface("IMyInterface",
                               interfaceStructType,
                               interfaceParentInterfaces,
                               interfaceMethodSignatures);
    
    StructType* controllerStructType = StructType::create(mLLVMContext, "CMyController");
    vector<Field*> controllerReceivedFields;
    vector<Field*> controllerInjectedFields;
    vector<Field*> controllerStateFields;
    vector<Method*> controllerMethods;
    vector<Interface*> controllerInterfaces;
    mController = new Controller("CMyController",
                                 controllerStructType,
                                 controllerReceivedFields,
                                 controllerInjectedFields,
                                 controllerStateFields,
                                 controllerMethods,
                                 controllerInterfaces);
    
    mBindAction = new BindAction("CMyController", "IMyInterface");
}
  
  ~BindActionTest() { }
};

TEST_F(BindActionTest, bindInterfaceToControllerMissingControllerDeathTest) {
  mContext.addInterface(mInterface);
  
  EXPECT_EXIT(mBindAction->generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Controller CMyController is not defined");
}

TEST_F(BindActionTest, bindControllerToInterfaceMissingInterfaceDeathTest) {
  mContext.addController(mController);
  
  EXPECT_EXIT(mBindAction->generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Interface IMyInterface is not defined");
}

TEST_F(BindActionTest, generateIRTest) {
  mContext.addController(mController);
  mContext.addInterface(mInterface);

  mBindAction->generateIR(mContext);
  
  EXPECT_EQ(mContext.getBoundController(mInterface), mController);
}
