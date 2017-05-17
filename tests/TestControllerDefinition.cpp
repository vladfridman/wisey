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
#include "wisey/IRGenerationContext.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodDeclaration.hpp"
#include "wisey/MethodSignature.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"

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
  MethodDeclaration *mMethodDeclaration;
  vector<ControllerFieldDeclaration*> mReceivedFields;
  vector<ControllerFieldDeclaration*> mInjectedFields;
  vector<ControllerFieldDeclaration*> mStateFields;
  vector<MethodDeclaration*> mMethodDeclarations;
  Block mBlock;
  NiceMock<MockStatement> mMockStatement;
  
  ControllerDefinitionTest() : mLLVMContext(mContext.getLLVMContext()) {
    mBlock.getStatements().push_back(&mMockStatement);
    CompoundStatement* compoundStatement = new CompoundStatement(mBlock);
    PrimitiveTypeSpecifier* intTypeSpecifier =
    new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
    PrimitiveTypeSpecifier* floatTypeSpecifier =
    new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
    Identifier* intArgumentIdentifier = new Identifier("intargument");
    VariableDeclaration* intArgument =
    new VariableDeclaration(*intTypeSpecifier, *intArgumentIdentifier);
    VariableList methodArguments;
    methodArguments.push_back(intArgument);
    vector<ITypeSpecifier*> thrownExceptions;
    mMethodDeclaration = new MethodDeclaration(AccessLevel::PUBLIC_ACCESS,
                                               *floatTypeSpecifier,
                                               "foo",
                                               methodArguments,
                                               thrownExceptions,
                                               *compoundStatement);
    mMethodDeclarations.push_back(mMethodDeclaration);
  }
};

TEST_F(ControllerDefinitionTest, simpleDefinitionTest) {
  PrimitiveTypeSpecifier* longType = new PrimitiveTypeSpecifier(PrimitiveTypes::LONG_TYPE);
  PrimitiveTypeSpecifier* floatType = new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
  ExpressionList arguments;
  ControllerFieldDeclaration* field1 =
    new ControllerFieldDeclaration(RECEIVED_FIELD, *longType, "field1", arguments);
  ControllerFieldDeclaration* field2 =
    new ControllerFieldDeclaration(RECEIVED_FIELD, *floatType, "field2", arguments);
  mReceivedFields.push_back(field1);
  mReceivedFields.push_back(field2);
  
  vector<string> interfaces;
  ControllerDefinition controllerDefinition("CMyController",
                                            mReceivedFields,
                                            mInjectedFields,
                                            mStateFields,
                                            mMethodDeclarations,
                                            interfaces);
  
  EXPECT_CALL(mMockStatement, generateIR(_));

  controllerDefinition.generateIR(mContext);
  
  ASSERT_NE(mContext.getController("CMyController"), nullptr);

  Controller* controller = mContext.getController("CMyController");
  StructType* structType =
    (StructType*) controller->getLLVMType(mLLVMContext)->getPointerElementType();
  
  ASSERT_NE(structType, nullptr);
  EXPECT_TRUE(structType->getNumElements() == 2);
  EXPECT_EQ(structType->getElementType(0), Type::getInt64Ty(mLLVMContext));
  EXPECT_EQ(structType->getElementType(1), Type::getFloatTy(mLLVMContext));
  EXPECT_STREQ(controller->getName().c_str(), "CMyController");
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

