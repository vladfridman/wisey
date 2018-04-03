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

#include "MockStatement.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/FixedFieldDefinition.hpp"
#include "wisey/FloatConstant.hpp"
#include "wisey/IObjectElementDefinition.hpp"
#include "wisey/InjectedFieldDefinition.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodDefinition.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/ModelDefinition.hpp"
#include "wisey/Names.hpp"
#include "wisey/PackageType.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/ProgramPrefix.hpp"
#include "wisey/ReturnStatement.hpp"
#include "wisey/VariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
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
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);

    mImportProfile = new ImportProfile(mPackage);
    mContext.setImportProfile(mImportProfile);
    
    mBlock->getStatements().push_back(mMockStatement);
    mBlock->getStatements().push_back(new ReturnStatement(new FloatConstant(0.5), 0));
    CompoundStatement* compoundStatement = new CompoundStatement(mBlock, 0);
    const PrimitiveTypeSpecifier* intTypeSpecifier = PrimitiveTypes::INT_TYPE->newTypeSpecifier();
    const PrimitiveTypeSpecifier* floatTypeSpecifier =
    PrimitiveTypes::FLOAT_TYPE->newTypeSpecifier();
    Identifier* intArgumentIdentifier = new Identifier("intargument");
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
                                               compoundStatement,
                                               0);
  }
};

TEST_F(ModelDefinitionTest, prototypeObjectTest) {
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
  ModelTypeSpecifierFull* typeSpecifier = new ModelTypeSpecifierFull(packageExpression, "MMyModel");
  vector<IObjectDefinition*> innerObjectDefinitions;
  ModelDefinition modelDefinition(AccessLevel::PUBLIC_ACCESS,
                                  typeSpecifier,
                                  mObjectElements,
                                  interfaces,
                                  innerObjectDefinitions);
  
  modelDefinition.prototypeObject(mContext);

  Model* model = mContext.getModel("systems.vos.wisey.compiler.tests.MMyModel");
  
  EXPECT_STREQ(model->getTypeName().c_str(), "systems.vos.wisey.compiler.tests.MMyModel");
  EXPECT_STREQ(model->getShortName().c_str(), "MMyModel");
  EXPECT_EQ(model->findMethod("foo"), nullptr);
}

TEST_F(ModelDefinitionTest, prototypeMethodsTest) {
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
  ModelTypeSpecifierFull* typeSpecifier = new ModelTypeSpecifierFull(packageExpression, "MMyModel");
  vector<IObjectDefinition*> innerObjectDefinitions;
  ModelDefinition modelDefinition(AccessLevel::PUBLIC_ACCESS,
                                  typeSpecifier,
                                  mObjectElements,
                                  interfaces,
                                  innerObjectDefinitions);
  
  modelDefinition.prototypeObject(mContext);
  modelDefinition.prototypeMethods(mContext);

  Model* model = mContext.getModel("systems.vos.wisey.compiler.tests.MMyModel");

  EXPECT_NE(model->findMethod("foo"), nullptr);
}

TEST_F(ModelDefinitionTest, generateIRTest) {
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
  ModelTypeSpecifierFull* typeSpecifier = new ModelTypeSpecifierFull(packageExpression, "MMyModel");
  vector<IObjectDefinition*> innerObjectDefinitions;
  ModelDefinition modelDefinition(AccessLevel::PUBLIC_ACCESS,
                                  typeSpecifier,
                                  mObjectElements,
                                  interfaces,
                                  innerObjectDefinitions);

  EXPECT_CALL(*mMockStatement, generateIR(_));
  
  modelDefinition.prototypeObject(mContext);
  modelDefinition.prototypeMethods(mContext);
  modelDefinition.generateIR(mContext);
  Model* model = mContext.getModel("systems.vos.wisey.compiler.tests.MMyModel");
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
  string interfaceFullName = "systems.vos.wisey.compiler.tests.IMyInterface";
  StructType *structType = StructType::create(mLLVMContext, interfaceFullName);
  vector<IObjectElementDefinition*> interfaceElements;
  VariableList methodArguments;
  vector<IModelTypeSpecifier*> methodThrownExceptions;
  PackageType* packageType = new PackageType(Names::getLangPackageName());
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ModelTypeSpecifierFull* modelTypeSpecifier =
    new ModelTypeSpecifierFull(packageExpression, Names::getNPEModelName());
  methodThrownExceptions.push_back(modelTypeSpecifier);
  const PrimitiveTypeSpecifier* intSpecifier = PrimitiveTypes::INT_TYPE->newTypeSpecifier();
  VariableDeclaration* methodArgument =
  VariableDeclaration::create(intSpecifier, new Identifier("intargument"), 0);
  methodArguments.push_back(methodArgument);
  const PrimitiveTypeSpecifier* floatSpecifier = PrimitiveTypes::FLOAT_TYPE->newTypeSpecifier();
  IObjectElementDefinition* methodSignature =
    new MethodSignatureDeclaration(floatSpecifier,
                                   "foo",
                                   methodArguments,
                                   methodThrownExceptions);
  interfaceElements.push_back(methodSignature);
  vector<IInterfaceTypeSpecifier*> parentInterfaces;
  Interface* interface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                 interfaceFullName,
                                                 structType,
                                                 parentInterfaces,
                                                 interfaceElements);
  mContext.addInterface(interface);
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
  interfaces.push_back(new InterfaceTypeSpecifier(NULL, "IMyInterface"));

  mObjectElements.push_back(mMethodDefinition);

  packageType = new PackageType(mPackage);
  packageExpression = new FakeExpression(NULL, packageType);
  ModelTypeSpecifierFull* typeSpecifier = new ModelTypeSpecifierFull(packageExpression, "MModel");
  vector<IObjectDefinition*> innerObjectDefinitions;
  ModelDefinition modelDefinition(AccessLevel::PUBLIC_ACCESS,
                                  typeSpecifier,
                                  mObjectElements,
                                  interfaces,
                                  innerObjectDefinitions);
  modelDefinition.prototypeObject(mContext);
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
  interfaces.push_back(new InterfaceTypeSpecifier(packageExpression, "IMyInterface"));
  
  mObjectElements.push_back(mMethodDefinition);

  packageType = new PackageType(mPackage);
  packageExpression = new FakeExpression(NULL, packageType);
  ModelTypeSpecifierFull* typeSpecifier = new ModelTypeSpecifierFull(packageExpression, "MModel");
  vector<IObjectDefinition*> innerObjectDefinitions;
  ModelDefinition modelDefinition(AccessLevel::PUBLIC_ACCESS,
                                  typeSpecifier,
                                  mObjectElements,
                                  interfaces,
                                  innerObjectDefinitions);
  modelDefinition.prototypeObject(mContext);
  
  EXPECT_EXIT(modelDefinition.prototypeMethods(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Interface systems.vos.wisey.compiler.tests.IMyInterface is not defined");
}

TEST_F(ModelDefinitionTest, modelWithInjectedFieldDeathTest) {
  const PrimitiveTypeSpecifier* longType = PrimitiveTypes::LONG_TYPE->newTypeSpecifier();
  InjectionArgumentList arguments;
  InjectedFieldDefinition* field1 = new InjectedFieldDefinition(longType, "field1", arguments);
  mObjectElements.push_back(field1);
  
  mObjectElements.push_back(mMethodDefinition);

  vector<IInterfaceTypeSpecifier*> interfaces;
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ModelTypeSpecifierFull* typeSpecifier = new ModelTypeSpecifierFull(packageExpression, "MMyModel");
  vector<IObjectDefinition*> innerObjectDefinitions;
  ModelDefinition modelDefinition(AccessLevel::PUBLIC_ACCESS,
                                  typeSpecifier,
                                  mObjectElements,
                                  interfaces,
                                  innerObjectDefinitions);
  modelDefinition.prototypeObject(mContext);
  
  EXPECT_EXIT(modelDefinition.prototypeMethods(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Models can only have fixed fields");
}

TEST_F(TestFileRunner, modelDefinitionRunTest) {
  runFile("tests/samples/test_model_definition.yz", "0");
}

TEST_F(TestFileRunner, modelDefinitionExplicitFixedFieldsRunTest) {
  runFile("tests/samples/test_model_definition_explicit_fixed_fields.yz", "3");
}

TEST_F(TestFileRunner, modelDefinitionWithMethodRunTest) {
  runFile("tests/samples/test_model_method.yz", "7");
}

TEST_F(TestFileRunner, setterInModelDeathRunTest) {
  expectFailCompile("tests/samples/test_setter_in_model.yz",
                    1,
                    "Error: Can not assign to field mValue");
}

TEST_F(TestFileRunner, modelWithStateFieldDeathRunTest) {
  expectFailCompile("tests/samples/test_model_with_state_field.yz",
                    1,
                    "Error: Models can only have fixed fields");
}
