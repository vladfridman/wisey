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
#include "Argument.hpp"
#include "FakeExpression.hpp"
#include "FloatConstant.hpp"
#include "ExternalNodeDefinition.hpp"
#include "ExternalMethodDefinition.hpp"
#include "IObjectElementDefinition.hpp"
#include "MethodSignatureDeclaration.hpp"
#include "PrimitiveTypes.hpp"
#include "PrimitiveTypeSpecifier.hpp"
#include "ReceivedFieldDefinition.hpp"

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
  
  ~ExternalNodeDefinitionTest() {
  }
};

TEST_F(ExternalNodeDefinitionTest, prototypeObjectTest) {
  const PrimitiveTypeSpecifier* longType = PrimitiveTypes::LONG->newTypeSpecifier(0);
  const PrimitiveTypeSpecifier* floatType = PrimitiveTypes::FLOAT->newTypeSpecifier(0);
  ReceivedFieldDefinition* field1 = ReceivedFieldDefinition::create(longType, "field1", 0);
  ReceivedFieldDefinition* field2 = ReceivedFieldDefinition::create(floatType, "field2", 0);
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
  
  nodeDefinition.prototypeObject(mContext, mContext.getImportProfile());
  
  Node* node = mContext.getNode("systems.vos.wisey.compiler.tests.NMyNode", 0);
  
  EXPECT_STREQ(node->getTypeName().c_str(), "systems.vos.wisey.compiler.tests.NMyNode");
  EXPECT_STREQ(node->getShortName().c_str(), "NMyNode");
  EXPECT_EQ(node->findMethod("foo"), nullptr);
}

TEST_F(ExternalNodeDefinitionTest, prototypeMethodsTest) {
  const PrimitiveTypeSpecifier* longType = PrimitiveTypes::LONG->newTypeSpecifier(0);
  const PrimitiveTypeSpecifier* floatType = PrimitiveTypes::FLOAT->newTypeSpecifier(0);
  ReceivedFieldDefinition* field1 = ReceivedFieldDefinition::create(longType, "field1", 0);
  ReceivedFieldDefinition* field2 = ReceivedFieldDefinition::create(floatType, "field2", 0);
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
  
  nodeDefinition.prototypeObject(mContext, mContext.getImportProfile());
  nodeDefinition.prototypeMethods(mContext);
  
  Node* node = mContext.getNode("systems.vos.wisey.compiler.tests.NMyNode", 0);
  
  EXPECT_NE(node->findMethod("foo"), nullptr);
}

TEST_F(TestFileRunner, externalNodeDefinitionsRunTest) {
  compileFile("tests/samples/test_external_node_definitions.yz");
}

