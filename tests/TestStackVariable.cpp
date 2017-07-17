//
//  TestStackVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link StackVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/StackVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct StackVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  StackVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    mBlock = BasicBlock::Create(mContext.getLLVMContext(), "entry");
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~StackVariableTest() {
    delete mBlock;
    delete mStringStream;
  }
};

TEST_F(StackVariableTest, generateAssignmentIRTest) {
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, Type::getInt32Ty(mLLVMContext), "foo");

  StackVariable stackVariable("foo", PrimitiveTypes::INT_TYPE, alloc);
  Value* assignValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
  NiceMock<MockExpression> expression;
  ON_CALL(expression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  ON_CALL(expression, generateIR(_)).WillByDefault(Return(assignValue));
  
  stackVariable.generateAssignmentIR(mContext, &expression);
  
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

TEST_F(StackVariableTest, generateAssignmentIRWithCastTest) {
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, Type::getInt32Ty(mLLVMContext), "foo");
  StackVariable stackVariable("foo", PrimitiveTypes::INT_TYPE, alloc);
  Value* assignValue = ConstantInt::get(Type::getInt1Ty(mLLVMContext), 1);
  NiceMock<MockExpression> expression;
  ON_CALL(expression, getType(_)).WillByDefault(Return(PrimitiveTypes::BOOLEAN_TYPE));
  ON_CALL(expression, generateIR(_)).WillByDefault(Return(assignValue));
  
  stackVariable.generateAssignmentIR(mContext, &expression);
  
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

TEST_F(StackVariableTest, generateIdentifierIRTest) {
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, Type::getInt32Ty(mLLVMContext), "foo");
  StackVariable stackVariable("foo", PrimitiveTypes::INT_TYPE, alloc);
  
  stackVariable.generateIdentifierIR(mContext, "bar");

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

TEST_F(StackVariableTest, freeTest) {
  StackVariable stackVariable("foo", PrimitiveTypes::INT_TYPE, NULL);

  stackVariable.free(mContext);
  
  ASSERT_EQ(mBlock->size(), 0u);
}

TEST_F(StackVariableTest, existsInOuterScopeTest) {
  StackVariable stackVariable("foo", PrimitiveTypes::INT_TYPE, NULL);
  
  EXPECT_FALSE(stackVariable.existsInOuterScope());
}

TEST_F(StackVariableTest, setToNullDeathTest) {
  StackVariable stackVariable("foo", PrimitiveTypes::INT_TYPE, NULL);
  
  EXPECT_EXIT(stackVariable.setToNull(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Stack Variables should not be set to null");
}

TEST_F(TestFileSampleRunner, assignmentWithAutocastRunTest) {
  runFile("tests/samples/test_assignment_with_autocast.yz", "1");
}
