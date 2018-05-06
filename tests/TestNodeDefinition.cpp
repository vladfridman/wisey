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

#include "MockObjectType.hpp"
#include "MockObjectTypeSpecifier.hpp"
#include "MockStatement.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/Argument.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/FixedFieldDefinition.hpp"
#include "wisey/FloatConstant.hpp"
#include "wisey/IObjectElementDefinition.hpp"
#include "wisey/InjectedFieldDefinition.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/MethodDefinition.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/Names.hpp"
#include "wisey/NodeDefinition.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/ReturnStatement.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct NodeDefinitionTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  MethodDefinition *mMethodDefinition;
  vector<IObjectElementDefinition*> mObjectElements;
  Block* mBlock;
  NiceMock<MockStatement>* mMockStatement;
  string mPackage = "systems.vos.wisey.compiler.tests";
  ImportProfile* mImportProfile;

  NodeDefinitionTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mBlock(new Block()),
  mMockStatement(new NiceMock<MockStatement>()) {
    TestPrefix::generateIR(mContext);

    mBlock->getStatements().push_back(mMockStatement);
    mBlock->getStatements().push_back(new ReturnStatement(new FloatConstant(0.5, 0), 0));
    CompoundStatement* compoundStatement = new CompoundStatement(mBlock, 0);
    const PrimitiveTypeSpecifier* intTypeSpecifier = PrimitiveTypes::INT->newTypeSpecifier(0);
    const PrimitiveTypeSpecifier* floatTypeSpecifier =
    PrimitiveTypes::FLOAT->newTypeSpecifier(0);
    Identifier* intArgumentIdentifier = new Identifier("intargument", 0);
    VariableDeclaration* intArgument =
    VariableDeclaration::create(intTypeSpecifier, intArgumentIdentifier, 0);
    VariableList methodArguments;
    methodArguments.push_back(intArgument);
    vector<IModelTypeSpecifier*> thrownExceptions;
    mMethodDefinition = new MethodDefinition(AccessLevel::PUBLIC_ACCESS,
                                             floatTypeSpecifier,
                                             "foo",
                                             methodArguments,
                                             thrownExceptions,
                                             new MethodQualifiers(0),
                                             compoundStatement,
                                             0);
  }
  
  ~NodeDefinitionTest() {
  }
};

TEST_F(NodeDefinitionTest, prototypeObjectTest) {
  const PrimitiveTypeSpecifier* longType = PrimitiveTypes::LONG->newTypeSpecifier(0);
  const PrimitiveTypeSpecifier* floatType = PrimitiveTypes::FLOAT->newTypeSpecifier(0);
  FixedFieldDefinition* field1 = new FixedFieldDefinition(longType, "field1", 0);
  FixedFieldDefinition* field2 = new FixedFieldDefinition(floatType, "field2", 0);
  mObjectElements.push_back(field1);
  mObjectElements.push_back(field2);
  mObjectElements.push_back(mMethodDefinition);

  vector<IInterfaceTypeSpecifier*> interfaces;
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  NodeTypeSpecifierFull* typeSpecifier =
  new NodeTypeSpecifierFull(packageExpression, "NMyNode", 0);
  vector<IObjectDefinition*> innerObjectDefinitions;
  NodeDefinition nodeDefinition(AccessLevel::PUBLIC_ACCESS,
                                typeSpecifier,
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

TEST_F(NodeDefinitionTest, prototypeMethodsTest) {
  const PrimitiveTypeSpecifier* longType = PrimitiveTypes::LONG->newTypeSpecifier(0);
  const PrimitiveTypeSpecifier* floatType = PrimitiveTypes::FLOAT->newTypeSpecifier(0);
  FixedFieldDefinition* field1 = new FixedFieldDefinition(longType, "field1", 0);
  FixedFieldDefinition* field2 = new FixedFieldDefinition(floatType, "field2", 0);
  mObjectElements.push_back(field1);
  mObjectElements.push_back(field2);
  mObjectElements.push_back(mMethodDefinition);

  vector<IInterfaceTypeSpecifier*> interfaces;
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  NodeTypeSpecifierFull* typeSpecifier = new NodeTypeSpecifierFull(packageExpression, "NMyNode", 0);
  vector<IObjectDefinition*> innerObjectDefinitions;
  NodeDefinition nodeDefinition(AccessLevel::PUBLIC_ACCESS,
                                typeSpecifier,
                                mObjectElements,
                                interfaces,
                                innerObjectDefinitions,
                                0);
  
  nodeDefinition.prototypeObject(mContext, mContext.getImportProfile());
  nodeDefinition.prototypeMethods(mContext);
  
  Node* node = mContext.getNode("systems.vos.wisey.compiler.tests.NMyNode", 0);
  
  EXPECT_NE(node->findMethod("foo"), nullptr);
}

TEST_F(NodeDefinitionTest, generateIRTest) {
  const PrimitiveTypeSpecifier* longType = PrimitiveTypes::LONG->newTypeSpecifier(0);
  const PrimitiveTypeSpecifier* floatType = PrimitiveTypes::FLOAT->newTypeSpecifier(0);
  FixedFieldDefinition* field1 = new FixedFieldDefinition(longType, "field1", 0);
  FixedFieldDefinition* field2 = new FixedFieldDefinition(floatType, "field2", 0);
  mObjectElements.push_back(field1);
  mObjectElements.push_back(field2);
  mObjectElements.push_back(mMethodDefinition);

  vector<IInterfaceTypeSpecifier*> interfaces;
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  NodeTypeSpecifierFull* typeSpecifier =
  new NodeTypeSpecifierFull(packageExpression, "NMyNode", 0);
  vector<IObjectDefinition*> innerObjectDefinitions;
  NodeDefinition nodeDefinition(AccessLevel::PUBLIC_ACCESS,
                                typeSpecifier,
                                mObjectElements,
                                interfaces,
                                innerObjectDefinitions,
                                0);
  
  EXPECT_CALL(*mMockStatement, generateIR(_));
  
  nodeDefinition.prototypeObject(mContext, mContext.getImportProfile());
  nodeDefinition.prototypeMethods(mContext);
  nodeDefinition.generateIR(mContext);
  Node* node = mContext.getNode("systems.vos.wisey.compiler.tests.NMyNode", 0);
  StructType* structType = (StructType*) node->getLLVMType(mContext)->getPointerElementType();
  
  ASSERT_NE(structType, nullptr);
  EXPECT_EQ(structType->getNumElements(), 3u);
  EXPECT_EQ(structType->getElementType(0),
            FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
            ->getPointerTo()->getPointerTo());
  EXPECT_EQ(structType->getElementType(1), Type::getInt64Ty(mLLVMContext));
  EXPECT_EQ(structType->getElementType(2), Type::getFloatTy(mLLVMContext));
  EXPECT_STREQ(node->getTypeName().c_str(), "systems.vos.wisey.compiler.tests.NMyNode");
  EXPECT_STREQ(node->getShortName().c_str(), "NMyNode");
  EXPECT_NE(node->findMethod("foo"), nullptr);
}

TEST_F(NodeDefinitionTest, interfaceImplmenetationDefinitionTest) {
  CompoundStatement* compoundStatement = new CompoundStatement(mBlock, 0);
  const PrimitiveTypeSpecifier* intTypeSpecifier = PrimitiveTypes::INT->newTypeSpecifier(0);
  const PrimitiveTypeSpecifier* floatTypeSpecifier =
  PrimitiveTypes::FLOAT->newTypeSpecifier(0);
  Identifier* intArgumentIdentifier = new Identifier("intargument", 0);
  VariableDeclaration* intArgument =
  VariableDeclaration::create(intTypeSpecifier, intArgumentIdentifier, 0);
  VariableList methodArguments;
  methodArguments.push_back(intArgument);
  vector<IModelTypeSpecifier*> thrownExceptions;
  MethodQualifiers* methodQualifiers = new MethodQualifiers(0);
  methodQualifiers->getMethodQualifierSet().insert(MethodQualifier::OVERRIDE);
  mMethodDefinition = new MethodDefinition(AccessLevel::PUBLIC_ACCESS,
                                           floatTypeSpecifier,
                                           "foo",
                                           methodArguments,
                                           thrownExceptions,
                                           methodQualifiers,
                                           compoundStatement,
                                           0);

  string interfaceFullName = "systems.vos.wisey.compiler.tests.IMyInterface";
  StructType* structType = StructType::create(mLLVMContext, interfaceFullName);
  vector<IObjectElementDefinition*> interfaceElements;
  methodArguments.clear();
  vector<IModelTypeSpecifier*> methodThrownExceptions;
  PackageType* packageType = new PackageType(Names::getLangPackageName());
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  methodThrownExceptions.push_back(new ModelTypeSpecifier(packageExpression,
                                                          Names::getNPEModelName(),
                                                          0));
  const PrimitiveTypeSpecifier* intSpecifier = PrimitiveTypes::INT->newTypeSpecifier(0);
  const PrimitiveTypeSpecifier* floatSpecifier = PrimitiveTypes::FLOAT->newTypeSpecifier(0);
  VariableDeclaration* methodArgument =
  VariableDeclaration::create(intSpecifier, new Identifier("intargument", 0), 0);
  methodArguments.push_back(methodArgument);
  IObjectElementDefinition* methodSignature =
    new MethodSignatureDeclaration(floatSpecifier,
                                   "foo",
                                   methodArguments,
                                   methodThrownExceptions,
                                   new MethodQualifiers(0),
                                   0);
  interfaceElements.push_back(methodSignature);
  vector<IInterfaceTypeSpecifier*> parentInterfaces;
  Interface* interface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                 interfaceFullName,
                                                 structType,
                                                 parentInterfaces,
                                                 interfaceElements,
                                                 mContext.getImportProfile(),
                                                 0);
  mContext.addInterface(interface, 0);
  interface->buildMethods(mContext);
  
  llvm::Constant* stringConstant = ConstantDataArray::getString(mLLVMContext,
                                                                interface->getTypeName());
  new GlobalVariable(*mContext.getModule(),
                     stringConstant->getType(),
                     true,
                     GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                     stringConstant,
                     interface->getObjectNameGlobalVariableName());
  
  vector<IInterfaceTypeSpecifier*> interfaces;
  interfaces.push_back(new InterfaceTypeSpecifier(NULL, "IMyInterface", 0));

  mObjectElements.push_back(mMethodDefinition);

  packageType = new PackageType(mPackage);
  packageExpression = new FakeExpression(NULL, packageType);
  NodeTypeSpecifierFull* typeSpecifier = new NodeTypeSpecifierFull(packageExpression, "NMyNode", 0);
  vector<IObjectDefinition*> innerObjectDefinitions;
  NodeDefinition nodeDefinition(AccessLevel::PUBLIC_ACCESS,
                                typeSpecifier,
                                mObjectElements,
                                interfaces,
                                innerObjectDefinitions,
                                0);
  nodeDefinition.prototypeObject(mContext, mContext.getImportProfile());
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
  EXPECT_EQ(vModelTypesPointer->getType()->getPointerElementType()->getArrayNumElements(), 4u);
}

TEST_F(NodeDefinitionTest, interfaceNotDefinedDeathTest) {
  vector<IInterfaceTypeSpecifier*> interfaces;
  string package = "systems.vos.wisey.compiler.tests";
  PackageType* packageType = new PackageType(package);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  interfaces.push_back(new InterfaceTypeSpecifier(packageExpression, "IMyInterface", 3));
  
  mObjectElements.push_back(mMethodDefinition);

  packageType = new PackageType(mPackage);
  packageExpression = new FakeExpression(NULL, packageType);
  NodeTypeSpecifierFull* typeSpecifier = new NodeTypeSpecifierFull(packageExpression, "NMyNode", 0);
  vector<IObjectDefinition*> innerObjectDefinitions;
  NodeDefinition nodeDefinition(AccessLevel::PUBLIC_ACCESS,
                                typeSpecifier,
                                mObjectElements,
                                interfaces,
                                innerObjectDefinitions,
                                0);
  nodeDefinition.prototypeObject(mContext, mContext.getImportProfile());
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(nodeDefinition.prototypeMethods(mContext));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Interface systems.vos.wisey.compiler.tests.IMyInterface is not defined\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(NodeDefinitionTest, nodeWithInjectedFieldDeathTest) {
  const PrimitiveTypeSpecifier* floatType = PrimitiveTypes::FLOAT->newTypeSpecifier(0);
  NiceMock<MockObjectType> object;
  ON_CALL(object, isOwner()).WillByDefault(Return(true));
  NiceMock<MockObjectTypeSpecifier>* objectSpecifier = new NiceMock<MockObjectTypeSpecifier>();
  ON_CALL(*objectSpecifier, getType(_)).WillByDefault(Return(&object));

  InjectionArgumentList arguments;
  InjectedFieldDefinition* field1 = new InjectedFieldDefinition(objectSpecifier,
                                                                "field1",
                                                                arguments,
                                                                1);
  FixedFieldDefinition* field2 = new FixedFieldDefinition(floatType, "field2", 0);
  mObjectElements.push_back(field1);
  mObjectElements.push_back(field2);
  mObjectElements.push_back(mMethodDefinition);

  vector<IInterfaceTypeSpecifier*> interfaces;
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  NodeTypeSpecifierFull* typeSpecifier = new NodeTypeSpecifierFull(packageExpression, "NMyNode", 0);
  vector<IObjectDefinition*> innerObjectDefinitions;
  NodeDefinition nodeDefinition(AccessLevel::PUBLIC_ACCESS,
                                typeSpecifier,
                                mObjectElements,
                                interfaces,
                                innerObjectDefinitions,
                                0);
  nodeDefinition.prototypeObject(mContext, mContext.getImportProfile());
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(nodeDefinition.prototypeMethods(mContext));
  EXPECT_STREQ("/tmp/source.yz(1): Error: Nodes can only have fixed or state fields\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, nodeDefinitionRunTest) {
  runFile("tests/samples/test_node_definition.yz", "0");
}

TEST_F(TestFileRunner, nodeDefinitionWithMethodRunTest) {
  runFile("tests/samples/test_node_method.yz", "5");
}

TEST_F(TestFileRunner, nodeStateFieldsNonNodeOwnerTypeDeathRunTest) {
  expectFailCompile("tests/samples/test_node_state_fields_non_node_owner_type.yz",
                    1,
                    "Error: Node state fields can only be node owner or interface owner type");
}

TEST_F(TestFileRunner, nodeWithInjectedFieldDeathRunTest) {
  expectFailCompile("tests/samples/test_node_with_injected_field.yz",
                    1,
                    "Error: Nodes can only have fixed or state fields");
}
