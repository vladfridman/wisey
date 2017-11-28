//
//  TestControllerDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ControllerDefinition}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockStatement.hpp"
#include "TestFileSampleRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/AccessLevel.hpp"
#include "wisey/ControllerDefinition.hpp"
#include "wisey/FieldDeclaration.hpp"
#include "wisey/FloatConstant.hpp"
#include "wisey/Interface.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodDeclaration.hpp"
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
using ::testing::Return;
using ::testing::Test;

struct ControllerDefinitionTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockStatement>* mMockStatement;
  vector<IObjectElementDeclaration*> mElementDeclarations;
  vector<InterfaceTypeSpecifier*> mInterfaces;
  
  ControllerDefinitionTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mMockStatement(new NiceMock<MockStatement>()) {
    TestPrefix::generateIR(mContext);
    
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);

    MethodDeclaration *methodDeclaration;
    Block* block = new Block();

    mContext.setPackage("systems.vos.wisey.compiler.tests");
    block->getStatements().push_back(mMockStatement);
    block->getStatements().push_back(new ReturnStatement(new FloatConstant(0.5)));
    CompoundStatement* compoundStatement = new CompoundStatement(block);
    PrimitiveTypeSpecifier* intTypeSpecifier =
    new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
    PrimitiveTypeSpecifier* floatTypeSpecifier =
    new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
    Identifier* intArgumentIdentifier = new Identifier("intargument");
    VariableDeclaration* intArgument =
    new VariableDeclaration(intTypeSpecifier, intArgumentIdentifier);
    VariableList methodArguments;
    methodArguments.push_back(intArgument);
    vector<ModelTypeSpecifier*> thrownExceptions;
    methodDeclaration = new MethodDeclaration(AccessLevel::PUBLIC_ACCESS,
                                              floatTypeSpecifier,
                                              "foo",
                                              methodArguments,
                                              thrownExceptions,
                                              compoundStatement,
                                              0);

    PrimitiveTypeSpecifier* longType = new PrimitiveTypeSpecifier(PrimitiveTypes::LONG_TYPE);
    PrimitiveTypeSpecifier* floatType = new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
    ExpressionList arguments;
    FieldDeclaration* field1 = new FieldDeclaration(RECEIVED_FIELD, longType, "field1", arguments);
    FieldDeclaration* field2 = new FieldDeclaration(RECEIVED_FIELD, floatType, "field2", arguments);
    mElementDeclarations.push_back(field1);
    mElementDeclarations.push_back(field2);
    mElementDeclarations.push_back(methodDeclaration);
  }
  
  ~ControllerDefinitionTest() {
  }
};

TEST_F(ControllerDefinitionTest, controllerDefinitionPrototypeObjectsTest) {
  vector<string> package;
  ControllerTypeSpecifier* typeSpecifier = new ControllerTypeSpecifier(package, "CMyController");
  ControllerDefinition controllerDefinition(typeSpecifier, mElementDeclarations, mInterfaces);

  EXPECT_CALL(*mMockStatement, generateIR(_)).Times(0);

  controllerDefinition.prototypeObjects(mContext);
  
  ASSERT_NE(mContext.getController("systems.vos.wisey.compiler.tests.CMyController"), nullptr);

  Controller* controller = mContext.getController("systems.vos.wisey.compiler.tests.CMyController");
  
  EXPECT_STREQ(controller->getShortName().c_str(), "CMyController");
  EXPECT_STREQ(controller->getName().c_str(), "systems.vos.wisey.compiler.tests.CMyController");
  EXPECT_EQ(controller->findMethod("foo"), nullptr);
}

TEST_F(ControllerDefinitionTest, controllerDefinitionPrototypeMethodsTest) {
  vector<string> package;
  ControllerTypeSpecifier* typeSpecifier = new ControllerTypeSpecifier(package, "CMyController");
  ControllerDefinition controllerDefinition(typeSpecifier, mElementDeclarations, mInterfaces);

  EXPECT_CALL(*mMockStatement, generateIR(_)).Times(0);
  
  controllerDefinition.prototypeObjects(mContext);
  controllerDefinition.prototypeMethods(mContext);
  
  Controller* controller = mContext.getController("systems.vos.wisey.compiler.tests.CMyController");
  EXPECT_NE(controller->findMethod("foo"), nullptr);
}

TEST_F(ControllerDefinitionTest, controllerDefinitionGenerateIRTest) {
  vector<string> package;
  ControllerTypeSpecifier* typeSpecifier = new ControllerTypeSpecifier(package, "CMyController");
  ControllerDefinition controllerDefinition(typeSpecifier, mElementDeclarations, mInterfaces);

  EXPECT_CALL(*mMockStatement, generateIR(_));
  
  controllerDefinition.prototypeObjects(mContext);
  controllerDefinition.prototypeMethods(mContext);
  controllerDefinition.generateIR(mContext);
  
  ASSERT_NE(mContext.getController("systems.vos.wisey.compiler.tests.CMyController"), nullptr);
  
  Controller* controller = mContext.getController("systems.vos.wisey.compiler.tests.CMyController");
  StructType* structType = (StructType*) controller->getLLVMType(mLLVMContext)
    ->getPointerElementType();

  ASSERT_NE(structType, nullptr);
  EXPECT_EQ(structType->getNumElements(), 3u);
  // reference counter
  EXPECT_EQ(structType->getElementType(0), Type::getInt64Ty(mLLVMContext));
  EXPECT_EQ(structType->getElementType(1), Type::getInt64Ty(mLLVMContext));
  EXPECT_EQ(structType->getElementType(2), Type::getFloatTy(mLLVMContext));
  EXPECT_STREQ(controller->getShortName().c_str(), "CMyController");
  EXPECT_STREQ(controller->getName().c_str(), "systems.vos.wisey.compiler.tests.CMyController");
  EXPECT_NE(controller->findMethod("foo"), nullptr);
}

TEST_F(ControllerDefinitionTest, controllerWithFixedFieldDeathTest) {
  vector<string> package;
  ControllerTypeSpecifier* typeSpecifier = new ControllerTypeSpecifier(package, "CMyController");
  ExpressionList arguments;
  PrimitiveTypeSpecifier* intType = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
  FieldDeclaration* field = new FieldDeclaration(FIXED_FIELD, intType, "field3", arguments);
  mElementDeclarations.clear();
  mElementDeclarations.push_back(field);
  ControllerDefinition controllerDefinition(typeSpecifier, mElementDeclarations, mInterfaces);
  controllerDefinition.prototypeObjects(mContext);
  
  EXPECT_EXIT(controllerDefinition.prototypeMethods(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Controllers can only have fixed, injected or state fields");
}

TEST_F(ControllerDefinitionTest, fieldsDeclaredAfterMethodsDeathTest) {
  vector<string> package;
  ControllerTypeSpecifier* typeSpecifier = new ControllerTypeSpecifier(package, "CMyController");
  ExpressionList arguments;
  PrimitiveTypeSpecifier* intType = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
  FieldDeclaration* field = new FieldDeclaration(FIXED_FIELD, intType, "field3", arguments);
  mElementDeclarations.push_back(field);
  ControllerDefinition controllerDefinition(typeSpecifier, mElementDeclarations, mInterfaces);
  controllerDefinition.prototypeObjects(mContext);
  
  EXPECT_EXIT(controllerDefinition.prototypeMethods(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Fields should be declared before methods");
}

TEST_F(TestFileSampleRunner, controllerDefinitionSyntaxRunTest) {
  runFile("tests/samples/test_controller_definition.yz", "8");
}

TEST_F(TestFileSampleRunner, controllerDefinitionWithModelStateSyntaxRunTest) {
  runFile("tests/samples/test_controller_definition_with_model_state.yz", "21");
}

TEST_F(TestFileSampleRunner, controllerDefinitionWithInjectedInterfaceFieldRunTest) {
  runFile("tests/samples/test_controller_definition_with_injected_interface_field.yz", "8");
}

TEST_F(TestFileSampleRunner, controllerWithFixedFieldDeathRunTest) {
  expectFailCompile("tests/samples/test_controller_with_fixed_field.yz",
                    1,
                    "Error: Controllers can only have fixed, injected or state fields");
}

TEST_F(TestFileSampleRunner, objectFieldsAfterMethodsDeathRunTest) {
  expectFailCompile("tests/samples/test_object_fields_after_methods.yz",
                    1,
                    "Error: Fields should be declared before methods");
}
