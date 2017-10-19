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
  vector<string> mPackage;

  InterfaceTypeSpecifierTest() {
    mContext.setPackage("systems.vos.wisey.compiler.tests");
    
    vector<Type*> types;
    string interfaceFullName = "systems.vos.wisey.compiler.tests.IShape";
    StructType* structType = StructType::create(mContext.getLLVMContext(), "IShape");
    structType->setBody(types);
    VariableList methodArguments;
    vector<IObjectElementDeclaration*> objectElements;
    vector<ModelTypeSpecifier*> methodExceptions;
    PrimitiveTypeSpecifier* intSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
    IObjectElementDeclaration* methodSignature = new MethodSignatureDeclaration(intSpecifier,
                                                                                "foo",
                                                                                methodArguments,
                                                                                methodExceptions);
    objectElements.push_back(methodSignature);
    vector<InterfaceTypeSpecifier*> parentInterfaces;
    mInterface = Interface::newInterface(interfaceFullName,
                                         structType,
                                         parentInterfaces,
                                         objectElements);
   
    mContext.addInterface(mInterface);

    mPackage.push_back("systems");
    mPackage.push_back("vos");
    mPackage.push_back("wisey");
    mPackage.push_back("compiler");
    mPackage.push_back("tests");
  }
};

TEST_F(InterfaceTypeSpecifierTest, interfaceTypeSpecifierCreateTest) {
  vector<string> package;
  InterfaceTypeSpecifier interfaceTypeSpecifier(package, "IShape");
  
  EXPECT_EQ(interfaceTypeSpecifier.getType(mContext), mInterface);
}

TEST_F(InterfaceTypeSpecifierTest, interfaceTypeSpecifierCreateWithPackageTest) {
  InterfaceTypeSpecifier interfaceTypeSpecifier(mPackage, "IShape");
  
  EXPECT_EQ(interfaceTypeSpecifier.getType(mContext), mInterface);
}

TEST_F(InterfaceTypeSpecifierTest, printToStreamTest) {
  InterfaceTypeSpecifier interfaceTypeSpecifier(mPackage, "IShape");

  stringstream stringStream;
  interfaceTypeSpecifier.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("systems.vos.wisey.compiler.tests.IShape", stringStream.str().c_str());
}

TEST_F(InterfaceTypeSpecifierTest, interfaceTypeSpecifierSamePackageDeathTest) {
  vector<string> package;
  InterfaceTypeSpecifier interfaceTypeSpecifier(package, "IObject");
  
  EXPECT_EXIT(interfaceTypeSpecifier.getType(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Interface systems.vos.wisey.compiler.tests.IObject is not defined");
}

TEST_F(InterfaceTypeSpecifierTest, interfaceTypeSpecifierNotDefinedDeathTest) {
  InterfaceTypeSpecifier interfaceTypeSpecifier(mPackage, "IObject");
  
  EXPECT_EXIT(interfaceTypeSpecifier.getType(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Interface systems.vos.wisey.compiler.tests.IObject is not defined");
}
