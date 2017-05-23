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
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

struct InterfaceTypeSpecifierTest : public ::testing::Test {
  IRGenerationContext mContext;
  Interface* mInterface;
  vector<string> mPackage;

  InterfaceTypeSpecifierTest() {
    vector<Type*> types;
    string interfaceFullName = "systems.vos.wisey.compiler.tests.IShape";
    StructType* structType = StructType::create(mContext.getLLVMContext(), "IShape");
    structType->setBody(types);
    vector<MethodArgument*> methodArguments;
    vector<MethodSignature*> methodSignatures;
    vector<IType*> methodExceptions;
    MethodSignature* methodSignature = new MethodSignature("foo",
                                                           AccessLevel::PUBLIC_ACCESS,
                                                           PrimitiveTypes::INT_TYPE,
                                                           methodArguments,
                                                           methodExceptions,
                                                           0);
    methodSignatures.push_back(methodSignature);
    vector<Interface*> parentInterfaces;
    mInterface = new Interface("IShape",
                               interfaceFullName,
                               structType,
                               parentInterfaces,
                               methodSignatures);
   
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

TEST_F(InterfaceTypeSpecifierTest, interfaceTypeSpecifierNoImportDeathTest) {
  vector<string> package;
  InterfaceTypeSpecifier interfaceTypeSpecifier(package, "IObject");
  
  EXPECT_EXIT(interfaceTypeSpecifier.getType(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Could not find definition for IObject");
}

TEST_F(InterfaceTypeSpecifierTest, interfaceTypeSpecifierNotDefinedDeathTest) {
  InterfaceTypeSpecifier interfaceTypeSpecifier(mPackage, "IObject");
  
  EXPECT_EXIT(interfaceTypeSpecifier.getType(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Interface systems.vos.wisey.compiler.tests.IObject is not defined");
}
