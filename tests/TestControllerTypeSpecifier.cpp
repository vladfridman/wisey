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
  vector<string> mPackage;
  
  ControllerTypeSpecifierTest() {
    LLVMContext& llvmContext = mContext.getLLVMContext();
    mContext.setPackage("systems.vos.wisey.compiler.tests");
    
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    string controllerFullName = "systems.vos.wisey.compiler.tests.CMultiplier";
    StructType* structType = StructType::create(llvmContext, "CMultiplier");
    structType->setBody(types);
    vector<Field*> fields;
    ExpressionList fieldArguments;
    fields.push_back(new Field(RECEIVED_FIELD,
                               PrimitiveTypes::INT_TYPE,
                               "left",
                               0,
                               fieldArguments));
    fields.push_back(new Field(RECEIVED_FIELD,
                               PrimitiveTypes::INT_TYPE,
                               "right",
                               1,
                               fieldArguments));
    vector<MethodArgument*> methodArguments;
    vector<IMethod*> methods;
    vector<const Model*> thrownExceptions;
    IMethod* multiplyMethod = new Method("multiply",
                                         AccessLevel::PUBLIC_ACCESS,
                                         PrimitiveTypes::INT_TYPE,
                                         methodArguments,
                                         thrownExceptions,
                                         NULL);
    methods.push_back(multiplyMethod);
    mController = new Controller(controllerFullName, structType);
    mController->setFields(fields);
    mController->setMethods(methods);
    mContext.addController(mController);

    mPackage.push_back("systems");
    mPackage.push_back("vos");
    mPackage.push_back("wisey");
    mPackage.push_back("compiler");
    mPackage.push_back("tests");
  }
};

TEST_F(ControllerTypeSpecifierTest, controllerTypeSpecifierCreateTest) {
  vector<string> package;
  ControllerTypeSpecifier controllerTypeSpecifier(package, "CMultiplier");
  
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
  vector<string> package;
  ControllerTypeSpecifier controllerTypeSpecifier(package, "CAdder");
  
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
