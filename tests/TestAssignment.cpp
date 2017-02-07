//
//  TestAssignment.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/20/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Assignment}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/Assignment.hpp"
#include "yazyk/Identifier.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

class MockExpression : public IExpression {
public:
  MOCK_CONST_METHOD1(generateIR, Value* (IRGenerationContext&));
};

struct AssignmentTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockExpression> mExpression;
  BasicBlock* mBlock = BasicBlock::Create(mContext.getLLVMContext(), "entry");
  string mStringBuffer;
  raw_string_ostream* mStringStream;

public:
  
  AssignmentTest() {
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
    Value* value = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 5);
    ON_CALL(mExpression, generateIR(_)).WillByDefault(Return(value));
  }
  
  ~AssignmentTest() {
    delete mBlock;
    delete mStringStream;
  }
};

TEST_F(AssignmentTest, VariableNotDeclaredDeathTest) {
  Identifier identifier("foo", "bar");
  Assignment assignment(identifier, mExpression);
  Mock::AllowLeak(&mExpression);

  EXPECT_EXIT(assignment.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "undeclared variable foo");
}

TEST_F(AssignmentTest, SimpleTest) {
  string name = "foo";
  Identifier identifier(name, "bar");
  Assignment assignment(identifier, mExpression);
  AllocaInst* alloc = new AllocaInst(Type::getInt32Ty(mContext.getLLVMContext()),
                                     name,
                                     mBlock);
  mContext.getScopes().setStackVariable(name, PrimitiveTypes::INT_TYPE, alloc);

  assignment.generateIR(mContext);

  ASSERT_EQ(2ul, mBlock->size());
  BasicBlock::iterator iterator = mBlock->begin();
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %foo = alloca i32");
  mStringBuffer.clear();
  
  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  store i32 5, i32* %foo");
  mStringBuffer.clear();
}

TEST_F(AssignmentTest, HeapVariableTest) {
  Scopes& scopes = mContext.getScopes();
  NiceMock<MockExpression> expression;
  Value* fooValue = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 3);
  BitCastInst* bitCastInst = new BitCastInst(fooValue,
                                             fooValue->getType(),
                                             "foo",
                                             mContext.getBasicBlock());
  ON_CALL(expression, generateIR(_)).WillByDefault(Return(bitCastInst));
  scopes.setHeapVariable("foo", PrimitiveTypes::INT_TYPE, bitCastInst);
  scopes.setUnitializedHeapVariable("bar", PrimitiveTypes::INT_TYPE);
  Identifier identifier("bar", "foo");
  Assignment assignment(identifier, expression);
  
  EXPECT_NE(scopes.getVariable("foo"), nullptr);
  EXPECT_NE(scopes.getVariable("bar"), nullptr);
  
  assignment.generateIR(mContext);
  
  EXPECT_EQ(scopes.getVariable("foo"), nullptr);
  ASSERT_NE(scopes.getVariable("bar"), nullptr);
  EXPECT_EQ(BitCastInst::classof(scopes.getVariable("bar")->getValue()), true);
}

TEST_F(TestFileSampleRunner, ModelVariableAssignmentTest) {
  runFile("tests/samples/test_assignment_model_variable.yz", "0");
}
