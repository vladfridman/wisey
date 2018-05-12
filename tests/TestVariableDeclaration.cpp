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
#include "TestFileRunner.hpp"
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
  mIdentifier(new Identifier("foo", 0)) {
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
  const PrimitiveTypeSpecifier* typeSpecifier = PrimitiveTypes::INT->newTypeSpecifier(0);
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
  const PrimitiveTypeSpecifier* typeSpecifier = PrimitiveTypes::INT->newTypeSpecifier(0);
  NiceMock<MockExpression>* expression = new NiceMock<MockExpression>();
  Value * value = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 5);
  ON_CALL(*expression, generateIR(_, _)).WillByDefault(Return(value));
  ON_CALL(*expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
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
  ModelTypeSpecifier* typeSpecifier = new ModelTypeSpecifier(NULL, "MModel", 0);
  
  string modelFullName = "systems.vos.wisey.compiler.tests.MModel";
  StructType* structType = StructType::create(mLLVMContext, modelFullName);
  vector<Type*> types;
  types.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                  ->getPointerTo()->getPointerTo());
  types.push_back(Type::getInt32Ty(mLLVMContext));
  types.push_back(Type::getInt32Ty(mLLVMContext));
  structType->setBody(types);
  vector<IField*> fields;
  fields.push_back(new FixedField(PrimitiveTypes::INT, "fieldA", 0));
  fields.push_back(new FixedField(PrimitiveTypes::INT, "fieldB", 0));
  Model* model = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                                 modelFullName,
                                 structType,
                                 mContext.getImportProfile(),
                                 0);
  model->setFields(mContext, fields, 1u);

  mContext.addModel(model, 0);
  VariableDeclaration* declaration = VariableDeclaration::create(typeSpecifier, mIdentifier, 0);
  
  declaration->generateIR(mContext);
  
  EXPECT_NE(mContext.getScopes().getVariable("foo"), nullptr);

  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %foo = alloca %systems.vos.wisey.compiler.tests.MModel*"
  "\n  store %systems.vos.wisey.compiler.tests.MModel* null, %systems.vos.wisey.compiler.tests.MModel** %foo\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
  delete declaration;
}

TEST_F(TestFileRunner, variableDeclarationRunTest) {
  runFile("tests/samples/test_variable_declaration.yz", "5");
}

TEST_F(TestFileRunner, variableDeclarationAssignToZeroRunTest) {
  runFile("tests/samples/test_variable_declaration_assign_to_zero.yz", "0");
}

TEST_F(TestFileRunner, uninitializedIntRunTest) {
  runFile("tests/samples/test_uninitialized_int.yz", "2");
}

TEST_F(TestFileRunner, stringRunTest) {
  runFileCheckOutput("tests/samples/test_string.yz", "hello\n", "");
}

TEST_F(TestFileRunner, stringUninitialzedRunTest) {
  runFileCheckOutput("tests/samples/test_string_uninitialized.yz", "1\n", "");
}
