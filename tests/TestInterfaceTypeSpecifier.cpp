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

#include "TestPrefix.hpp"
#include "FakeExpression.hpp"
#include "InterfaceTypeSpecifier.hpp"
#include "MethodSignature.hpp"
#include "MethodSignatureDeclaration.hpp"
#include "PrimitiveTypes.hpp"
#include "PrimitiveTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

struct InterfaceTypeSpecifierTest : public ::testing::Test {
  IRGenerationContext mContext;
  Interface* mInterface;
  string mPackage = "systems.vos.wisey.compiler.tests";
  ImportProfile* mImportProfile;

  InterfaceTypeSpecifierTest() {
    TestPrefix::generateIR(mContext);
    
    vector<Type*> types;
    string interfaceFullName = "systems.vos.wisey.compiler.tests.IShape";
    StructType* structType = StructType::create(mContext.getLLVMContext(), "IShape");
    structType->setBody(types);
    VariableList methodArguments;
    vector<IObjectElementDefinition*> objectElements;
    vector<IModelTypeSpecifier*> methodExceptions;
    const PrimitiveTypeSpecifier* intSpecifier = PrimitiveTypes::INT->newTypeSpecifier(0);
    IObjectElementDefinition* methodSignature =
    new MethodSignatureDeclaration(intSpecifier,
                                   "foo",
                                   methodArguments,
                                   methodExceptions,
                                   new MethodQualifiers(0),
                                   0);
    objectElements.push_back(methodSignature);
    vector<IInterfaceTypeSpecifier*> parentInterfaces;
    mInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                         interfaceFullName,
                                         structType,
                                         parentInterfaces,
                                         objectElements,
                                         mContext.getImportProfile(),
                                         0);
   
    mContext.addInterface(mInterface, 0);
  }
};

TEST_F(InterfaceTypeSpecifierTest, interfaceTypeSpecifierCreateTest) {
  InterfaceTypeSpecifier interfaceTypeSpecifier(NULL, "IShape", 7);
  
  EXPECT_EQ(mInterface, interfaceTypeSpecifier.getType(mContext));
  EXPECT_EQ(7, interfaceTypeSpecifier.getLine());
}

TEST_F(InterfaceTypeSpecifierTest, interfaceTypeSpecifierCreateWithPackageTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  InterfaceTypeSpecifier interfaceTypeSpecifier(packageExpression, "IShape", 0);
  
  EXPECT_EQ(mInterface, interfaceTypeSpecifier.getType(mContext));
}

TEST_F(InterfaceTypeSpecifierTest, printToStreamTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  InterfaceTypeSpecifier interfaceTypeSpecifier(packageExpression, "IShape", 0);

  stringstream stringStream;
  interfaceTypeSpecifier.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("systems.vos.wisey.compiler.tests.IShape", stringStream.str().c_str());
}

TEST_F(InterfaceTypeSpecifierTest, interfaceTypeSpecifierSamePackageDeathTest) {
  InterfaceTypeSpecifier interfaceTypeSpecifier(NULL, "IObject", 3);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(interfaceTypeSpecifier.getType(mContext));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Interface systems.vos.wisey.compiler.tests.IObject is not defined\n",
              buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(InterfaceTypeSpecifierTest, interfaceTypeSpecifierNotDefinedDeathTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  InterfaceTypeSpecifier interfaceTypeSpecifier(packageExpression, "IObject", 1);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(interfaceTypeSpecifier.getType(mContext));
  EXPECT_STREQ("/tmp/source.yz(1): Error: Interface systems.vos.wisey.compiler.tests.IObject is not defined\n",
              buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}
