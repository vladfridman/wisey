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

#include "TestFileSampleRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/AccessLevel.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/ExternalControllerDefinition.hpp"
#include "wisey/ExternalMethodDefinition.hpp"
#include "wisey/FloatConstant.hpp"
#include "wisey/Interface.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/IModelTypeSpecifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/ReceivedFieldDefinition.hpp"

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
    
    const PrimitiveTypeSpecifier* intTypeSpecifier = PrimitiveTypes::INT_TYPE->newTypeSpecifier();
    const PrimitiveTypeSpecifier* floatTypeSpecifier =
    PrimitiveTypes::FLOAT_TYPE->newTypeSpecifier();
    Identifier* intArgumentIdentifier = new Identifier("intargument");
    VariableDeclaration* intArgument =
    VariableDeclaration::create(intTypeSpecifier, intArgumentIdentifier, 0);
    VariableList methodArguments;
    methodArguments.push_back(intArgument);
    vector<IModelTypeSpecifier*> thrownExceptions;
    methodDeclaration = new ExternalMethodDefinition(floatTypeSpecifier,
                                                      "foo",
                                                      methodArguments,
                                                      thrownExceptions);
    
    const PrimitiveTypeSpecifier* longType = PrimitiveTypes::LONG_TYPE->newTypeSpecifier();
    const PrimitiveTypeSpecifier* floatType = PrimitiveTypes::FLOAT_TYPE->newTypeSpecifier();
    InjectionArgumentList arguments;
    ReceivedFieldDefinition* field1 = new ReceivedFieldDefinition(longType, "mField1");
    ReceivedFieldDefinition* field2 = new ReceivedFieldDefinition(floatType, "mField2");
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
                                                                               "CMyController");
  vector<IObjectDefinition*> innerObjectDefinitions;
  ExternalControllerDefinition controllerDefinition(typeSpecifier,
                                                    mElementDeclarations,
                                                    mInterfaces,
                                                    innerObjectDefinitions);

  controllerDefinition.prototypeObject(mContext);
  
  ASSERT_NE(mContext.getController("systems.vos.wisey.compiler.tests.CMyController"), nullptr);
  
  Controller* controller = mContext.getController("systems.vos.wisey.compiler.tests.CMyController");
  
  EXPECT_STREQ(controller->getShortName().c_str(), "CMyController");
  EXPECT_STREQ(controller->getTypeName().c_str(), "systems.vos.wisey.compiler.tests.CMyController");
  EXPECT_EQ(controller->findMethod("foo"), nullptr);
}

TEST_F(ExternalControllerDefinitionTest, prototypeMethodsTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ControllerTypeSpecifierFull* typeSpecifier = new ControllerTypeSpecifierFull(packageExpression,
                                                                               "CMyController");
  vector<IObjectDefinition*> innerObjectDefinitions;
  ExternalControllerDefinition controllerDefinition(typeSpecifier,
                                                    mElementDeclarations,
                                                    mInterfaces,
                                                    innerObjectDefinitions);

  controllerDefinition.prototypeObject(mContext);
  controllerDefinition.prototypeMethods(mContext);
  
  Controller* controller = mContext.getController("systems.vos.wisey.compiler.tests.CMyController");
  EXPECT_NE(controller->findMethod("foo"), nullptr);
}

TEST_F(TestFileSampleRunner, externalControllerDefinitionsRunTest) {
  compileFile("tests/samples/test_external_controller_definitions.yz");
}

