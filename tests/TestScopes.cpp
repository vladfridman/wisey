//
//  TestScopes.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Scopes}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockStatement.hpp"
#include "MockType.hpp"
#include "MockVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/HeapVariable.hpp"
#include "wisey/LocalStackVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ScopesTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Scopes mScopes;
  
  ScopesTest() : mLLVMContext(mContext.getLLVMContext()) {

  }
};

TEST_F(ScopesTest, scopesTest) {
  mScopes.pushScope();
  Value* fooValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  Value* barValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
  
  LocalStackVariable* fooVariable =
  new LocalStackVariable("foo", PrimitiveTypes::INT_TYPE, fooValue);
  LocalStackVariable* barVariable =
  new LocalStackVariable("bar", PrimitiveTypes::INT_TYPE, barValue);
  mScopes.setVariable(fooVariable);
  mScopes.pushScope();
  mScopes.setVariable(barVariable);
  
  EXPECT_EQ(mScopes.getVariable("bar")->getValue(), barValue);
  EXPECT_EQ(mScopes.getVariable("foo")->getValue(), fooValue);
  
  mScopes.popScope(mContext);
  EXPECT_EQ(mScopes.getVariable("foo")->getValue(), fooValue);
  EXPECT_EQ(mScopes.getVariable("bar"), nullptr);
  
  barVariable = new LocalStackVariable("bar", PrimitiveTypes::INT_TYPE, barValue);
  mScopes.setVariable(barVariable);
  EXPECT_EQ(mScopes.getVariable("foo")->getValue(), fooValue);
  EXPECT_EQ(mScopes.getVariable("bar")->getValue(), barValue);
  
  mScopes.popScope(mContext);
  EXPECT_EQ(mScopes.getVariable("foo"), nullptr);
  EXPECT_EQ(mScopes.getVariable("bar"), nullptr);
}

TEST_F(ScopesTest, scopesCorrectlyOrderedTest) {
  mScopes.pushScope();
  Value* outerValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  Value* innerValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
  
  LocalStackVariable* outerVariable =
  new LocalStackVariable("foo", PrimitiveTypes::INT_TYPE, outerValue);
  LocalStackVariable* innerVariable =
  new LocalStackVariable("foo", PrimitiveTypes::INT_TYPE, innerValue);
  
  mScopes.setVariable(outerVariable);
  mScopes.pushScope();
  mScopes.setVariable(innerVariable);
  
  EXPECT_EQ(mScopes.getVariable("foo")->getValue(), innerValue);
  
  mScopes.popScope(mContext);
  
  EXPECT_EQ(mScopes.getVariable("foo")->getValue(), outerValue);
}

TEST_F(ScopesTest, returnTypeTest) {
  mScopes.pushScope();
  mScopes.setReturnType(PrimitiveTypes::DOUBLE_TYPE);
  mScopes.pushScope();
  mScopes.pushScope();
  
  const IType* returnType = mScopes.getReturnType();
  EXPECT_EQ(returnType, PrimitiveTypes::DOUBLE_TYPE);
}

TEST_F(ScopesTest, getScopeDeathTest) {
  EXPECT_EXIT(mScopes.getScope(),
              ::testing::ExitedWithCode(1),
              "Error: Can not get scope. Scope list is empty.");
}

TEST_F(ScopesTest, getScopeTest) {
  mScopes.pushScope();
  Scope* scope = mScopes.getScope();
  
  EXPECT_NE(scope, nullptr);
}

TEST_F(ScopesTest, clearVariableTest) {
  Value* fooValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  
  mScopes.pushScope();
  LocalStackVariable* fooVariable =
  new LocalStackVariable("foo", PrimitiveTypes::INT_TYPE, fooValue);
  mScopes.setVariable(fooVariable);
  
  EXPECT_EQ(mScopes.getVariable("foo")->getValue(), fooValue);
  
  mScopes.clearVariable("foo");
  
  EXPECT_EQ(mScopes.getVariable("foo"), nullptr);
}

TEST_F(ScopesTest, clearVariableDeathTest) {
  EXPECT_EXIT(mScopes.clearVariable("foo"),
              ::testing::ExitedWithCode(1),
              "Error: Could not clear variable 'foo': the Scopes stack is empty");
  
  mScopes.pushScope();
  
  EXPECT_EXIT(mScopes.clearVariable("foo"),
              ::testing::ExitedWithCode(1),
              "Error: Could not clear variable 'foo': it was not found");
}

TEST_F(ScopesTest, setHeapVariableTest) {
  mScopes.pushScope();
  Value* fooValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  HeapVariable* heapVariable =
    new HeapVariable("foo", PrimitiveTypes::INT_TYPE, fooValue);
  mScopes.setVariable(heapVariable);
  
  ASSERT_NE(mScopes.getVariable("foo"), nullptr);
  EXPECT_EQ(mScopes.getVariable("foo")->getValue(), fooValue);
}

TEST_F(ScopesTest, setUnitializedHeapVariableTest) {
  mScopes.pushScope();
  
  HeapVariable* unitializedHeapVariable =
    new HeapVariable("foo", PrimitiveTypes::INT_TYPE, NULL);
  mScopes.setVariable(unitializedHeapVariable);
  
  ASSERT_NE(mScopes.getVariable("foo"), nullptr);
  EXPECT_EQ(mScopes.getVariable("foo")->getValue(), nullptr);
}

TEST_F(ScopesTest, setLandingPadBlockTest) {
  mScopes.pushScope();
  BasicBlock* basicBlock = BasicBlock::Create(mLLVMContext);
  
  mScopes.setLandingPadBlock(basicBlock);
  mScopes.pushScope();
  
  ASSERT_EQ(mScopes.getLandingPadBlock(), basicBlock);
}

TEST_F(ScopesTest, setExceptionContinueBlockTest) {
  mScopes.pushScope();
  BasicBlock* basicBlock = BasicBlock::Create(mLLVMContext);
  
  mScopes.setExceptionContinueBlock(basicBlock);
  mScopes.pushScope();
  
  ASSERT_EQ(mScopes.getExceptionContinueBlock(), basicBlock);
}

TEST_F(ScopesTest, setExceptionFinallyTest) {
  mScopes.pushScope();
  NiceMock<MockStatement>* mockStatement = new NiceMock<MockStatement>();
  
  mScopes.setExceptionFinally(mockStatement);
  mScopes.pushScope();
  
  ASSERT_EQ(mScopes.getExceptionFinally(), mockStatement);
}

TEST_F(ScopesTest, reportUnhandledExceptionsDeathTest) {
  mScopes.pushScope();
  
  NiceMock<MockType> mockExceptionType;
  Mock::AllowLeak(&mockExceptionType);
  ON_CALL(mockExceptionType, getName()).WillByDefault(Return("MExceptions"));
  mScopes.getScope()->addException(&mockExceptionType);
  
  EXPECT_EXIT(mScopes.popScope(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Exception MExceptions is not handled");
}

TEST_F(ScopesTest, freeOwnedMemoryTest) {
  NiceMock<MockVariable> foo;
  NiceMock<MockVariable> bar;

  ON_CALL(foo, getName()).WillByDefault(Return("foo"));
  ON_CALL(bar, getName()).WillByDefault(Return("bar"));
  
  mScopes.pushScope();
  mScopes.setVariable(&foo);
  mScopes.pushScope();
  mScopes.setVariable(&bar);
  
  EXPECT_CALL(foo, free(_));
  EXPECT_CALL(bar, free(_));
  
  mScopes.freeOwnedMemory(mContext);
}

