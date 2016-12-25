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

#include "yazyk/Assignment.hpp"
#include "yazyk/Identifier.hpp"
#include "yazyk/IRGenerationContext.hpp"

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
    mContext.pushBlock(mBlock);
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
  NiceMock<MockExpression> expression;
  Identifier identifier("foo", "bar");
  Assignment assignment(identifier, mExpression);
  Mock::AllowLeak(&mExpression);

  EXPECT_EXIT(assignment.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "undeclared variable foo");
}

TEST_F(AssignmentTest, SimpleTest) {
  string name = "foo";
  NiceMock<MockExpression> expression;
  Identifier identifier(name, "bar");
  Assignment assignment(identifier, mExpression);
  AllocaInst* alloc = new AllocaInst(Type::getInt32Ty(mContext.getLLVMContext()),
                                     name,
                                     mBlock);
  mContext.locals()[name] = alloc;

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
