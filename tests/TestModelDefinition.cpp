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
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/ModelDefinition.hpp"
#include "yazyk/PrimitiveTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

TEST(ModelDefinitionTest, TestSimpleDefinition) {
  IRGenerationContext context;
  LLVMContext& llvmContext = context.getLLVMContext();
  PrimitiveTypeSpecifier longType(PRIMITIVE_TYPE_LONG);
  PrimitiveTypeSpecifier floatType(PRIMITIVE_TYPE_FLOAT);
  ModelFieldDeclaration field1(longType, "field1");
  ModelFieldDeclaration field2(floatType, "field2");
  vector<ModelFieldDeclaration *> fields;
  fields.push_back(&field1);
  fields.push_back(&field2);
  ModelDefinition modelDefinition("mymodel", fields);
  
  modelDefinition.generateIR(context);
  Model* model = context.getModel("model.mymodel");
  StructType* structType = model->getStructType();
  
  ASSERT_TRUE(structType != NULL);
  EXPECT_TRUE(structType->getNumElements() == 2);
  EXPECT_EQ(structType->getElementType(0), Type::getInt64Ty(llvmContext));
  EXPECT_EQ(structType->getElementType(1), Type::getFloatTy(llvmContext));
}

TEST_F(TestFileSampleRunner, ModelDefinitionTest) {
  runFile("tests/samples/test_model_definition.yz", "0");
}
