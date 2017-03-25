//
//  TestLocalStackVariable.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LocalStackVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileSampleRunner.hpp"
#include "yazyk/IExpression.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/LocalStackVariable.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LocalStackVariableTest : public Test {
  IRGenerationContext mContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  LocalStackVariableTest() {
    mBlock = BasicBlock::Create(mContext.getLLVMContext(), "entry");
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~LocalStackVariableTest() {
    delete mBlock;
    delete mStringStream;
  }
};

TEST_F(LocalStackVariableTest, GenerateAssignmentIRTest) {
  AllocaInst* alloc = new AllocaInst(Type::getInt32Ty(mContext.getLLVMContext()),
                                     "foo",
                                     mBlock);
  LocalStackVariable localStackVariable("foo", PrimitiveTypes::INT_TYPE, alloc);
  Value* assignValue = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 5);
  NiceMock<MockExpression> expression;
  ON_CALL(expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  ON_CALL(expression, generateIR(_)).WillByDefault(Return(assignValue));
  
  localStackVariable.generateAssignmentIR(mContext, expression);
  
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

TEST_F(LocalStackVariableTest, GenerateAssignmentIRWithCastTest) {
  AllocaInst* alloc = new AllocaInst(Type::getInt32Ty(mContext.getLLVMContext()),
                                     "foo",
                                     mBlock);
  LocalStackVariable localStackVariable("foo", PrimitiveTypes::INT_TYPE, alloc);
  Value* assignValue = ConstantInt::get(Type::getInt1Ty(mContext.getLLVMContext()), 1);
  NiceMock<MockExpression> expression;
  ON_CALL(expression, getType(_)).WillByDefault(Return(PrimitiveTypes::BOOLEAN_TYPE));
  ON_CALL(expression, generateIR(_)).WillByDefault(Return(assignValue));
  
  localStackVariable.generateAssignmentIR(mContext, expression);
  
  ASSERT_EQ(3ul, mBlock->size());
  BasicBlock::iterator iterator = mBlock->begin();
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %foo = alloca i32");
  mStringBuffer.clear();
  
  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %conv = zext i1 true to i32");
  mStringBuffer.clear();
  
  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  store i32 %conv, i32* %foo");
  mStringBuffer.clear();
}

TEST_F(LocalStackVariableTest, GenerateIdentifierIRTest) {
  AllocaInst* alloc = new AllocaInst(Type::getInt32Ty(mContext.getLLVMContext()),
                                     "foo",
                                     mBlock);
  LocalStackVariable localStackVariable("foo", PrimitiveTypes::INT_TYPE, alloc);
  
  localStackVariable.generateIdentifierIR(mContext, "bar");

  ASSERT_EQ(2ul, mBlock->size());
  
  BasicBlock::iterator iterator = mBlock->begin();
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %foo = alloca i32");
  mStringBuffer.clear();
  
  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %bar = load i32, i32* %foo");
  mStringBuffer.clear();
}

TEST_F(LocalStackVariableTest, TestFree) {
  LocalStackVariable localStackVariable("foo", PrimitiveTypes::INT_TYPE, NULL);

  localStackVariable.free(mContext);
  
  ASSERT_EQ(mBlock->size(), 0u);
}

TEST_F(TestFileSampleRunner, AssignmentWithAutocastRunTest) {
  runFile("tests/samples/test_assignment_with_autocast.yz", "1");
}
