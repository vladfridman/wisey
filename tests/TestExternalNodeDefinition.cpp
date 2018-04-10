//
//  TestExternalNodeDefinition.cpp
//  runtests
//
//  Created by Vladimir Fridman on 9/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ExternalNodeDefinition}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>

#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/FixedFieldDefinition.hpp"
#include "wisey/FloatConstant.hpp"
#include "wisey/ExternalNodeDefinition.hpp"
#include "wisey/ExternalMethodDefinition.hpp"
#include "wisey/IObjectElementDefinition.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct ExternalNodeDefinitionTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  ExternalMethodDefinition* mMethodDefinition;
  vector<IObjectElementDefinition*> mObjectElements;
  string mPackage = "systems.vos.wisey.compiler.tests";
  ImportProfile* mImportProfile;

  ExternalNodeDefinitionTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    mImportProfile = new ImportProfile(mPackage);
    mContext.setImportProfile(mImportProfile);

    const PrimitiveTypeSpecifier* intTypeSpecifier = PrimitiveTypes::INT_TYPE->newTypeSpecifier();
    const PrimitiveTypeSpecifier* floatTypeSpecifier =
    PrimitiveTypes::FLOAT_TYPE->newTypeSpecifier();
    Identifier* intArgumentIdentifier = new Identifier("intargument");
    VariableDeclaration* intArgument =
    VariableDeclaration::create(intTypeSpecifier, intArgumentIdentifier, 0);
    VariableList methodArguments;
    methodArguments.push_back(intArgument);
    vector<IModelTypeSpecifier*> thrownExceptions;
    MethodQualifierSet methodQualifiers;
    mMethodDefinition = new ExternalMethodDefinition(floatTypeSpecifier,
                                                     "foo",
                                                     methodArguments,
                                                     thrownExceptions,
                                                     methodQualifiers,
                                                     0);
  }
  
  ~ExternalNodeDefinitionTest() {
  }
};

TEST_F(ExternalNodeDefinitionTest, prototypeObjectTest) {
  const PrimitiveTypeSpecifier* longType = PrimitiveTypes::LONG_TYPE->newTypeSpecifier();
  const PrimitiveTypeSpecifier* floatType = PrimitiveTypes::FLOAT_TYPE->newTypeSpecifier();
  FixedFieldDefinition* field1 = new FixedFieldDefinition(longType, "field1");
  FixedFieldDefinition* field2 = new FixedFieldDefinition(floatType, "field2");
  mObjectElements.push_back(field1);
  mObjectElements.push_back(field2);
  mObjectElements.push_back(mMethodDefinition);

  vector<IInterfaceTypeSpecifier*> interfaces;
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  NodeTypeSpecifierFull* typeSpecifier = new NodeTypeSpecifierFull(packageExpression,
                                                                   "NMyNode",
                                                                   0);
  vector<IObjectDefinition*> innerObjectDefinitions;
  ExternalNodeDefinition nodeDefinition(typeSpecifier,
                                        mObjectElements,
                                        interfaces,
                                        innerObjectDefinitions,
                                        0);
  
  nodeDefinition.prototypeObject(mContext);
  
  Node* node = mContext.getNode("systems.vos.wisey.compiler.tests.NMyNode", 0);
  
  EXPECT_STREQ(node->getTypeName().c_str(), "systems.vos.wisey.compiler.tests.NMyNode");
  EXPECT_STREQ(node->getShortName().c_str(), "NMyNode");
  EXPECT_EQ(node->findMethod("foo"), nullptr);
}

TEST_F(ExternalNodeDefinitionTest, prototypeMethodsTest) {
  const PrimitiveTypeSpecifier* longType = PrimitiveTypes::LONG_TYPE->newTypeSpecifier();
  const PrimitiveTypeSpecifier* floatType = PrimitiveTypes::FLOAT_TYPE->newTypeSpecifier();
  FixedFieldDefinition* field1 = new FixedFieldDefinition(longType, "field1");
  FixedFieldDefinition* field2 = new FixedFieldDefinition(floatType, "field2");
  mObjectElements.push_back(field1);
  mObjectElements.push_back(field2);
  mObjectElements.push_back(mMethodDefinition);

  vector<IInterfaceTypeSpecifier*> interfaces;
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  NodeTypeSpecifierFull* typeSpecifier = new NodeTypeSpecifierFull(packageExpression, "NMyNode", 0);
  vector<IObjectDefinition*> innerObjectDefinitions;
  ExternalNodeDefinition nodeDefinition(typeSpecifier,
                                        mObjectElements,
                                        interfaces,
                                        innerObjectDefinitions,
                                        0);
  
  nodeDefinition.prototypeObject(mContext);
  nodeDefinition.prototypeMethods(mContext);
  
  Node* node = mContext.getNode("systems.vos.wisey.compiler.tests.NMyNode", 0);
  
  EXPECT_NE(node->findMethod("foo"), nullptr);
}

TEST_F(TestFileRunner, externalNodeDefinitionsRunTest) {
  compileFile("tests/samples/test_external_node_definitions.yz");
}

