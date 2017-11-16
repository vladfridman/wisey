//
//  TestNodeDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link NodeDefinition}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>

#include "MockStatement.hpp"
#include "TestFileSampleRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/FieldDeclaration.hpp"
#include "wisey/FloatConstant.hpp"
#include "wisey/IObjectElementDeclaration.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodDeclaration.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/Names.hpp"
#include "wisey/NodeDefinition.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/ProgramPrefix.hpp"
#include "wisey/ReturnStatement.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Test;

struct NodeDefinitionTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  MethodDeclaration *mMethodDeclaration;
  vector<IObjectElementDeclaration*> mObjectElements;
  Block* mBlock;
  NiceMock<MockStatement>* mMockStatement;
  
  NodeDefinitionTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mBlock(new Block()),
  mMockStatement(new NiceMock<MockStatement>()) {
    TestPrefix::generateIR(mContext);
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);

    mContext.setPackage("systems.vos.wisey.compiler.tests");
    mBlock->getStatements().push_back(mMockStatement);
    mBlock->getStatements().push_back(new ReturnStatement(new FloatConstant(0.5)));
    CompoundStatement* compoundStatement = new CompoundStatement(mBlock);
    PrimitiveTypeSpecifier* intTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
    PrimitiveTypeSpecifier* floatTypeSpecifier =
      new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
    Identifier* intArgumentIdentifier = new Identifier("intargument");
    VariableDeclaration* intArgument =
    new VariableDeclaration(intTypeSpecifier, intArgumentIdentifier);
    VariableList methodArguments;
    methodArguments.push_back(intArgument);
    vector<ModelTypeSpecifier*> thrownExceptions;
    mMethodDeclaration = new MethodDeclaration(AccessLevel::PUBLIC_ACCESS,
                                               floatTypeSpecifier,
                                               "foo",
                                               methodArguments,
                                               thrownExceptions,
                                               compoundStatement);
  }
  
  ~NodeDefinitionTest() {
  }
};

TEST_F(NodeDefinitionTest, prototypeObjectsTest) {
  PrimitiveTypeSpecifier* longType = new PrimitiveTypeSpecifier(PrimitiveTypes::LONG_TYPE);
  PrimitiveTypeSpecifier* floatType = new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
  vector<IExpression*> arguments;
  FieldDeclaration* field1 = new FieldDeclaration(FIXED_FIELD, longType, "field1", arguments);
  FieldDeclaration* field2 = new FieldDeclaration(FIXED_FIELD, floatType, "field2", arguments);
  mObjectElements.push_back(field1);
  mObjectElements.push_back(field2);
  mObjectElements.push_back(mMethodDeclaration);

  vector<InterfaceTypeSpecifier*> interfaces;
  vector<string> package;
  NodeTypeSpecifier* typeSpecifier = new NodeTypeSpecifier(package, "NMyNode");
  NodeDefinition nodeDefinition(typeSpecifier, mObjectElements, interfaces);
  
  nodeDefinition.prototypeObjects(mContext);
  
  Node* node = mContext.getNode("systems.vos.wisey.compiler.tests.NMyNode");
  
  EXPECT_STREQ(node->getName().c_str(), "systems.vos.wisey.compiler.tests.NMyNode");
  EXPECT_STREQ(node->getShortName().c_str(), "NMyNode");
  EXPECT_EQ(node->findMethod("foo"), nullptr);
}

TEST_F(NodeDefinitionTest, prototypeMethodsTest) {
  PrimitiveTypeSpecifier* longType = new PrimitiveTypeSpecifier(PrimitiveTypes::LONG_TYPE);
  PrimitiveTypeSpecifier* floatType = new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
  vector<IExpression*> arguments;
  FieldDeclaration* field1 = new FieldDeclaration(FIXED_FIELD, longType, "field1", arguments);
  FieldDeclaration* field2 = new FieldDeclaration(FIXED_FIELD, floatType, "field2", arguments);
  mObjectElements.push_back(field1);
  mObjectElements.push_back(field2);
  mObjectElements.push_back(mMethodDeclaration);

  vector<InterfaceTypeSpecifier*> interfaces;
  vector<string> package;
  NodeTypeSpecifier* typeSpecifier = new NodeTypeSpecifier(package, "NMyNode");
  NodeDefinition nodeDefinition(typeSpecifier, mObjectElements, interfaces);
  
  nodeDefinition.prototypeObjects(mContext);
  nodeDefinition.prototypeMethods(mContext);
  
  Node* node = mContext.getNode("systems.vos.wisey.compiler.tests.NMyNode");
  
  EXPECT_NE(node->findMethod("foo"), nullptr);
}

TEST_F(NodeDefinitionTest, generateIRTest) {
  PrimitiveTypeSpecifier* longType = new PrimitiveTypeSpecifier(PrimitiveTypes::LONG_TYPE);
  PrimitiveTypeSpecifier* floatType = new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
  vector<IExpression*> arguments;
  FieldDeclaration* field1 = new FieldDeclaration(FIXED_FIELD, longType, "field1", arguments);
  FieldDeclaration* field2 = new FieldDeclaration(FIXED_FIELD, floatType, "field2", arguments);
  mObjectElements.push_back(field1);
  mObjectElements.push_back(field2);
  mObjectElements.push_back(mMethodDeclaration);

  vector<InterfaceTypeSpecifier*> interfaces;
  vector<string> package;
  NodeTypeSpecifier* typeSpecifier = new NodeTypeSpecifier(package, "NMyNode");
  NodeDefinition nodeDefinition(typeSpecifier, mObjectElements, interfaces);
  
  EXPECT_CALL(*mMockStatement, generateIR(_));
  
  nodeDefinition.prototypeObjects(mContext);
  nodeDefinition.prototypeMethods(mContext);
  nodeDefinition.generateIR(mContext);
  Node* node = mContext.getNode("systems.vos.wisey.compiler.tests.NMyNode");
  StructType* structType = (StructType*) node->getLLVMType(mLLVMContext)->getPointerElementType();
  
  ASSERT_NE(structType, nullptr);
  EXPECT_EQ(structType->getNumElements(), 3u);
  // reference counter
  EXPECT_EQ(structType->getElementType(0), Type::getInt64Ty(mLLVMContext));
  EXPECT_EQ(structType->getElementType(1), Type::getInt64Ty(mLLVMContext));
  EXPECT_EQ(structType->getElementType(2), Type::getFloatTy(mLLVMContext));
  EXPECT_STREQ(node->getName().c_str(), "systems.vos.wisey.compiler.tests.NMyNode");
  EXPECT_STREQ(node->getShortName().c_str(), "NMyNode");
  EXPECT_NE(node->findMethod("foo"), nullptr);
}

TEST_F(NodeDefinitionTest, interfaceImplmenetationDefinitionTest) {
  string interfaceFullName = "systems.vos.wisey.compiler.tests.IMyInterface";
  StructType* structType = StructType::create(mLLVMContext, interfaceFullName);
  vector<IObjectElementDeclaration*> interfaceElements;
  VariableList methodArguments;
  vector<ModelTypeSpecifier*> methodThrownExceptions;
  vector<string> wiseyLangPackage;
  wiseyLangPackage.push_back("wisey");
  wiseyLangPackage.push_back("lang");
  methodThrownExceptions.push_back(new ModelTypeSpecifier(wiseyLangPackage,
                                                          Names::getNPEModelName()));
  const PrimitiveTypeSpecifier* intSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
  PrimitiveTypeSpecifier* floatSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
  VariableDeclaration* methodArgument = new VariableDeclaration(intSpecifier,
                                                                new Identifier("intargument"));
  methodArguments.push_back(methodArgument);
  IObjectElementDeclaration* methodSignature =
    new MethodSignatureDeclaration(floatSpecifier,
                                   "foo",
                                   methodArguments,
                                   methodThrownExceptions);
  interfaceElements.push_back(methodSignature);
  vector<InterfaceTypeSpecifier*> parentInterfaces;
  Interface* interface = Interface::newInterface(interfaceFullName,
                                                 structType,
                                                 parentInterfaces,
                                                 interfaceElements);
  mContext.addInterface(interface);
  interface->buildMethods(mContext);
  
  llvm::Constant* stringConstant = ConstantDataArray::getString(mLLVMContext, interface->getName());
  new GlobalVariable(*mContext.getModule(),
                     stringConstant->getType(),
                     true,
                     GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                     stringConstant,
                     interface->getObjectNameGlobalVariableName());
  
  vector<InterfaceTypeSpecifier*> interfaces;
  vector<string> emptyPackage;
  interfaces.push_back(new InterfaceTypeSpecifier(emptyPackage, "IMyInterface"));

  mObjectElements.push_back(mMethodDeclaration);

  NodeTypeSpecifier* typeSpecifier = new NodeTypeSpecifier(emptyPackage, "NMyNode");
  NodeDefinition nodeDefinition(typeSpecifier, mObjectElements, interfaces);
  nodeDefinition.prototypeObjects(mContext);
  nodeDefinition.prototypeMethods(mContext);
  nodeDefinition.generateIR(mContext);
  
  GlobalVariable* vTablePointer = mContext.getModule()->
    getNamedGlobal("systems.vos.wisey.compiler.tests.NMyNode.vtable");
  
  ASSERT_NE(vTablePointer, nullptr);
  ASSERT_TRUE(vTablePointer->getType()->getPointerElementType()->isStructTy());
  EXPECT_EQ(vTablePointer->getType()->getPointerElementType()->getStructNumElements(), 1u);
  llvm::Constant* vTableInitializer = vTablePointer->getInitializer();
  ASSERT_TRUE(vTableInitializer->getType()->isStructTy());
  EXPECT_EQ(vTableInitializer->getType()->getStructNumElements(), 1u);
  
  GlobalVariable* vModelTypesPointer =
  mContext.getModule()->getNamedGlobal("systems.vos.wisey.compiler.tests.NMyNode.typetable");
  EXPECT_NE(vModelTypesPointer, nullptr);
  ASSERT_TRUE(vModelTypesPointer->getType()->getPointerElementType()->isArrayTy());
  EXPECT_EQ(vModelTypesPointer->getType()->getPointerElementType()->getArrayNumElements(), 3u);
}

TEST_F(NodeDefinitionTest, interfaceNotDefinedDeathTest) {
  vector<InterfaceTypeSpecifier*> interfaces;
  vector<string> package;
  package.push_back("systems");
  package.push_back("vos");
  package.push_back("wisey");
  package.push_back("compiler");
  package.push_back("tests");
  interfaces.push_back(new InterfaceTypeSpecifier(package, "IMyInterface"));
  
  mObjectElements.push_back(mMethodDeclaration);

  vector<string> packageSpecifier;
  NodeTypeSpecifier* typeSpecifier = new NodeTypeSpecifier(packageSpecifier, "NMyNode");
  NodeDefinition nodeDefinition(typeSpecifier, mObjectElements, interfaces);
  nodeDefinition.prototypeObjects(mContext);
  
  EXPECT_EXIT(nodeDefinition.prototypeMethods(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Interface systems.vos.wisey.compiler.tests.IMyInterface is not defined");
}

TEST_F(NodeDefinitionTest, nodeWithInjectedFieldDeathTest) {
  PrimitiveTypeSpecifier* longType = new PrimitiveTypeSpecifier(PrimitiveTypes::LONG_TYPE);
  PrimitiveTypeSpecifier* floatType = new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
  vector<IExpression*> arguments;
  FieldDeclaration* field1 = new FieldDeclaration(INJECTED_FIELD, longType, "field1", arguments);
  FieldDeclaration* field2 = new FieldDeclaration(FIXED_FIELD, floatType, "field2", arguments);
  mObjectElements.push_back(field1);
  mObjectElements.push_back(field2);
  mObjectElements.push_back(mMethodDeclaration);

  vector<InterfaceTypeSpecifier*> interfaces;
  vector<string> package;
  NodeTypeSpecifier* typeSpecifier = new NodeTypeSpecifier(package, "NMyNode");
  NodeDefinition nodeDefinition(typeSpecifier, mObjectElements, interfaces);
  nodeDefinition.prototypeObjects(mContext);
  
  EXPECT_EXIT(nodeDefinition.prototypeMethods(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Nodes can only have fixed or state fields");
}

TEST_F(TestFileSampleRunner, nodeDefinitionRunTest) {
  runFile("tests/samples/test_node_definition.yz", "0");
}

TEST_F(TestFileSampleRunner, nodeDefinitionWithMethodRunTest) {
  runFile("tests/samples/test_node_method.yz", "5");
}

TEST_F(TestFileSampleRunner, nodeStateFieldsNonNodeOwnerTypeDeathRunTest) {
  expectFailCompile("tests/samples/test_node_state_fields_non_node_owner_type.yz",
                    1,
                    "Error: Node state fields can only be node owner or interface owner type");
}

TEST_F(TestFileSampleRunner, nodeWithInjectedFieldDeathRunTest) {
  expectFailCompile("tests/samples/test_node_with_injected_field.yz",
                    1,
                    "Error: Nodes can only have fixed or state fields");
}
