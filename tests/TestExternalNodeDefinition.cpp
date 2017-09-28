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

#include "TestFileSampleRunner.hpp"
#include "wisey/FloatConstant.hpp"
#include "wisey/ExternalNodeDefinition.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/ProgramPrefix.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct ExternalNodeDefinitionTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  MethodSignatureDeclaration* mMethodSignatureDeclaration;
  vector<FieldDeclaration*> mFieldDeclarations;
  vector<MethodSignatureDeclaration*> mMethodSignatures;
  
  ExternalNodeDefinitionTest() : mLLVMContext(mContext.getLLVMContext()) {
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    
    mContext.setPackage("systems.vos.wisey.compiler.tests");
    PrimitiveTypeSpecifier* intTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
    PrimitiveTypeSpecifier* floatTypeSpecifier =
    new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
    Identifier* intArgumentIdentifier = new Identifier("intargument");
    VariableDeclaration* intArgument =
    new VariableDeclaration(intTypeSpecifier, intArgumentIdentifier);
    VariableList methodArguments;
    methodArguments.push_back(intArgument);
    vector<ModelTypeSpecifier*> thrownExceptions;
    mMethodSignatureDeclaration = new MethodSignatureDeclaration(floatTypeSpecifier,
                                                                 "foo",
                                                                 methodArguments,
                                                                 thrownExceptions);
    mMethodSignatures.push_back(mMethodSignatureDeclaration);
  }
  
  ~ExternalNodeDefinitionTest() {
  }
};

TEST_F(ExternalNodeDefinitionTest, prototypeObjectsTest) {
  PrimitiveTypeSpecifier* longType = new PrimitiveTypeSpecifier(PrimitiveTypes::LONG_TYPE);
  PrimitiveTypeSpecifier* floatType = new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
  vector<IExpression*> arguments;
  FieldDeclaration* field1 = new FieldDeclaration(FIXED_FIELD, longType, "field1", arguments);
  FieldDeclaration* field2 = new FieldDeclaration(FIXED_FIELD, floatType, "field2", arguments);
  mFieldDeclarations.push_back(field1);
  mFieldDeclarations.push_back(field2);
  
  vector<InterfaceTypeSpecifier*> interfaces;
  vector<string> package;
  NodeTypeSpecifier* typeSpecifier = new NodeTypeSpecifier(package, "NMyNode");
  ExternalNodeDefinition nodeDefinition(typeSpecifier,
                                        mFieldDeclarations,
                                        mMethodSignatures,
                                        interfaces);
  
  nodeDefinition.prototypeObjects(mContext);
  
  Node* node = mContext.getNode("systems.vos.wisey.compiler.tests.NMyNode");
  
  EXPECT_STREQ(node->getName().c_str(), "systems.vos.wisey.compiler.tests.NMyNode");
  EXPECT_STREQ(node->getShortName().c_str(), "NMyNode");
  EXPECT_EQ(node->findMethod("foo"), nullptr);
}

TEST_F(ExternalNodeDefinitionTest, prototypeMethodsTest) {
  PrimitiveTypeSpecifier* longType = new PrimitiveTypeSpecifier(PrimitiveTypes::LONG_TYPE);
  PrimitiveTypeSpecifier* floatType = new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
  vector<IExpression*> arguments;
  FieldDeclaration* field1 = new FieldDeclaration(FIXED_FIELD, longType, "field1", arguments);
  FieldDeclaration* field2 = new FieldDeclaration(FIXED_FIELD, floatType, "field2", arguments);
  mFieldDeclarations.push_back(field1);
  mFieldDeclarations.push_back(field2);
  
  vector<InterfaceTypeSpecifier*> interfaces;
  vector<string> package;
  NodeTypeSpecifier* typeSpecifier = new NodeTypeSpecifier(package, "NMyNode");
  ExternalNodeDefinition nodeDefinition(typeSpecifier,
                                        mFieldDeclarations,
                                        mMethodSignatures,
                                        interfaces);
  
  nodeDefinition.prototypeObjects(mContext);
  nodeDefinition.prototypeMethods(mContext);
  
  Node* node = mContext.getNode("systems.vos.wisey.compiler.tests.NMyNode");
  
  EXPECT_NE(node->findMethod("foo"), nullptr);
}

TEST_F(TestFileSampleRunner, externalNodeDefinitionsRunTest) {
  compileFile("tests/samples/test_external_node_definitions.yz");
}

