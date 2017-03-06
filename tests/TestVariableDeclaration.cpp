//
//  TestVariableDeclaration.cpp
//  Yazyk
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

#include "TestFileSampleRunner.hpp"
#include "yazyk/Identifier.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/ModelTypeSpecifier.hpp"
#include "yazyk/PrimitiveTypes.hpp"
#include "yazyk/PrimitiveTypeSpecifier.hpp"
#include "yazyk/VariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

class MockExpression : public IExpression {
public:
  MOCK_CONST_METHOD1(generateIR, Value* (IRGenerationContext&));
  MOCK_CONST_METHOD1(getType, IType* (IRGenerationContext&));
};

struct VariableDeclarationTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext = mContext.getLLVMContext();
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Function* mFunction;
  
  VariableDeclarationTest() {
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

TEST_F(VariableDeclarationTest, StackVariableDeclarationWithoutAssignmentTest) {
  Identifier identifier("foo", "bar");
  PrimitiveTypeSpecifier typeSpecifier(PrimitiveTypes::INT_TYPE);
  VariableDeclaration declaration(typeSpecifier, identifier);

  declaration.generateIR(mContext);
  
  EXPECT_NE(mContext.getScopes().getVariable("foo"), nullptr);
  ASSERT_EQ(1ul, mBlock->size());
  *mStringStream << mBlock->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  %foo = alloca i32");
}

TEST_F(VariableDeclarationTest, StackVariableDeclarationWithAssignmentTest) {
  Identifier identifier("foo", "bar");
  PrimitiveTypeSpecifier typeSpecifier(PrimitiveTypes::INT_TYPE);
  NiceMock<MockExpression> mExpression;
  Value * value = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 5);
  ON_CALL(mExpression, generateIR(_)).WillByDefault(Return(value));
  ON_CALL(mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  VariableDeclaration declaration(typeSpecifier, identifier, &mExpression);
  
  declaration.generateIR(mContext);
  
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
}

TEST_F(VariableDeclarationTest, HeapVariableDeclarationWithoutAssignmentTest) {
  Identifier identifier("foo", "bar");
  ModelTypeSpecifier typeSpecifier("model");
  
  StructType* structType = StructType::create(mLLVMContext, "test");
  vector<Type*> types;
  types.push_back(Type::getInt32Ty(mLLVMContext));
  types.push_back(Type::getInt32Ty(mLLVMContext));
  structType->setBody(types);
  map<string, ModelField*>* fields = new map<string, ModelField*>();
  ModelField* fieldA = new ModelField(PrimitiveTypes::INT_TYPE, 0);
  ModelField* fieldB = new ModelField(PrimitiveTypes::INT_TYPE, 0);
  (*fields)["fieldA"] = fieldA;
  (*fields)["fieldB"] = fieldB;
  map<string, Method*>* methods = new map<string, Method*>();
  vector<Interface*> interfaces;
  Model* model = new Model("model", structType, fields, methods, interfaces);

  mContext.addModel(model);
  VariableDeclaration declaration(typeSpecifier, identifier);
  
  declaration.generateIR(mContext);
  
  EXPECT_NE(mContext.getScopes().getVariable("foo"), nullptr);
  ASSERT_EQ(0ul, mBlock->size());
}

TEST_F(TestFileSampleRunner, VariableDeclarationTest) {
  runFile("tests/samples/test_variable_declaration.yz", "5");
}

TEST_F(TestFileSampleRunner, VariableDeclarationAssignToZeroTest) {
  runFile("tests/samples/test_variable_declaration_assign_to_zero.yz", "0");
}
