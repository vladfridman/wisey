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
#include "wisey/AccessLevel.hpp"
#include "wisey/ExternalControllerDefinition.hpp"
#include "wisey/ExternalMethodDeclaration.hpp"
#include "wisey/FloatConstant.hpp"
#include "wisey/Interface.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/ProgramPrefix.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct ExternalControllerDefinitionTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  vector<FieldDeclaration*> mFieldDeclarations;
  vector<IMethodDeclaration*> mMethodDeclarations;
  vector<InterfaceTypeSpecifier*> mInterfaces;
  
  ExternalControllerDefinitionTest() : mLLVMContext(mContext.getLLVMContext()) {
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    
    ExternalMethodDeclaration* methodDeclaration;
    
    mContext.setPackage("systems.vos.wisey.compiler.tests");
    PrimitiveTypeSpecifier* intTypeSpecifier =
    new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
    PrimitiveTypeSpecifier* floatTypeSpecifier =
    new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
    Identifier* intArgumentIdentifier = new Identifier("intargument");
    VariableDeclaration* intArgument =
    new VariableDeclaration(intTypeSpecifier, intArgumentIdentifier);
    VariableList methodArguments;
    methodArguments.push_back(intArgument);
    vector<ModelTypeSpecifier*> thrownExceptions;
    methodDeclaration = new ExternalMethodDeclaration(floatTypeSpecifier,
                                                      "foo",
                                                      methodArguments,
                                                      thrownExceptions);
    mMethodDeclarations.push_back(methodDeclaration);
    
    PrimitiveTypeSpecifier* longType = new PrimitiveTypeSpecifier(PrimitiveTypes::LONG_TYPE);
    PrimitiveTypeSpecifier* floatType = new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
    ExpressionList arguments;
    FieldDeclaration* field1 = new FieldDeclaration(RECEIVED_FIELD, longType, "mField1", arguments);
    FieldDeclaration* field2 =
      new FieldDeclaration(RECEIVED_FIELD, floatType, "mField2", arguments);
    mFieldDeclarations.push_back(field1);
    mFieldDeclarations.push_back(field2);
  }
  
  ~ExternalControllerDefinitionTest() {
  }
};

TEST_F(ExternalControllerDefinitionTest, prototypeObjectsTest) {
  vector<string> package;
  ControllerTypeSpecifier* typeSpecifier = new ControllerTypeSpecifier(package, "CMyController");
  ExternalControllerDefinition controllerDefinition(typeSpecifier,
                                                    mFieldDeclarations,
                                                    mMethodDeclarations,
                                                    mInterfaces);

  controllerDefinition.prototypeObjects(mContext);
  
  ASSERT_NE(mContext.getController("systems.vos.wisey.compiler.tests.CMyController"), nullptr);
  
  Controller* controller = mContext.getController("systems.vos.wisey.compiler.tests.CMyController");
  
  EXPECT_STREQ(controller->getShortName().c_str(), "CMyController");
  EXPECT_STREQ(controller->getName().c_str(), "systems.vos.wisey.compiler.tests.CMyController");
  EXPECT_EQ(controller->findMethod("foo"), nullptr);
}

TEST_F(ExternalControllerDefinitionTest, prototypeMethodsTest) {
  vector<string> package;
  ControllerTypeSpecifier* typeSpecifier = new ControllerTypeSpecifier(package, "CMyController");
  ExternalControllerDefinition controllerDefinition(typeSpecifier,
                                                    mFieldDeclarations,
                                                    mMethodDeclarations,
                                                    mInterfaces);

  controllerDefinition.prototypeObjects(mContext);
  controllerDefinition.prototypeMethods(mContext);
  
  Controller* controller = mContext.getController("systems.vos.wisey.compiler.tests.CMyController");
  EXPECT_NE(controller->findMethod("foo"), nullptr);
}

TEST_F(TestFileSampleRunner, externalControllerDefinitionsRunTest) {
  compileFile("tests/samples/test_external_controller_definitions.yz");
}

