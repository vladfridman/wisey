//
//  TestExternalControllerDefinition.cpp
//  runtests
//
//  Created by Vladimir Fridman on 9/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ExternalControllerDefinition}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "AccessLevel.hpp"
#include "Argument.hpp"
#include "FakeExpression.hpp"
#include "ExternalControllerDefinition.hpp"
#include "ExternalMethodDefinition.hpp"
#include "FloatConstant.hpp"
#include "Interface.hpp"
#include "InterfaceTypeSpecifier.hpp"
#include "IModelTypeSpecifier.hpp"
#include "IRGenerationContext.hpp"
#include "PrimitiveTypes.hpp"
#include "PrimitiveTypeSpecifier.hpp"
#include "ReceivedFieldDefinition.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct ExternalControllerDefinitionTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  vector<IObjectElementDefinition*> mElementDeclarations;
  vector<IInterfaceTypeSpecifier*> mInterfaces;
  string mPackage = "systems.vos.wisey.compiler.tests";
  
  ExternalControllerDefinitionTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    ExternalMethodDefinition* methodDeclaration;
    
    const PrimitiveTypeSpecifier* intTypeSpecifier = PrimitiveTypes::INT->newTypeSpecifier(0);
    const PrimitiveTypeSpecifier* floatTypeSpecifier =
    PrimitiveTypes::FLOAT->newTypeSpecifier(0);
    Identifier* intArgumentIdentifier = new Identifier("intargument", 0);
    VariableDeclaration* intArgument =
    VariableDeclaration::create(intTypeSpecifier, intArgumentIdentifier, 0);
    VariableList methodArguments;
    methodArguments.push_back(intArgument);
    vector<IModelTypeSpecifier*> thrownExceptions;
    methodDeclaration = new ExternalMethodDefinition(floatTypeSpecifier,
                                                     "foo",
                                                     methodArguments,
                                                     thrownExceptions,
                                                     new MethodQualifiers(0),
                                                     0);

    const PrimitiveTypeSpecifier* longType = PrimitiveTypes::LONG->newTypeSpecifier(0);
    const PrimitiveTypeSpecifier* floatType = PrimitiveTypes::FLOAT->newTypeSpecifier(0);
    InjectionArgumentList arguments;
    ReceivedFieldDefinition* field1 = ReceivedFieldDefinition::create(longType, "mField1", 0);
    ReceivedFieldDefinition* field2 = ReceivedFieldDefinition::create(floatType, "mField2", 0);
    mElementDeclarations.push_back(field1);
    mElementDeclarations.push_back(field2);
    mElementDeclarations.push_back(methodDeclaration);
  }
  
  ~ExternalControllerDefinitionTest() {
  }
};

TEST_F(ExternalControllerDefinitionTest, prototypeObjectTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ControllerTypeSpecifierFull* typeSpecifier = new ControllerTypeSpecifierFull(packageExpression,
                                                                               "CMyController",
                                                                               0);
  vector<IObjectDefinition*> innerObjectDefinitions;
  ExternalControllerDefinition controllerDefinition(typeSpecifier,
                                                    mElementDeclarations,
                                                    mInterfaces,
                                                    innerObjectDefinitions,
                                                    NULL,
                                                    0);

  controllerDefinition.prototypeObject(mContext, mContext.getImportProfile());
  
  ASSERT_NE(mContext.getController("systems.vos.wisey.compiler.tests.CMyController", 0), nullptr);
  
  Controller* controller =
  mContext.getController("systems.vos.wisey.compiler.tests.CMyController", 0);
  
  EXPECT_STREQ(controller->getShortName().c_str(), "CMyController");
  EXPECT_STREQ(controller->getTypeName().c_str(), "systems.vos.wisey.compiler.tests.CMyController");
  EXPECT_EQ(controller->findMethod("foo"), nullptr);
}

TEST_F(ExternalControllerDefinitionTest, prototypeMethodsTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ControllerTypeSpecifierFull* typeSpecifier = new ControllerTypeSpecifierFull(packageExpression,
                                                                               "CMyController",
                                                                               0);
  vector<IObjectDefinition*> innerObjectDefinitions;
  ExternalControllerDefinition controllerDefinition(typeSpecifier,
                                                    mElementDeclarations,
                                                    mInterfaces,
                                                    innerObjectDefinitions,
                                                    NULL,
                                                    0);

  controllerDefinition.prototypeObject(mContext, mContext.getImportProfile());
  controllerDefinition.prototypeMethods(mContext);
  
  Controller* controller =
  mContext.getController("systems.vos.wisey.compiler.tests.CMyController", 0);
  EXPECT_NE(controller->findMethod("foo"), nullptr);
}

TEST_F(TestFileRunner, externalControllerDefinitionsRunTest) {
  compileFile("tests/samples/test_external_controller_definitions.yz");
}

