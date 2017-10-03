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
#include "wisey/FloatConstant.hpp"
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
  vector<FieldDeclaration*> mFieldDeclarations;
  vector<IMethodDeclaration*> mMethodDeclarations;
  Block* mBlock;
  NiceMock<MockStatement>* mMockStatement;
  
  NodeDefinitionTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mBlock(new Block()),
  mMockStatement(new NiceMock<MockStatement>()) {
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
    mMethodDeclarations.push_back(mMethodDeclaration);
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
  mFieldDeclarations.push_back(field1);
  mFieldDeclarations.push_back(field2);
  
  vector<InterfaceTypeSpecifier*> interfaces;
  vector<string> package;
  NodeTypeSpecifier* typeSpecifier = new NodeTypeSpecifier(package, "NMyNode");
  NodeDefinition nodeDefinition(typeSpecifier, mFieldDeclarations, mMethodDeclarations, interfaces);
  
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
  mFieldDeclarations.push_back(field1);
  mFieldDeclarations.push_back(field2);
  
  vector<InterfaceTypeSpecifier*> interfaces;
  vector<string> package;
  NodeTypeSpecifier* typeSpecifier = new NodeTypeSpecifier(package, "NMyNode");
  NodeDefinition nodeDefinition(typeSpecifier, mFieldDeclarations, mMethodDeclarations, interfaces);
  
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
  mFieldDeclarations.push_back(field1);
  mFieldDeclarations.push_back(field2);
  
  vector<InterfaceTypeSpecifier*> interfaces;
  vector<string> package;
  NodeTypeSpecifier* typeSpecifier = new NodeTypeSpecifier(package, "NMyNode");
  NodeDefinition nodeDefinition(typeSpecifier, mFieldDeclarations, mMethodDeclarations, interfaces);
  
  EXPECT_CALL(*mMockStatement, generateIR(_));
  
  nodeDefinition.prototypeObjects(mContext);
  nodeDefinition.prototypeMethods(mContext);
  nodeDefinition.generateIR(mContext);
  Node* node = mContext.getNode("systems.vos.wisey.compiler.tests.NMyNode");
  StructType* structType = (StructType*) node->getLLVMType(mLLVMContext)
  ->getPointerElementType()->getPointerElementType();
  
  ASSERT_NE(structType, nullptr);
  EXPECT_TRUE(structType->getNumElements() == 2);
  EXPECT_EQ(structType->getElementType(0), Type::getInt64Ty(mLLVMContext));
  EXPECT_EQ(structType->getElementType(1), Type::getFloatTy(mLLVMContext));
  EXPECT_STREQ(node->getName().c_str(), "systems.vos.wisey.compiler.tests.NMyNode");
  EXPECT_STREQ(node->getShortName().c_str(), "NMyNode");
  EXPECT_NE(node->findMethod("foo"), nullptr);
}

TEST_F(NodeDefinitionTest, interfaceImplmenetationDefinitionTest) {
  string interfaceFullName = "systems.vos.wisey.compiler.tests.IMyInterface";
  StructType* structType = StructType::create(mLLVMContext, interfaceFullName);
  vector<MethodSignatureDeclaration*> interfaceMethodSignatures;
  VariableList methodArguments;
  vector<ModelTypeSpecifier*> methodThrownExceptions;
  vector<string> package;
  methodThrownExceptions.push_back(new ModelTypeSpecifier(package, Names::getNPEModelName()));
  const PrimitiveTypeSpecifier* intSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
  PrimitiveTypeSpecifier* floatSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
  VariableDeclaration* methodArgument = new VariableDeclaration(intSpecifier,
                                                                new Identifier("intargument"));
  methodArguments.push_back(methodArgument);
  MethodSignatureDeclaration* methodSignature =
    new MethodSignatureDeclaration(floatSpecifier,
                                   "foo",
                                   methodArguments,
                                   methodThrownExceptions);
  interfaceMethodSignatures.push_back(methodSignature);
  vector<InterfaceTypeSpecifier*> parentInterfaces;
  Interface* interface = new Interface(interfaceFullName,
                                       structType,
                                       parentInterfaces,
                                       interfaceMethodSignatures);
  mContext.addInterface(interface);
  interface->buildMethods(mContext);
  
  Constant* stringConstant = ConstantDataArray::getString(mLLVMContext, interface->getName());
  new GlobalVariable(*mContext.getModule(),
                     stringConstant->getType(),
                     true,
                     GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                     stringConstant,
                     interface->getObjectNameGlobalVariableName());
  
  vector<InterfaceTypeSpecifier*> interfaces;
  interfaces.push_back(new InterfaceTypeSpecifier(package, "IMyInterface"));
  
  NodeTypeSpecifier* typeSpecifier = new NodeTypeSpecifier(package, "NMyNode");
  NodeDefinition nodeDefinition(typeSpecifier, mFieldDeclarations, mMethodDeclarations, interfaces);
  nodeDefinition.prototypeObjects(mContext);
  nodeDefinition.prototypeMethods(mContext);
  nodeDefinition.generateIR(mContext);
  
  GlobalVariable* vTablePointer = mContext.getModule()->
  getGlobalVariable("systems.vos.wisey.compiler.tests.NMyNode.vtable");
  
  ASSERT_NE(vTablePointer, nullptr);
  ASSERT_TRUE(vTablePointer->getType()->getPointerElementType()->isStructTy());
  EXPECT_EQ(vTablePointer->getType()->getPointerElementType()->getStructNumElements(), 1u);
  Constant* vTableInitializer = vTablePointer->getInitializer();
  ASSERT_TRUE(vTableInitializer->getType()->isStructTy());
  EXPECT_EQ(vTableInitializer->getType()->getStructNumElements(), 1u);
  
  GlobalVariable* vModelTypesPointer =
  mContext.getModule()->getGlobalVariable("systems.vos.wisey.compiler.tests.NMyNode.typetable");
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
  
  vector<string> packageSpecifier;
  NodeTypeSpecifier* typeSpecifier = new NodeTypeSpecifier(packageSpecifier, "NMyNode");
  NodeDefinition nodeDefinition(typeSpecifier, mFieldDeclarations, mMethodDeclarations, interfaces);
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
  mFieldDeclarations.push_back(field1);
  mFieldDeclarations.push_back(field2);
  
  vector<InterfaceTypeSpecifier*> interfaces;
  vector<string> package;
  NodeTypeSpecifier* typeSpecifier = new NodeTypeSpecifier(package, "NMyNode");
  NodeDefinition nodeDefinition(typeSpecifier, mFieldDeclarations, mMethodDeclarations, interfaces);
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
                    "Error: Node state fields can only be node owner type");
}

TEST_F(TestFileSampleRunner, nodeWithInjectedFieldDeathRunTest) {
  expectFailCompile("tests/samples/test_node_with_injected_field.yz",
                    1,
                    "Error: Nodes can only have fixed or state fields");
}
