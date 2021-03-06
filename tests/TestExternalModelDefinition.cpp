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

#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "Argument.hpp"
#include "ExternalMethodDefinition.hpp"
#include "ExternalModelDefinition.hpp"
#include "FakeExpression.hpp"
#include "ReceivedFieldDefinition.hpp"
#include "FloatConstant.hpp"
#include "Names.hpp"
#include "PrimitiveTypes.hpp"
#include "PrimitiveTypeSpecifier.hpp"
#include "ReceivedFieldDefinition.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct ExternalModelDefinitionTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  ExternalMethodDefinition* mMethodDefinition;
  vector<IObjectElementDefinition*> mElementDeclarations;
  string mPackage = "systems.vos.wisey.compiler.tests";
  
  ExternalModelDefinitionTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    const PrimitiveTypeSpecifier* intTypeSpecifier = PrimitiveTypes::INT->newTypeSpecifier(0);
    const PrimitiveTypeSpecifier* floatTypeSpecifier =
    PrimitiveTypes::FLOAT->newTypeSpecifier(0);
    Identifier* intArgumentIdentifier = new Identifier("intargument", 0);
    VariableDeclaration* intArgument =
    VariableDeclaration::create(intTypeSpecifier, intArgumentIdentifier, 0);
    VariableList methodArguments;
    methodArguments.push_back(intArgument);
    vector<IModelTypeSpecifier*> thrownExceptions;
    mMethodDefinition = new ExternalMethodDefinition(floatTypeSpecifier,
                                                     "foo",
                                                     methodArguments,
                                                     thrownExceptions,
                                                     new MethodQualifiers(0),
                                                     0);
  }
};

TEST_F(ExternalModelDefinitionTest, prototypeObjectTest) {
  const PrimitiveTypeSpecifier* longType = PrimitiveTypes::LONG->newTypeSpecifier(0);
  const PrimitiveTypeSpecifier* floatType = PrimitiveTypes::FLOAT->newTypeSpecifier(0);
  InjectionArgumentList arguments;
  ReceivedFieldDefinition* field1 = ReceivedFieldDefinition::create(longType, "field1", 0);
  ReceivedFieldDefinition* field2 = ReceivedFieldDefinition::create(floatType, "field2", 0);
  mElementDeclarations.push_back(field1);
  mElementDeclarations.push_back(field2);
  mElementDeclarations.push_back(mMethodDefinition);

  vector<IInterfaceTypeSpecifier*> interfaces;
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ModelTypeSpecifierFull* typeSpecifier =
  new ModelTypeSpecifierFull(packageExpression, "MMyModel", 0);
  vector<IObjectDefinition*> innerObjectDefinitions;
  ExternalModelDefinition modelDefinition(typeSpecifier,
                                          mElementDeclarations,
                                          interfaces,
                                          innerObjectDefinitions,
                                          0);
  
  modelDefinition.prototypeObject(mContext, mContext.getImportProfile());
  
  Model* model = mContext.getModel("systems.vos.wisey.compiler.tests.MMyModel", 0);
  
  EXPECT_STREQ(model->getTypeName().c_str(), "systems.vos.wisey.compiler.tests.MMyModel");
  EXPECT_STREQ(model->getShortName().c_str(), "MMyModel");
  EXPECT_EQ(model->findMethod("foo"), nullptr);
}

TEST_F(ExternalModelDefinitionTest, prototypeMethodsTest) {
  const PrimitiveTypeSpecifier* longType = PrimitiveTypes::LONG->newTypeSpecifier(0);
  const PrimitiveTypeSpecifier* floatType = PrimitiveTypes::FLOAT->newTypeSpecifier(0);
  InjectionArgumentList arguments;
  ReceivedFieldDefinition* field1 = ReceivedFieldDefinition::create(longType, "field1", 0);
  ReceivedFieldDefinition* field2 = ReceivedFieldDefinition::create(floatType, "field2", 0);
  mElementDeclarations.push_back(field1);
  mElementDeclarations.push_back(field2);
  mElementDeclarations.push_back(mMethodDefinition);

  vector<IInterfaceTypeSpecifier*> interfaces;
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ModelTypeSpecifierFull* typeSpecifier =
  new ModelTypeSpecifierFull(packageExpression, "MMyModel", 0);
  vector<IObjectDefinition*> innerObjectDefinitions;
  ExternalModelDefinition modelDefinition(typeSpecifier,
                                          mElementDeclarations,
                                          interfaces,
                                          innerObjectDefinitions,
                                          0);

  modelDefinition.prototypeObject(mContext, mContext.getImportProfile());
  modelDefinition.prototypeMethods(mContext);
  
  Model* model = mContext.getModel("systems.vos.wisey.compiler.tests.MMyModel", 0);
  
  EXPECT_NE(model->findMethod("foo"), nullptr);
}

TEST_F(TestFileRunner, externalModelDefinitionsRunTest) {
  compileFile("tests/samples/test_external_model_definitions.yz");
}
