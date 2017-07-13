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
#include "wisey/MethodArgument.hpp"
#include "wisey/ModelDefinition.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"

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
  vector<FieldDeclaration*> mFields;
  vector<MethodDeclaration*> mMethodDeclarations;
  Block* mBlock;
  NiceMock<MockStatement>* mMockStatement;
 
  ModelDefinitionTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mBlock(new Block()),
  mMockStatement(new NiceMock<MockStatement>()) {
    mContext.setPackage("systems.vos.wisey.compiler.tests");
    mBlock->getStatements().push_back(mMockStatement);
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
  
  ~ModelDefinitionTest() {
  }
};

TEST_F(ModelDefinitionTest, prototypeObjectsTest) {
  PrimitiveTypeSpecifier* longType = new PrimitiveTypeSpecifier(PrimitiveTypes::LONG_TYPE);
  PrimitiveTypeSpecifier* floatType = new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
  vector<IExpression*> arguments;
  FieldDeclaration* field1 = new FieldDeclaration(FIXED_FIELD, longType, "field1", arguments);
  FieldDeclaration* field2 = new FieldDeclaration(FIXED_FIELD, floatType, "field2", arguments);
  mFields.push_back(field1);
  mFields.push_back(field2);
  
  vector<InterfaceTypeSpecifier*> interfaces;
  ModelDefinition modelDefinition("MMyModel", mFields, mMethodDeclarations, interfaces);
  
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
  mFields.push_back(field1);
  mFields.push_back(field2);
  
  vector<InterfaceTypeSpecifier*> interfaces;
  ModelDefinition modelDefinition("MMyModel", mFields, mMethodDeclarations, interfaces);
  
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
  mFields.push_back(field1);
  mFields.push_back(field2);
  
  vector<InterfaceTypeSpecifier*> interfaces;
  ModelDefinition modelDefinition("MMyModel", mFields, mMethodDeclarations, interfaces);

  EXPECT_CALL(*mMockStatement, generateIR(_));
  
  modelDefinition.prototypeObjects(mContext);
  modelDefinition.prototypeMethods(mContext);
  modelDefinition.generateIR(mContext);
  Model* model = mContext.getModel("systems.vos.wisey.compiler.tests.MMyModel");
  StructType* structType = (StructType*) model->getLLVMType(mLLVMContext)->getPointerElementType();
  
  ASSERT_NE(structType, nullptr);
  EXPECT_TRUE(structType->getNumElements() == 2);
  EXPECT_EQ(structType->getElementType(0), Type::getInt64Ty(mLLVMContext));
  EXPECT_EQ(structType->getElementType(1), Type::getFloatTy(mLLVMContext));
  EXPECT_STREQ(model->getName().c_str(), "systems.vos.wisey.compiler.tests.MMyModel");
  EXPECT_STREQ(model->getShortName().c_str(), "MMyModel");
  EXPECT_NE(model->findMethod("foo"), nullptr);
}

TEST_F(ModelDefinitionTest, interfaceImplmenetationDefinitionTest) {
  string interfaceFullName = "systems.vos.wisey.compiler.tests.IMyInterface";
  StructType *structType = StructType::create(mLLVMContext, interfaceFullName);
  vector<Type*> types;
  Type* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), true);
  Type* vtableType = functionType->getPointerTo()->getPointerTo();
  types.push_back(vtableType);
  structType->setBody(types);
  vector<MethodSignature*> interfaceMethodSignatures;
  vector<MethodArgument*> methodArguments;
  vector<const Model*> methodThrownExceptions;
  methodArguments.push_back(new MethodArgument(PrimitiveTypes::INT_TYPE, "intargument"));
  MethodSignature* methodSignature = new MethodSignature("foo",
                                                         AccessLevel::PUBLIC_ACCESS,
                                                         PrimitiveTypes::FLOAT_TYPE,
                                                         methodArguments,
                                                         methodThrownExceptions,
                                                         0);
  interfaceMethodSignatures.push_back(methodSignature);
  Interface* interface = new Interface(interfaceFullName, structType);
  vector<Interface*> parentInterfaces;
  interface->setParentInterfacesAndMethodSignatures(parentInterfaces, interfaceMethodSignatures);
  Constant* stringConstant = ConstantDataArray::getString(mLLVMContext, interface->getName());
  new GlobalVariable(*mContext.getModule(),
                     stringConstant->getType(),
                     true,
                     GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                     stringConstant,
                     interface->getObjectNameGlobalVariableName());

  mContext.addInterface(interface);
  vector<InterfaceTypeSpecifier*> interfaces;
  vector<string> package;
  interfaces.push_back(new InterfaceTypeSpecifier(package, "IMyInterface"));
  
  ModelDefinition modelDefinition("MModel", mFields, mMethodDeclarations, interfaces);
  modelDefinition.prototypeObjects(mContext);
  modelDefinition.prototypeMethods(mContext);
  modelDefinition.generateIR(mContext);
  
  GlobalVariable* vTablePointer = mContext.getModule()->
  getGlobalVariable("systems.vos.wisey.compiler.tests.MModel.vtable");
  
  ASSERT_NE(vTablePointer, nullptr);
  ASSERT_TRUE(vTablePointer->getType()->getPointerElementType()->isStructTy());
  EXPECT_EQ(vTablePointer->getType()->getPointerElementType()->getStructNumElements(), 1u);
  Constant* vTableInitializer = vTablePointer->getInitializer();
  ASSERT_TRUE(vTableInitializer->getType()->isStructTy());
  EXPECT_EQ(vTableInitializer->getType()->getStructNumElements(), 1u);

  GlobalVariable* vModelTypesPointer =
    mContext.getModule()->getGlobalVariable("systems.vos.wisey.compiler.tests.MModel.typetable");
  EXPECT_NE(vModelTypesPointer, nullptr);
  ASSERT_TRUE(vModelTypesPointer->getType()->getPointerElementType()->isArrayTy());
  EXPECT_EQ(vModelTypesPointer->getType()->getPointerElementType()->getArrayNumElements(), 3u);
}

TEST_F(ModelDefinitionTest, interfaceNotDefinedDeathTest) {
  vector<InterfaceTypeSpecifier*> interfaces;
  vector<string> package;
  package.push_back("systems");
  package.push_back("vos");
  package.push_back("wisey");
  package.push_back("compiler");
  package.push_back("tests");
  interfaces.push_back(new InterfaceTypeSpecifier(package, "IMyInterface"));
  
  ModelDefinition modelDefinition("MModel", mFields, mMethodDeclarations, interfaces);
  modelDefinition.prototypeObjects(mContext);
  
  EXPECT_EXIT(modelDefinition.prototypeMethods(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Interface systems.vos.wisey.compiler.tests.IMyInterface is not defined");
}

TEST_F(TestFileSampleRunner, modelDefinitionRunTest) {
  runFile("tests/samples/test_model_definition.yz", "0");
}

TEST_F(TestFileSampleRunner, modelDefinitionWithMethodRunTest) {
  runFile("tests/samples/test_model_method.yz", "7");
}
