//
//  TestVariableDeclaration.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/18/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link VariableDeclaration}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/ControllerTypeSpecifier.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/VariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct VariableDeclarationTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Function* mFunction;
  Identifier* mIdentifier;
  
  VariableDeclarationTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mIdentifier(new Identifier("foo")) {
    mContext.setPackage("systems.vos.wisey.compiler.tests");
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "test",
                                 mContext.getModule());
    mBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~VariableDeclarationTest() {
    delete mStringStream;
  }
};

TEST_F(VariableDeclarationTest, stackVariableDeclarationWithoutAssignmentTest) {
  PrimitiveTypeSpecifier* typeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
  VariableDeclaration* declaration = VariableDeclaration::create(typeSpecifier, mIdentifier, 0);

  declaration->generateIR(mContext);
  
  EXPECT_NE(mContext.getScopes().getVariable("foo"), nullptr);
  ASSERT_EQ(2ul, mBlock->size());
  *mStringStream << *mBlock;
  string expected =
  "\nentry:"
  "\n  %foo = alloca i32"
  "\n  store i32 0, i32* %foo\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
  delete declaration;
}

TEST_F(VariableDeclarationTest, stackVariableDeclarationWithAssignmentTest) {
  PrimitiveTypeSpecifier* typeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
  NiceMock<MockExpression>* expression = new NiceMock<MockExpression>();
  Value * value = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 5);
  ON_CALL(*expression, generateIR(_, _)).WillByDefault(Return(value));
  ON_CALL(*expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  VariableDeclaration* declaration =
  VariableDeclaration::createWithAssignment(typeSpecifier, mIdentifier, expression, 0);
  
  declaration->generateIR(mContext);
  
  EXPECT_NE(mContext.getScopes().getVariable("foo"), nullptr);
  ASSERT_EQ(2ul, mBlock->size());

  BasicBlock::iterator iterator = mBlock->begin();
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %foo = alloca i32");
  iterator++;
  mStringBuffer.clear();
  
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  store i32 5, i32* %foo");
  mStringBuffer.clear();
  delete declaration;
}

TEST_F(VariableDeclarationTest, modelVariableDeclarationWithoutAssignmentTest) {
  vector<string> package;
  ModelTypeSpecifier* typeSpecifier = new ModelTypeSpecifier(package, "MModel");
  
  string modelFullName = "systems.vos.wisey.compiler.tests.MModel";
  StructType* structType = StructType::create(mLLVMContext, modelFullName);
  vector<Type*> types;
  types.push_back(Type::getInt64Ty(mLLVMContext));
  types.push_back(Type::getInt32Ty(mLLVMContext));
  types.push_back(Type::getInt32Ty(mLLVMContext));
  structType->setBody(types);
  vector<Field*> fields;
  ExpressionList arguments;
  fields.push_back(new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "fieldA", arguments));
  fields.push_back(new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "fieldB", arguments));
  Model* model = Model::newModel(modelFullName, structType);
  model->setFields(fields, 1u);

  mContext.addModel(model);
  VariableDeclaration* declaration = VariableDeclaration::create(typeSpecifier, mIdentifier, 0);
  
  declaration->generateIR(mContext);
  
  EXPECT_NE(mContext.getScopes().getVariable("foo"), nullptr);

  *mStringStream << *mBlock;
  
  string expected =
  "\nentry:"
  "\n  %referenceDeclaration = alloca %systems.vos.wisey.compiler.tests.MModel*"
  "\n  store %systems.vos.wisey.compiler.tests.MModel* null, "
  "%systems.vos.wisey.compiler.tests.MModel** %referenceDeclaration\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
  delete declaration;
}

TEST_F(TestFileSampleRunner, variableDeclarationRunTest) {
  runFile("tests/samples/test_variable_declaration.yz", "5");
}

TEST_F(TestFileSampleRunner, variableDeclarationAssignToZeroRunTest) {
  runFile("tests/samples/test_variable_declaration_assign_to_zero.yz", "0");
}

TEST_F(TestFileSampleRunner, uninitializedIntRunTest) {
  runFile("tests/samples/test_uninitialized_int.yz", "2");
}

TEST_F(TestFileSampleRunner, stringRunTest) {
  runFileCheckOutput("tests/samples/test_string.yz", "hello\n", "");
}

TEST_F(TestFileSampleRunner, stringUninitialzedRunTest) {
  runFileCheckOutput("tests/samples/test_string_uninitialized.yz", "1\n", "");
}
