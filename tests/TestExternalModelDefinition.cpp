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
#include "wisey/FakeExpression.hpp"
#include "wisey/FixedFieldDeclaration.hpp"
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
  string mPackage = "systems.vos.wisey.compiler.tests";
  
  ExternalModelDefinitionTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    const PrimitiveTypeSpecifier* intTypeSpecifier = PrimitiveTypes::INT_TYPE->newTypeSpecifier();
    const PrimitiveTypeSpecifier* floatTypeSpecifier =
    PrimitiveTypes::FLOAT_TYPE->newTypeSpecifier();
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

TEST_F(ExternalModelDefinitionTest, prototypeObjectTest) {
  const PrimitiveTypeSpecifier* longType = PrimitiveTypes::LONG_TYPE->newTypeSpecifier();
  const PrimitiveTypeSpecifier* floatType = PrimitiveTypes::FLOAT_TYPE->newTypeSpecifier();
  InjectionArgumentList arguments;
  FixedFieldDeclaration* field1 = new FixedFieldDeclaration(longType, "field1");
  FixedFieldDeclaration* field2 = new FixedFieldDeclaration(floatType, "field2");
  mElementDeclarations.push_back(field1);
  mElementDeclarations.push_back(field2);
  mElementDeclarations.push_back(mMethodDeclaration);

  vector<IInterfaceTypeSpecifier*> interfaces;
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ModelTypeSpecifierFull* typeSpecifier = new ModelTypeSpecifierFull(packageExpression, "MMyModel");
  vector<IObjectDefinition*> innerObjectDefinitions;
  ExternalModelDefinition modelDefinition(typeSpecifier,
                                          mElementDeclarations,
                                          interfaces,
                                          innerObjectDefinitions);
  
  modelDefinition.prototypeObject(mContext);
  
  Model* model = mContext.getModel("systems.vos.wisey.compiler.tests.MMyModel");
  
  EXPECT_STREQ(model->getTypeName().c_str(), "systems.vos.wisey.compiler.tests.MMyModel");
  EXPECT_STREQ(model->getShortName().c_str(), "MMyModel");
  EXPECT_EQ(model->findMethod("foo"), nullptr);
}

TEST_F(ExternalModelDefinitionTest, prototypeMethodsTest) {
  const PrimitiveTypeSpecifier* longType = PrimitiveTypes::LONG_TYPE->newTypeSpecifier();
  const PrimitiveTypeSpecifier* floatType = PrimitiveTypes::FLOAT_TYPE->newTypeSpecifier();
  InjectionArgumentList arguments;
  FixedFieldDeclaration* field1 = new FixedFieldDeclaration(longType, "field1");
  FixedFieldDeclaration* field2 = new FixedFieldDeclaration(floatType, "field2");
  mElementDeclarations.push_back(field1);
  mElementDeclarations.push_back(field2);
  mElementDeclarations.push_back(mMethodDeclaration);

  vector<IInterfaceTypeSpecifier*> interfaces;
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ModelTypeSpecifierFull* typeSpecifier = new ModelTypeSpecifierFull(packageExpression, "MMyModel");
  vector<IObjectDefinition*> innerObjectDefinitions;
  ExternalModelDefinition modelDefinition(typeSpecifier,
                                          mElementDeclarations,
                                          interfaces,
                                          innerObjectDefinitions);

  modelDefinition.prototypeObject(mContext);
  modelDefinition.prototypeMethods(mContext);
  
  Model* model = mContext.getModel("systems.vos.wisey.compiler.tests.MMyModel");
  
  EXPECT_NE(model->findMethod("foo"), nullptr);
}

TEST_F(TestFileSampleRunner, externalModelDefinitionsRunTest) {
  compileFile("tests/samples/test_external_model_definitions.yz");
}
