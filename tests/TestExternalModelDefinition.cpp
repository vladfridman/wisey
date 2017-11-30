//
//  TestExternalModelDefinition.cpp
//  runtests
//
//  Created by Vladimir Fridman on 9/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ExternalModelDefinition}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>

#include "TestFileSampleRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/ExternalMethodDeclaration.hpp"
#include "wisey/ExternalModelDefinition.hpp"
#include "wisey/FieldDeclaration.hpp"
#include "wisey/FloatConstant.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct ExternalModelDefinitionTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  ExternalMethodDeclaration* mMethodDeclaration;
  vector<IObjectElementDeclaration*> mElementDeclarations;
  
  ExternalModelDefinitionTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    mContext.setPackage("systems.vos.wisey.compiler.tests");
    PrimitiveTypeSpecifier* intTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
    PrimitiveTypeSpecifier* floatTypeSpecifier =
    new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
    Identifier* intArgumentIdentifier = new Identifier("intargument");
    VariableDeclaration* intArgument =
    VariableDeclaration::create(intTypeSpecifier, intArgumentIdentifier, 0);
    VariableList methodArguments;
    methodArguments.push_back(intArgument);
    vector<IModelTypeSpecifier*> thrownExceptions;
    mMethodDeclaration = new ExternalMethodDeclaration(floatTypeSpecifier,
                                                       "foo",
                                                       methodArguments,
                                                       thrownExceptions);
  }
};

TEST_F(ExternalModelDefinitionTest, prototypeObjectsTest) {
  PrimitiveTypeSpecifier* longType = new PrimitiveTypeSpecifier(PrimitiveTypes::LONG_TYPE);
  PrimitiveTypeSpecifier* floatType = new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
  vector<IExpression*> arguments;
  FieldDeclaration* field1 = new FieldDeclaration(FIXED_FIELD, longType, "field1", arguments);
  FieldDeclaration* field2 = new FieldDeclaration(FIXED_FIELD, floatType, "field2", arguments);
  mElementDeclarations.push_back(field1);
  mElementDeclarations.push_back(field2);
  mElementDeclarations.push_back(mMethodDeclaration);

  vector<IInterfaceTypeSpecifier*> interfaces;
  ModelTypeSpecifier* typeSpecifier = new ModelTypeSpecifier("", "MMyModel");
  ExternalModelDefinition modelDefinition(typeSpecifier, mElementDeclarations, interfaces);
  
  modelDefinition.prototypeObjects(mContext);
  
  Model* model = mContext.getModel("systems.vos.wisey.compiler.tests.MMyModel");
  
  EXPECT_STREQ(model->getName().c_str(), "systems.vos.wisey.compiler.tests.MMyModel");
  EXPECT_STREQ(model->getShortName().c_str(), "MMyModel");
  EXPECT_EQ(model->findMethod("foo"), nullptr);
}

TEST_F(ExternalModelDefinitionTest, prototypeMethodsTest) {
  PrimitiveTypeSpecifier* longType = new PrimitiveTypeSpecifier(PrimitiveTypes::LONG_TYPE);
  PrimitiveTypeSpecifier* floatType = new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
  vector<IExpression*> arguments;
  FieldDeclaration* field1 = new FieldDeclaration(FIXED_FIELD, longType, "field1", arguments);
  FieldDeclaration* field2 = new FieldDeclaration(FIXED_FIELD, floatType, "field2", arguments);
  mElementDeclarations.push_back(field1);
  mElementDeclarations.push_back(field2);
  mElementDeclarations.push_back(mMethodDeclaration);

  vector<IInterfaceTypeSpecifier*> interfaces;
  ModelTypeSpecifier* typeSpecifier = new ModelTypeSpecifier("", "MMyModel");
  ExternalModelDefinition modelDefinition(typeSpecifier, mElementDeclarations, interfaces);

  modelDefinition.prototypeObjects(mContext);
  modelDefinition.prototypeMethods(mContext);
  
  Model* model = mContext.getModel("systems.vos.wisey.compiler.tests.MMyModel");
  
  EXPECT_NE(model->findMethod("foo"), nullptr);
}

TEST_F(TestFileSampleRunner, externalModelDefinitionsRunTest) {
  compileFile("tests/samples/test_external_model_definitions.yz");
}
