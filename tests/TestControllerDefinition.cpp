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
#include "wisey/AccessLevel.hpp"
#include "wisey/ControllerDefinition.hpp"
#include "wisey/Interface.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodDeclaration.hpp"
#include "wisey/MethodSignature.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/ProgramPrefix.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Test;

struct ControllerDefinitionTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockStatement>* mMockStatement;
  vector<FieldDeclaration*> mReceivedFields;
  vector<FieldDeclaration*> mInjectedFields;
  vector<FieldDeclaration*> mStateFields;
  vector<MethodDeclaration*> mMethodDeclarations;
  vector<InterfaceTypeSpecifier*> mInterfaces;
  
  ControllerDefinitionTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mMockStatement(new NiceMock<MockStatement>()) {
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);

    MethodDeclaration *methodDeclaration;
    Block* block = new Block();

    mContext.setPackage("systems.vos.wisey.compiler.tests");
    block->getStatements().push_back(mMockStatement);
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
                                              compoundStatement);
    mMethodDeclarations.push_back(methodDeclaration);

    PrimitiveTypeSpecifier* longType = new PrimitiveTypeSpecifier(PrimitiveTypes::LONG_TYPE);
    PrimitiveTypeSpecifier* floatType = new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
    ExpressionList arguments;
    FieldDeclaration* field1 =
    new FieldDeclaration(RECEIVED_FIELD, longType, "field1", arguments);
    FieldDeclaration* field2 =
    new FieldDeclaration(RECEIVED_FIELD, floatType, "field2", arguments);
    mReceivedFields.push_back(field1);
    mReceivedFields.push_back(field2);
  }
  
  ~ControllerDefinitionTest() {
  }
};

TEST_F(ControllerDefinitionTest, controllerDefinitionPrototypeObjectsTest) {
  ControllerDefinition controllerDefinition("CMyController",
                                            mReceivedFields,
                                            mInjectedFields,
                                            mStateFields,
                                            mMethodDeclarations,
                                            mInterfaces);

  EXPECT_CALL(*mMockStatement, generateIR(_)).Times(0);

  controllerDefinition.prototypeObjects(mContext);
  
  ASSERT_NE(mContext.getController("systems.vos.wisey.compiler.tests.CMyController"), nullptr);

  Controller* controller = mContext.getController("systems.vos.wisey.compiler.tests.CMyController");
  
  EXPECT_STREQ(controller->getShortName().c_str(), "CMyController");
  EXPECT_STREQ(controller->getName().c_str(), "systems.vos.wisey.compiler.tests.CMyController");
  EXPECT_EQ(controller->findMethod("foo"), nullptr);
}

TEST_F(ControllerDefinitionTest, controllerDefinitionPrototypeMethodsTest) {
  ControllerDefinition controllerDefinition("CMyController",
                                            mReceivedFields,
                                            mInjectedFields,
                                            mStateFields,
                                            mMethodDeclarations,
                                            mInterfaces);

  EXPECT_CALL(*mMockStatement, generateIR(_)).Times(0);
  
  controllerDefinition.prototypeObjects(mContext);
  controllerDefinition.prototypeMethods(mContext);
  
  Controller* controller = mContext.getController("systems.vos.wisey.compiler.tests.CMyController");
  EXPECT_NE(controller->findMethod("foo"), nullptr);
}

TEST_F(ControllerDefinitionTest, controllerDefinitionGenerateIRTest) {
  ControllerDefinition controllerDefinition("CMyController",
                                            mReceivedFields,
                                            mInjectedFields,
                                            mStateFields,
                                            mMethodDeclarations,
                                            mInterfaces);

  EXPECT_CALL(*mMockStatement, generateIR(_));
  
  controllerDefinition.prototypeObjects(mContext);
  controllerDefinition.prototypeMethods(mContext);
  controllerDefinition.generateIR(mContext);
  
  ASSERT_NE(mContext.getController("systems.vos.wisey.compiler.tests.CMyController"), nullptr);
  
  Controller* controller = mContext.getController("systems.vos.wisey.compiler.tests.CMyController");
  StructType* structType = (StructType*) controller->getLLVMType(mLLVMContext)
  ->getPointerElementType()->getPointerElementType();

  ASSERT_NE(structType, nullptr);
  ASSERT_EQ(structType->getNumElements(), 2u);
  EXPECT_EQ(structType->getElementType(0), Type::getInt64Ty(mLLVMContext));
  EXPECT_EQ(structType->getElementType(1), Type::getFloatTy(mLLVMContext));
  EXPECT_STREQ(controller->getShortName().c_str(), "CMyController");
  EXPECT_STREQ(controller->getName().c_str(), "systems.vos.wisey.compiler.tests.CMyController");
  EXPECT_NE(controller->findMethod("foo"), nullptr);
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

