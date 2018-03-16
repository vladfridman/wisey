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
#include "wisey/FixedField.hpp"
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
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Function* mFunction;
  Identifier* mIdentifier;
  string mPackage = "systems.vos.wisey.compiler.tests";
  ImportProfile* mImportProfile;

  VariableDeclarationTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mIdentifier(new Identifier("foo")) {
    mImportProfile = new ImportProfile(mPackage);
    mContext.setImportProfile(mImportProfile);

    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "test",
                                 mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~VariableDeclarationTest() {
    delete mStringStream;
  }
};

TEST_F(VariableDeclarationTest, stackVariableDeclarationWithoutAssignmentTest) {
  PrimitiveTypeSpecifier* typeSpecifier = PrimitiveTypes::INT_TYPE->newTypeSpecifier();
  VariableDeclaration* declaration = VariableDeclaration::create(typeSpecifier, mIdentifier, 0);

  declaration->generateIR(mContext);
  
  EXPECT_NE(mContext.getScopes().getVariable("foo"), nullptr);
  ASSERT_EQ(2ul, mBasicBlock->size());
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = alloca i32"
  "\n  store i32 0, i32* %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
  delete declaration;
}

TEST_F(VariableDeclarationTest, stackVariableDeclarationWithAssignmentTest) {
  PrimitiveTypeSpecifier* typeSpecifier = PrimitiveTypes::INT_TYPE->newTypeSpecifier();
  NiceMock<MockExpression>* expression = new NiceMock<MockExpression>();
  Value * value = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 5);
  ON_CALL(*expression, generateIR(_, _)).WillByDefault(Return(value));
  ON_CALL(*expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  VariableDeclaration* declaration =
  VariableDeclaration::createWithAssignment(typeSpecifier, mIdentifier, expression, 0);
  
  declaration->generateIR(mContext);
  
  EXPECT_NE(mContext.getScopes().getVariable("foo"), nullptr);

  *mStringStream << *mBasicBlock;

  string expected =
  "\nentry:"
  "\n  %0 = alloca i32"
  "\n  store i32 0, i32* %0"
  "\n  store i32 5, i32* %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
  delete declaration;
}

TEST_F(VariableDeclarationTest, modelVariableDeclarationWithoutAssignmentTest) {
  ModelTypeSpecifier* typeSpecifier = new ModelTypeSpecifier(NULL, "MModel");
  
  string modelFullName = "systems.vos.wisey.compiler.tests.MModel";
  StructType* structType = StructType::create(mLLVMContext, modelFullName);
  vector<Type*> types;
  types.push_back(Type::getInt64Ty(mLLVMContext));
  types.push_back(Type::getInt32Ty(mLLVMContext));
  types.push_back(Type::getInt32Ty(mLLVMContext));
  structType->setBody(types);
  vector<IField*> fields;
  fields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "fieldA"));
  fields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "fieldB"));
  Model* model = Model::newModel(AccessLevel::PUBLIC_ACCESS, modelFullName, structType);
  model->setFields(fields, 1u);

  mContext.addModel(model);
  VariableDeclaration* declaration = VariableDeclaration::create(typeSpecifier, mIdentifier, 0);
  
  declaration->generateIR(mContext);
  
  EXPECT_NE(mContext.getScopes().getVariable("foo"), nullptr);

  *mStringStream << *mBasicBlock;
  
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
