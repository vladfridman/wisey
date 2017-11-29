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
#include "TestFileSampleRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/FieldDeclaration.hpp"
#include "wisey/FloatConstant.hpp"
#include "wisey/IObjectElementDeclaration.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodDeclaration.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/ModelDefinition.hpp"
#include "wisey/Names.hpp"
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
  MethodDeclaration *mMethodDeclaration;
  vector<IObjectElementDeclaration*> mObjectElements;
  Block* mBlock;
  NiceMock<MockStatement>* mMockStatement;
 
  ModelDefinitionTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mBlock(new Block()),
  mMockStatement(new NiceMock<MockStatement>()) {
    TestPrefix::generateIR(mContext);
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);

    mContext.setPackage("systems.vos.wisey.compiler.tests");
    mBlock->getStatements().push_back(mMockStatement);
    mBlock->getStatements().push_back(new ReturnStatement(new FloatConstant(0.5), 0));
    CompoundStatement* compoundStatement = new CompoundStatement(mBlock, 0);
    PrimitiveTypeSpecifier* intTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
    PrimitiveTypeSpecifier* floatTypeSpecifier =
      new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
    Identifier* intArgumentIdentifier = new Identifier("intargument");
    VariableDeclaration* intArgument =
    VariableDeclaration::create(intTypeSpecifier, intArgumentIdentifier, 0);
    VariableList methodArguments;
    methodArguments.push_back(intArgument);
    vector<ModelTypeSpecifier*> thrownExceptions;
    mMethodDeclaration = new MethodDeclaration(AccessLevel::PUBLIC_ACCESS,
                                               floatTypeSpecifier,
                                               "foo",
                                               methodArguments,
                                               thrownExceptions,
                                               compoundStatement,
                                               0);
  }
};

TEST_F(ModelDefinitionTest, prototypeObjectsTest) {
  PrimitiveTypeSpecifier* longType = new PrimitiveTypeSpecifier(PrimitiveTypes::LONG_TYPE);
  PrimitiveTypeSpecifier* floatType = new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
  vector<IExpression*> arguments;
  FieldDeclaration* field1 = new FieldDeclaration(FIXED_FIELD, longType, "field1", arguments);
  FieldDeclaration* field2 = new FieldDeclaration(FIXED_FIELD, floatType, "field2", arguments);
  mObjectElements.push_back(field1);
  mObjectElements.push_back(field2);
  mObjectElements.push_back(mMethodDeclaration);

  vector<InterfaceTypeSpecifier*> interfaces;
  ModelTypeSpecifier* typeSpecifier = new ModelTypeSpecifier("", "MMyModel");
  ModelDefinition modelDefinition(typeSpecifier, mObjectElements, interfaces);
  
  modelDefinition.prototypeObjects(mContext);

  Model* model = mContext.getModel("systems.vos.wisey.compiler.tests.MMyModel");
  
  EXPECT_STREQ(model->getName().c_str(), "systems.vos.wisey.compiler.tests.MMyModel");
  EXPECT_STREQ(model->getShortName().c_str(), "MMyModel");
  EXPECT_EQ(model->findMethod("foo"), nullptr);
}

TEST_F(ModelDefinitionTest, prototypeMethodsTest) {
  PrimitiveTypeSpecifier* longType = new PrimitiveTypeSpecifier(PrimitiveTypes::LONG_TYPE);
  PrimitiveTypeSpecifier* floatType = new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
  vector<IExpression*> arguments;
  FieldDeclaration* field1 = new FieldDeclaration(FIXED_FIELD, longType, "field1", arguments);
  FieldDeclaration* field2 = new FieldDeclaration(FIXED_FIELD, floatType, "field2", arguments);
  mObjectElements.push_back(field1);
  mObjectElements.push_back(field2);
  mObjectElements.push_back(mMethodDeclaration);

  vector<InterfaceTypeSpecifier*> interfaces;
  ModelTypeSpecifier* typeSpecifier = new ModelTypeSpecifier("", "MMyModel");
  ModelDefinition modelDefinition(typeSpecifier, mObjectElements, interfaces);
  
  modelDefinition.prototypeObjects(mContext);
  modelDefinition.prototypeMethods(mContext);

  Model* model = mContext.getModel("systems.vos.wisey.compiler.tests.MMyModel");

  EXPECT_NE(model->findMethod("foo"), nullptr);
}

TEST_F(ModelDefinitionTest, generateIRTest) {
  PrimitiveTypeSpecifier* longType = new PrimitiveTypeSpecifier(PrimitiveTypes::LONG_TYPE);
  PrimitiveTypeSpecifier* floatType = new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
  vector<IExpression*> arguments;
  FieldDeclaration* field1 = new FieldDeclaration(FIXED_FIELD, longType, "field1", arguments);
  FieldDeclaration* field2 = new FieldDeclaration(FIXED_FIELD, floatType, "field2", arguments);
  mObjectElements.push_back(field1);
  mObjectElements.push_back(field2);
  mObjectElements.push_back(mMethodDeclaration);

  vector<InterfaceTypeSpecifier*> interfaces;
  ModelTypeSpecifier* typeSpecifier = new ModelTypeSpecifier("", "MMyModel");
  ModelDefinition modelDefinition(typeSpecifier, mObjectElements, interfaces);

  EXPECT_CALL(*mMockStatement, generateIR(_));
  
  modelDefinition.prototypeObjects(mContext);
  modelDefinition.prototypeMethods(mContext);
  modelDefinition.generateIR(mContext);
  Model* model = mContext.getModel("systems.vos.wisey.compiler.tests.MMyModel");
  StructType* structType = (StructType*) model->getLLVMType(mLLVMContext)->getPointerElementType();
  
  ASSERT_NE(structType, nullptr);
  EXPECT_EQ(structType->getNumElements(), 3u);
  // reference counter
  EXPECT_EQ(structType->getElementType(0), Type::getInt64Ty(mLLVMContext));
  EXPECT_EQ(structType->getElementType(1), Type::getInt64Ty(mLLVMContext));
  EXPECT_EQ(structType->getElementType(2), Type::getFloatTy(mLLVMContext));
  EXPECT_STREQ(model->getName().c_str(), "systems.vos.wisey.compiler.tests.MMyModel");
  EXPECT_STREQ(model->getShortName().c_str(), "MMyModel");
  EXPECT_NE(model->findMethod("foo"), nullptr);
}

TEST_F(ModelDefinitionTest, interfaceImplmenetationDefinitionTest) {
  string interfaceFullName = "systems.vos.wisey.compiler.tests.IMyInterface";
  StructType *structType = StructType::create(mLLVMContext, interfaceFullName);
  vector<IObjectElementDeclaration*> interfaceElements;
  VariableList methodArguments;
  vector<ModelTypeSpecifier*> methodThrownExceptions;
  ModelTypeSpecifier* modelTypeSpecifier = new ModelTypeSpecifier(Names::getLangPackageName(),
                                                                  Names::getNPEModelName());
  methodThrownExceptions.push_back(modelTypeSpecifier);
  const PrimitiveTypeSpecifier* intSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
  VariableDeclaration* methodArgument =
  VariableDeclaration::create(intSpecifier, new Identifier("intargument"), 0);
  methodArguments.push_back(methodArgument);
  PrimitiveTypeSpecifier* floatSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
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
  interfaces.push_back(new InterfaceTypeSpecifier("", "IMyInterface"));

  mObjectElements.push_back(mMethodDeclaration);

  ModelTypeSpecifier* typeSpecifier = new ModelTypeSpecifier("", "MModel");
  ModelDefinition modelDefinition(typeSpecifier, mObjectElements, interfaces);
  modelDefinition.prototypeObjects(mContext);
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
  EXPECT_EQ(vModelTypesPointer->getType()->getPointerElementType()->getArrayNumElements(), 3u);
}

TEST_F(ModelDefinitionTest, interfaceNotDefinedDeathTest) {
  vector<InterfaceTypeSpecifier*> interfaces;
  string package = "systems.vos.wisey.compiler.tests";
  interfaces.push_back(new InterfaceTypeSpecifier(package, "IMyInterface"));
  
  mObjectElements.push_back(mMethodDeclaration);

  ModelTypeSpecifier* typeSpecifier = new ModelTypeSpecifier("", "MModel");
  ModelDefinition modelDefinition(typeSpecifier, mObjectElements, interfaces);
  modelDefinition.prototypeObjects(mContext);
  
  EXPECT_EXIT(modelDefinition.prototypeMethods(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Interface systems.vos.wisey.compiler.tests.IMyInterface is not defined");
}

TEST_F(ModelDefinitionTest, modelWithInjectedFieldDeathTest) {
  PrimitiveTypeSpecifier* longType = new PrimitiveTypeSpecifier(PrimitiveTypes::LONG_TYPE);
  vector<IExpression*> arguments;
  FieldDeclaration* field1 = new FieldDeclaration(INJECTED_FIELD, longType, "field1", arguments);
  mObjectElements.push_back(field1);
  
  mObjectElements.push_back(mMethodDeclaration);

  vector<InterfaceTypeSpecifier*> interfaces;
  ModelTypeSpecifier* typeSpecifier = new ModelTypeSpecifier("", "MMyModel");
  ModelDefinition modelDefinition(typeSpecifier, mObjectElements, interfaces);
  modelDefinition.prototypeObjects(mContext);
  
  EXPECT_EXIT(modelDefinition.prototypeMethods(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Models can only have fixed fields");
}

TEST_F(TestFileSampleRunner, modelDefinitionRunTest) {
  runFile("tests/samples/test_model_definition.yz", "0");
}

TEST_F(TestFileSampleRunner, modelDefinitionExplicitFixedFieldsRunTest) {
  runFile("tests/samples/test_model_definition_explicit_fixed_fields.yz", "3");
}

TEST_F(TestFileSampleRunner, modelDefinitionWithMethodRunTest) {
  runFile("tests/samples/test_model_method.yz", "7");
}

TEST_F(TestFileSampleRunner, setterInModelDeathRunTest) {
  expectFailCompile("tests/samples/test_setter_in_model.yz",
                    1,
                    "Error: Can not assign to field mValue");
}

TEST_F(TestFileSampleRunner, modelWithStateFieldDeathRunTest) {
  expectFailCompile("tests/samples/test_model_with_state_field.yz",
                    1,
                    "Error: Models can only have fixed fields");
}
