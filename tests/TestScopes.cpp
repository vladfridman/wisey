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
  
  StackVariable* fooVariable =
  new StackVariable("foo", PrimitiveTypes::INT_TYPE, fooValue);
  StackVariable* barVariable =
  new StackVariable("bar", PrimitiveTypes::INT_TYPE, barValue);
  mScopes.setVariable(fooVariable);
  mScopes.pushScope();
  mScopes.setVariable(barVariable);
  
  EXPECT_EQ(mScopes.getVariable("bar")->getValue(), barValue);
  EXPECT_EQ(mScopes.getVariable("foo")->getValue(), fooValue);
  
  mScopes.popScope(mContext);
  EXPECT_EQ(mScopes.getVariable("foo")->getValue(), fooValue);
  EXPECT_EQ(mScopes.getVariable("bar"), nullptr);
  
  barVariable = new StackVariable("bar", PrimitiveTypes::INT_TYPE, barValue);
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
  
  StackVariable* outerVariable =
  new StackVariable("foo", PrimitiveTypes::INT_TYPE, outerValue);
  StackVariable* innerVariable =
  new StackVariable("foo", PrimitiveTypes::INT_TYPE, innerValue);
  
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
  mScopes.pushScope();
  Value* fooValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  StackVariable* fooVariable = new StackVariable("foo", PrimitiveTypes::INT_TYPE, fooValue);
  mScopes.setVariable(fooVariable);
  
  EXPECT_EQ(mScopes.getVariable("foo"), fooVariable);
  
  mScopes.clearVariable("foo");
  
  EXPECT_EQ(mScopes.getVariable("foo"), nullptr);
  EXPECT_EQ(mScopes.getVariableForAssignement("foo"), fooVariable);
}

TEST_F(ScopesTest, getClearedVariablesTest) {
  mScopes.pushScope();
  Value* value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  StackVariable* fooVariable = new StackVariable("foo", PrimitiveTypes::INT_TYPE, value);
  StackVariable* barVariable = new StackVariable("bar", PrimitiveTypes::INT_TYPE, value);
  mScopes.setVariable(fooVariable);
  mScopes.setVariable(barVariable);

  mScopes.clearVariable("foo");
  
  map<string, IVariable*> clearedVariables = mScopes.getClearedVariables();
  EXPECT_EQ(clearedVariables.size(), 1u);
  EXPECT_EQ(clearedVariables.count("foo"), 1u);
  EXPECT_EQ(clearedVariables.at("foo"), fooVariable);

  mScopes.clearVariable("bar");
  clearedVariables = mScopes.getClearedVariables();
  EXPECT_EQ(clearedVariables.size(), 2u);
  EXPECT_EQ(clearedVariables.count("foo"), 1u);
  EXPECT_EQ(clearedVariables.count("bar"), 1u);
  EXPECT_EQ(clearedVariables.at("foo"), fooVariable);
  EXPECT_EQ(clearedVariables.at("bar"), barVariable);
}

TEST_F(ScopesTest, setClearedVariablesTest) {
  mScopes.pushScope();
  Value* value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  StackVariable* fooVariable = new StackVariable("foo", PrimitiveTypes::INT_TYPE, value);
  StackVariable* barVariable = new StackVariable("bar", PrimitiveTypes::INT_TYPE, value);
  mScopes.setVariable(fooVariable);
  mScopes.setVariable(barVariable);
  
  map<string, IVariable*> clearedVariables;
  clearedVariables["foo"] = fooVariable;
  clearedVariables["bar"] = barVariable;
  mScopes.setClearedVariables(clearedVariables);
  clearedVariables.clear();
  
  clearedVariables = mScopes.getClearedVariables();
  EXPECT_EQ(clearedVariables.size(), 2u);
  EXPECT_EQ(clearedVariables.count("foo"), 1u);
  EXPECT_EQ(clearedVariables.count("bar"), 1u);
  EXPECT_EQ(clearedVariables.at("foo"), fooVariable);
  EXPECT_EQ(clearedVariables.at("bar"), barVariable);
}

TEST_F(ScopesTest, eraseFromClearedVariablesTest) {
  mScopes.pushScope();
  Value* value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  StackVariable* fooVariable = new StackVariable("foo", PrimitiveTypes::INT_TYPE, value);
  StackVariable* barVariable = new StackVariable("bar", PrimitiveTypes::INT_TYPE, value);
  mScopes.setVariable(fooVariable);
  mScopes.setVariable(barVariable);
  
  mScopes.clearVariable("foo");
  
  map<string, IVariable*> clearedVariables = mScopes.getClearedVariables();
  EXPECT_EQ(clearedVariables.size(), 1u);
  EXPECT_EQ(clearedVariables.count("foo"), 1u);
  EXPECT_EQ(clearedVariables.at("foo"), fooVariable);
  
  mScopes.eraseFromClearedVariables(fooVariable);
  EXPECT_EQ(mScopes.getClearedVariables().size(), 0u);
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

