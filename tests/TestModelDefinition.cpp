//
//  TestModelDefinition.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ModelDefinition}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/AccessSpecifiers.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/MethodDeclaration.hpp"
#include "yazyk/ModelDefinition.hpp"
#include "yazyk/PrimitiveTypes.hpp"
#include "yazyk/PrimitiveTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;

class MockStatement : public IStatement {
public:
  MOCK_CONST_METHOD1(generateIR, Value* (IRGenerationContext&));
};

TEST(ModelDefinitionTest, TestSimpleDefinition) {
  IRGenerationContext context;
  LLVMContext& llvmContext = context.getLLVMContext();
  PrimitiveTypeSpecifier longType(PrimitiveTypes::LONG_TYPE);
  PrimitiveTypeSpecifier floatType(PrimitiveTypes::FLOAT_TYPE);
  ModelFieldDeclaration field1(longType, "field1");
  ModelFieldDeclaration field2(floatType, "field2");
  vector<ModelFieldDeclaration *> fields;
  fields.push_back(&field1);
  fields.push_back(&field2);
  
  Block block;
  NiceMock<MockStatement> mockStatement;
  block.getStatements().push_back(&mockStatement);
  CompoundStatement compoundStatement(block);
  PrimitiveTypeSpecifier intTypeSpecifier(PrimitiveTypes::INT_TYPE);
  PrimitiveTypeSpecifier floatTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
  Identifier fooFunctionIdentifier("foo");
  Identifier intArgumentIdentifier("intargument");
  VariableDeclaration intArgument(intTypeSpecifier, intArgumentIdentifier);
  VariableList methodArguments;
  methodArguments.push_back(&intArgument);
  MethodDeclaration methodDeclaration(AccessSpecifiers::PUBLIC_ACCESS,
                                      floatTypeSpecifier,
                                      fooFunctionIdentifier,
                                      methodArguments,
                                      compoundStatement);
  vector<MethodDeclaration *> methods;
  methods.push_back(&methodDeclaration);
  
  ModelDefinition modelDefinition("mymodel", fields, methods);

  EXPECT_CALL(mockStatement, generateIR(_));
  
  modelDefinition.generateIR(context);
  Model* model = context.getModel("mymodel");
  StructType* structType = (StructType*) model->getLLVMType(llvmContext)->getPointerElementType();
  
  ASSERT_NE(structType, nullptr);
  EXPECT_TRUE(structType->getNumElements() == 2);
  EXPECT_EQ(structType->getElementType(0), Type::getInt64Ty(llvmContext));
  EXPECT_EQ(structType->getElementType(1), Type::getFloatTy(llvmContext));
  EXPECT_STREQ(model->getName().c_str(), "mymodel");
}

TEST_F(TestFileSampleRunner, ModelDefinitionTest) {
  runFile("tests/samples/test_model_definition.yz", "0");
}

TEST_F(TestFileSampleRunner, ModelDefinitionWithMethodTest) {
  runFile("tests/samples/test_model_method.yz", "0");
}
