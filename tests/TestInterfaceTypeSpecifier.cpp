//
//  TestInterfaceTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/7/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link InterfaceTypeSpecifier}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "wisey/FakeExpression.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/MethodSignature.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

struct InterfaceTypeSpecifierTest : public ::testing::Test {
  IRGenerationContext mContext;
  Interface* mInterface;
  string mPackage = "systems.vos.wisey.compiler.tests";
  ImportProfile* mImportProfile;

  InterfaceTypeSpecifierTest() {
    mImportProfile = new ImportProfile(mPackage);
    mContext.setImportProfile(mImportProfile);
    
    vector<Type*> types;
    string interfaceFullName = "systems.vos.wisey.compiler.tests.IShape";
    StructType* structType = StructType::create(mContext.getLLVMContext(), "IShape");
    structType->setBody(types);
    VariableList methodArguments;
    vector<IObjectElementDefinition*> objectElements;
    vector<IModelTypeSpecifier*> methodExceptions;
    const PrimitiveTypeSpecifier* intSpecifier = PrimitiveTypes::INT_TYPE->newTypeSpecifier();
    IObjectElementDefinition* methodSignature = new MethodSignatureDeclaration(intSpecifier,
                                                                                "foo",
                                                                                methodArguments,
                                                                                methodExceptions);
    objectElements.push_back(methodSignature);
    vector<IInterfaceTypeSpecifier*> parentInterfaces;
    mInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                         interfaceFullName,
                                         structType,
                                         parentInterfaces,
                                         objectElements);
   
    mContext.addInterface(mInterface);
  }
};

TEST_F(InterfaceTypeSpecifierTest, interfaceTypeSpecifierCreateTest) {
  InterfaceTypeSpecifier interfaceTypeSpecifier(NULL, "IShape");
  
  EXPECT_EQ(interfaceTypeSpecifier.getType(mContext), mInterface);
}

TEST_F(InterfaceTypeSpecifierTest, interfaceTypeSpecifierCreateWithPackageTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  InterfaceTypeSpecifier interfaceTypeSpecifier(packageExpression, "IShape");
  
  EXPECT_EQ(interfaceTypeSpecifier.getType(mContext), mInterface);
}

TEST_F(InterfaceTypeSpecifierTest, printToStreamTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  InterfaceTypeSpecifier interfaceTypeSpecifier(packageExpression, "IShape");

  stringstream stringStream;
  interfaceTypeSpecifier.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("systems.vos.wisey.compiler.tests.IShape", stringStream.str().c_str());
}

TEST_F(InterfaceTypeSpecifierTest, interfaceTypeSpecifierSamePackageDeathTest) {
  InterfaceTypeSpecifier interfaceTypeSpecifier(NULL, "IObject");
  
  EXPECT_EXIT(interfaceTypeSpecifier.getType(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Interface systems.vos.wisey.compiler.tests.IObject is not defined");
}

TEST_F(InterfaceTypeSpecifierTest, interfaceTypeSpecifierNotDefinedDeathTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  InterfaceTypeSpecifier interfaceTypeSpecifier(packageExpression, "IObject");
  
  EXPECT_EXIT(interfaceTypeSpecifier.getType(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Interface systems.vos.wisey.compiler.tests.IObject is not defined");
}
