//
//  TestControllerTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ControllerTypeSpecifier}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "wisey/ControllerTypeSpecifier.hpp"
#include "wisey/Method.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

struct ControllerTypeSpecifierTest : public ::testing::Test {
  IRGenerationContext mContext;
  Controller* mController;
  string mPackage = "systems.vos.wisey.compiler.tests";
  ImportProfile* mImportProfile;
  
  ControllerTypeSpecifierTest() {
    LLVMContext& llvmContext = mContext.getLLVMContext();
    mImportProfile = new ImportProfile(mPackage);
    mContext.setImportProfile(mImportProfile);
    
    vector<Type*> types;
    types.push_back(Type::getInt64Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    string controllerFullName = "systems.vos.wisey.compiler.tests.CMultiplier";
    StructType* structType = StructType::create(llvmContext, "CMultiplier");
    structType->setBody(types);
    vector<Field*> fields;
    InjectionArgumentList fieldArguments;
    fields.push_back(new Field(RECEIVED_FIELD,
                               PrimitiveTypes::INT_TYPE,
                               "left",
                               fieldArguments));
    fields.push_back(new Field(RECEIVED_FIELD,
                               PrimitiveTypes::INT_TYPE,
                               "right",
                                fieldArguments));
    vector<MethodArgument*> methodArguments;
    vector<IMethod*> methods;
    vector<const Model*> thrownExceptions;
    IMethod* multiplyMethod = new Method("multiply",
                                         AccessLevel::PUBLIC_ACCESS,
                                         PrimitiveTypes::INT_TYPE,
                                         methodArguments,
                                         thrownExceptions,
                                         NULL,
                                         0);
    methods.push_back(multiplyMethod);
    mController = Controller::newController(controllerFullName, structType);
    mController->setFields(fields, 1u);
    mController->setMethods(methods);
    mContext.addController(mController);
  }
};

TEST_F(ControllerTypeSpecifierTest, controllerTypeSpecifierCreateTest) {
  ControllerTypeSpecifier controllerTypeSpecifier("", "CMultiplier");
  
  EXPECT_EQ(controllerTypeSpecifier.getType(mContext), mController);
}

TEST_F(ControllerTypeSpecifierTest, controllerTypeSpecifierCreateWithPackageTest) {
  ControllerTypeSpecifier controllerTypeSpecifier(mPackage, "CMultiplier");
  
  EXPECT_EQ(controllerTypeSpecifier.getType(mContext), mController);
}

TEST_F(ControllerTypeSpecifierTest, printToStreamTest) {
  ControllerTypeSpecifier controllerTypeSpecifier(mPackage, "CMultiplier");

  stringstream stringStream;
  controllerTypeSpecifier.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("systems.vos.wisey.compiler.tests.CMultiplier", stringStream.str().c_str());
}

TEST_F(ControllerTypeSpecifierTest, controllerTypeSpecifierSamePackageDeathTest) {
  ControllerTypeSpecifier controllerTypeSpecifier("", "CAdder");
  
  EXPECT_EXIT(controllerTypeSpecifier.getType(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Controller systems.vos.wisey.compiler.tests.CAdder is not defined");
}

TEST_F(ControllerTypeSpecifierTest, controllerTypeSpecifierNotDefinedDeathTest) {
  ControllerTypeSpecifier controllerTypeSpecifier(mPackage, "CAdder");
  
  EXPECT_EXIT(controllerTypeSpecifier.getType(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Controller systems.vos.wisey.compiler.tests.CAdder is not defined");
}
