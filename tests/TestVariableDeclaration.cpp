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
};

struct VariableDeclarationTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext = mContext.getLLVMContext();
  BasicBlock* mBlock = BasicBlock::Create(mLLVMContext, "entry");
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  VariableDeclarationTest() {
    mContext.setBasicBlock(mBlock);
    mContext.pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~VariableDeclarationTest() {
    delete mBlock;
    delete mStringStream;
  }
};

TEST_F(VariableDeclarationTest, VariableDeclarationWithoutAssignmentTest) {
  Identifier identifier("foo", "bar");
  TypeSpecifier typeSpecifier(PRIMITIVE_TYPE_INT);
  VariableDeclaration declaration(typeSpecifier, identifier);

  declaration.generateIR(mContext);
  
  EXPECT_EQ(mContext.getVariable("foo") != NULL, true);
  ASSERT_EQ(1ul, mBlock->size());
  *mStringStream << mBlock->front();
  EXPECT_STREQ(mStringStream->str().c_str(), "  %foo = alloca i32");
}

TEST_F(VariableDeclarationTest, VariableDeclarationWithAssignmentTest) {
  Identifier identifier("foo", "bar");
  TypeSpecifier typeSpecifier(PRIMITIVE_TYPE_INT);
  NiceMock<MockExpression> mExpression;
  Value * value = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 5);
  ON_CALL(mExpression, generateIR(_)).WillByDefault(Return(value));
  VariableDeclaration declaration(typeSpecifier, identifier, &mExpression);
  
  declaration.generateIR(mContext);
  
  EXPECT_EQ(mContext.getVariable("foo") != NULL, true);
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

TEST_F(TestFileSampleRunner, VariableDeclarationTest) {
  runFile("tests/samples/test_variable_declaration.yz", "5");
}

TEST_F(TestFileSampleRunner, VariableDeclarationAssignToZeroTest) {
  runFile("tests/samples/test_variable_declaration_assign_to_zero.yz", "0");
}
