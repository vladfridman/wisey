//
//  TestModelDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ModelDefinition}
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
#include "wisey/ReceivedFieldDefinition.hpp"
#include "wisey/FloatConstant.hpp"
#include "wisey/IObjectElementDefinition.hpp"
#include "wisey/InjectedFieldDefinition.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/MethodDefinition.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/ModelDefinition.hpp"
#include "wisey/Names.hpp"
#include "wisey/PackageType.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/ReceivedFieldDefinition.hpp"
#include "wisey/ReturnStatement.hpp"
#include "wisey/VariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ModelDefinitionTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  MethodDefinition *mMethodDefinition;
  vector<IObjectElementDefinition*> mObjectElements;
  Block* mBlock;
  NiceMock<MockStatement>* mMockStatement;
  string mPackage = "systems.vos.wisey.compiler.tests";
  ImportProfile* mImportProfile;
 
  ModelDefinitionTest() :
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
};

TEST_F(ModelDefinitionTest, prototypeObjectTest) {
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
  ModelTypeSpecifierFull* typeSpecifier =
  new ModelTypeSpecifierFull(packageExpression, "MMyModel", 0);
  vector<IObjectDefinition*> innerObjectDefinitions;
  ModelDefinition modelDefinition(AccessLevel::PUBLIC_ACCESS,
                                  typeSpecifier,
                                  mObjectElements,
                                  interfaces,
                                  innerObjectDefinitions,
                                  0);
  
  modelDefinition.prototypeObject(mContext, mContext.getImportProfile());

  Model* model = mContext.getModel("systems.vos.wisey.compiler.tests.MMyModel", 0);
  
  EXPECT_STREQ(model->getTypeName().c_str(), "systems.vos.wisey.compiler.tests.MMyModel");
  EXPECT_STREQ(model->getShortName().c_str(), "MMyModel");
  EXPECT_EQ(model->findMethod("foo"), nullptr);
}

TEST_F(ModelDefinitionTest, prototypeMethodsTest) {
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
  ModelTypeSpecifierFull* typeSpecifier =
  new ModelTypeSpecifierFull(packageExpression, "MMyModel", 0);
  vector<IObjectDefinition*> innerObjectDefinitions;
  ModelDefinition modelDefinition(AccessLevel::PUBLIC_ACCESS,
                                  typeSpecifier,
                                  mObjectElements,
                                  interfaces,
                                  innerObjectDefinitions,
                                  0);
  
  modelDefinition.prototypeObject(mContext, mContext.getImportProfile());
  modelDefinition.prototypeMethods(mContext);

  Model* model = mContext.getModel("systems.vos.wisey.compiler.tests.MMyModel", 0);

  EXPECT_NE(model->findMethod("foo"), nullptr);
}

TEST_F(ModelDefinitionTest, generateIRTest) {
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
  ModelTypeSpecifierFull* typeSpecifier =
  new ModelTypeSpecifierFull(packageExpression, "MMyModel", 0);
  vector<IObjectDefinition*> innerObjectDefinitions;
  ModelDefinition modelDefinition(AccessLevel::PUBLIC_ACCESS,
                                  typeSpecifier,
                                  mObjectElements,
                                  interfaces,
                                  innerObjectDefinitions,
                                  0);

  EXPECT_CALL(*mMockStatement, generateIR(_));
  
  modelDefinition.prototypeObject(mContext, mContext.getImportProfile());
  modelDefinition.prototypeMethods(mContext);
  modelDefinition.generateIR(mContext);
  Model* model = mContext.getModel("systems.vos.wisey.compiler.tests.MMyModel", 0);
  StructType* structType = (StructType*) model->getLLVMType(mContext)->getPointerElementType();
  
  ASSERT_NE(structType, nullptr);
  EXPECT_EQ(structType->getNumElements(), 3u);
  EXPECT_EQ(structType->getElementType(0),
            FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
            ->getPointerTo()->getPointerTo());
  EXPECT_EQ(structType->getElementType(1), Type::getInt64Ty(mLLVMContext));
  EXPECT_EQ(structType->getElementType(2), Type::getFloatTy(mLLVMContext));
  EXPECT_STREQ(model->getTypeName().c_str(), "systems.vos.wisey.compiler.tests.MMyModel");
  EXPECT_STREQ(model->getShortName().c_str(), "MMyModel");
  EXPECT_NE(model->findMethod("foo"), nullptr);
}

TEST_F(ModelDefinitionTest, interfaceImplmenetationDefinitionTest) {
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
  StructType *structType = StructType::create(mLLVMContext, interfaceFullName);
  vector<IObjectElementDefinition*> interfaceElements;
  methodArguments.clear();
  vector<IModelTypeSpecifier*> methodThrownExceptions;
  PackageType* packageType = new PackageType(Names::getLangPackageName());
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ModelTypeSpecifierFull* modelTypeSpecifier =
    new ModelTypeSpecifierFull(packageExpression, Names::getNPEModelName(), 0);
  methodThrownExceptions.push_back(modelTypeSpecifier);
  const PrimitiveTypeSpecifier* intSpecifier = PrimitiveTypes::INT->newTypeSpecifier(0);
  VariableDeclaration* methodArgument =
  VariableDeclaration::create(intSpecifier, new Identifier("intargument", 0), 0);
  methodArguments.push_back(methodArgument);
  const PrimitiveTypeSpecifier* floatSpecifier = PrimitiveTypes::FLOAT->newTypeSpecifier(0);
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
  ModelTypeSpecifierFull* typeSpecifier =
  new ModelTypeSpecifierFull(packageExpression, "MModel", 0);
  vector<IObjectDefinition*> innerObjectDefinitions;
  ModelDefinition modelDefinition(AccessLevel::PUBLIC_ACCESS,
                                  typeSpecifier,
                                  mObjectElements,
                                  interfaces,
                                  innerObjectDefinitions,
                                  0);
  modelDefinition.prototypeObject(mContext, mContext.getImportProfile());
  modelDefinition.prototypeMethods(mContext);
  modelDefinition.generateIR(mContext);
  
  GlobalVariable* vTablePointer = mContext.getModule()->
    getNamedGlobal("systems.vos.wisey.compiler.tests.MModel.vtable");
  
  ASSERT_NE(vTablePointer, nullptr);
  ASSERT_TRUE(vTablePointer->getType()->getPointerElementType()->isStructTy());
  EXPECT_EQ(vTablePointer->getType()->getPointerElementType()->getStructNumElements(), 1u);
  llvm::Constant* vTableInitializer = vTablePointer->getInitializer();
  ASSERT_TRUE(vTableInitializer->getType()->isStructTy());
  EXPECT_EQ(vTableInitializer->getType()->getStructNumElements(), 1u);

  GlobalVariable* vModelTypesPointer =
    mContext.getModule()->getNamedGlobal("systems.vos.wisey.compiler.tests.MModel.typetable");
  EXPECT_NE(vModelTypesPointer, nullptr);
  ASSERT_TRUE(vModelTypesPointer->getType()->getPointerElementType()->isArrayTy());
  EXPECT_EQ(vModelTypesPointer->getType()->getPointerElementType()->getArrayNumElements(), 4u);
}

TEST_F(ModelDefinitionTest, interfaceNotDefinedDeathTest) {
  vector<IInterfaceTypeSpecifier*> interfaces;
  string package = "systems.vos.wisey.compiler.tests";
  PackageType* packageType = new PackageType(package);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  interfaces.push_back(new InterfaceTypeSpecifier(packageExpression, "IMyInterface", 3));
  
  mObjectElements.push_back(mMethodDefinition);

  packageType = new PackageType(mPackage);
  packageExpression = new FakeExpression(NULL, packageType);
  ModelTypeSpecifierFull* typeSpecifier =
  new ModelTypeSpecifierFull(packageExpression, "MModel", 0);
  vector<IObjectDefinition*> innerObjectDefinitions;
  ModelDefinition modelDefinition(AccessLevel::PUBLIC_ACCESS,
                                  typeSpecifier,
                                  mObjectElements,
                                  interfaces,
                                  innerObjectDefinitions,
                                  0);
  modelDefinition.prototypeObject(mContext, mContext.getImportProfile());
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(modelDefinition.prototypeMethods(mContext));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Interface systems.vos.wisey.compiler.tests.IMyInterface is not defined\n",
              buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(ModelDefinitionTest, modelWithInjectedFieldDeathTest) {
  NiceMock<MockObjectType> object;
  ON_CALL(object, isController()).WillByDefault(Return(true));
  NiceMock<MockObjectTypeSpecifier>* objectSpecifier = new NiceMock<MockObjectTypeSpecifier>();
  ON_CALL(*objectSpecifier, getType(_)).WillByDefault(Return(&object));
  
  InjectionArgumentList arguments;
  InjectedFieldDefinition* field1 = new InjectedFieldDefinition(objectSpecifier,
                                                                "field1",
                                                                arguments,
                                                                1);
  mObjectElements.push_back(field1);
  
  mObjectElements.push_back(mMethodDefinition);

  vector<IInterfaceTypeSpecifier*> interfaces;
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ModelTypeSpecifierFull* typeSpecifier =
  new ModelTypeSpecifierFull(packageExpression, "MMyModel", 0);
  vector<IObjectDefinition*> innerObjectDefinitions;
  ModelDefinition modelDefinition(AccessLevel::PUBLIC_ACCESS,
                                  typeSpecifier,
                                  mObjectElements,
                                  interfaces,
                                  innerObjectDefinitions,
                                  0);
  modelDefinition.prototypeObject(mContext, mContext.getImportProfile());
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(modelDefinition.prototypeMethods(mContext));
  EXPECT_STREQ("/tmp/source.yz(1): Error: Models can only have receive fields\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, modelDefinitionRunTest) {
  runFile("tests/samples/test_model_definition.yz", "0");
}

TEST_F(TestFileRunner, modelDefinitionExplicitReceivedFieldsRunTest) {
  runFile("tests/samples/test_model_definition_explicit_received_fields.yz", "3");
}

TEST_F(TestFileRunner, modelDefinitionWithMethodRunTest) {
  runFile("tests/samples/test_model_method.yz", "7");
}

TEST_F(TestFileRunner, setterInModelDeathRunTest) {
  expectFailCompile("tests/samples/test_setter_in_model.yz",
                    1,
                    "tests/samples/test_setter_in_model.yz\\(13\\): "
                    "Error: Can not assign to received field mValue");
}

TEST_F(TestFileRunner, modelWithStateFieldDeathRunTest) {
  expectFailCompile("tests/samples/test_model_with_state_field.yz",
                    1,
                    "Error: Models can only have receive fields");
}
