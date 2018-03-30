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
#include "wisey/FakeExpression.hpp"
#include "wisey/Method.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ReceivedField.hpp"

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
    types.push_back(FunctionType::get(Type::getInt32Ty(llvmContext), true)
                    ->getPointerTo()->getPointerTo());
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    string controllerFullName = "systems.vos.wisey.compiler.tests.CMultiplier";
    StructType* structType = StructType::create(llvmContext, "CMultiplier");
    structType->setBody(types);
    vector<IField*> fields;
    fields.push_back(new ReceivedField(PrimitiveTypes::INT_TYPE, "left"));
    fields.push_back(new ReceivedField(PrimitiveTypes::INT_TYPE, "right"));
    vector<MethodArgument*> methodArguments;
    vector<IMethod*> methods;
    vector<const Model*> thrownExceptions;
    IMethod* multiplyMethod = new Method(mController,
                                         "multiply",
                                         AccessLevel::PUBLIC_ACCESS,
                                         PrimitiveTypes::INT_TYPE,
                                         methodArguments,
                                         thrownExceptions,
                                         NULL,
                                         0);
    methods.push_back(multiplyMethod);
    mController = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                            controllerFullName,
                                            structType);
    mController->setFields(fields, 2u);
    mController->setMethods(methods);
    mContext.addController(mController);
  }
};

TEST_F(ControllerTypeSpecifierTest, controllerTypeSpecifierCreateTest) {
  ControllerTypeSpecifier controllerTypeSpecifier(NULL, "CMultiplier");
  
  EXPECT_EQ(controllerTypeSpecifier.getType(mContext), mController);
}

TEST_F(ControllerTypeSpecifierTest, controllerTypeSpecifierCreateWithPackageTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ControllerTypeSpecifier controllerTypeSpecifier(packageExpression, "CMultiplier");
  
  EXPECT_EQ(controllerTypeSpecifier.getType(mContext), mController);
}

TEST_F(ControllerTypeSpecifierTest, printToStreamTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ControllerTypeSpecifier controllerTypeSpecifier(packageExpression, "CMultiplier");

  stringstream stringStream;
  controllerTypeSpecifier.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("systems.vos.wisey.compiler.tests.CMultiplier", stringStream.str().c_str());
}

TEST_F(ControllerTypeSpecifierTest, controllerTypeSpecifierSamePackageDeathTest) {
  ControllerTypeSpecifier controllerTypeSpecifier(NULL, "CAdder");
  
  EXPECT_EXIT(controllerTypeSpecifier.getType(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Controller systems.vos.wisey.compiler.tests.CAdder is not defined");
}

TEST_F(ControllerTypeSpecifierTest, controllerTypeSpecifierNotDefinedDeathTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ControllerTypeSpecifier controllerTypeSpecifier(packageExpression, "CAdder");
  
  EXPECT_EXIT(controllerTypeSpecifier.getType(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Controller systems.vos.wisey.compiler.tests.CAdder is not defined");
}
