//
//  TestScopes.cpp
//  Yazyk
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

#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/LocalHeapVariable.hpp"
#include "yazyk/LocalStackVariable.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

TEST(ScopesTest, TestScopes) {
  Scopes scopes;
  LLVMContext llvmContext;
  
  scopes.pushScope();
  Value* fooValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 3);
  Value* barValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 5);
  
  LocalStackVariable* fooVariable =
    new LocalStackVariable("foo", PrimitiveTypes::INT_TYPE, fooValue);
  LocalStackVariable* barVariable =
    new LocalStackVariable("bar", PrimitiveTypes::INT_TYPE, barValue);
  scopes.setVariable(fooVariable);
  scopes.pushScope();
  scopes.setVariable(barVariable);
  
  EXPECT_EQ(scopes.getVariable("bar")->getValue(), barValue);
  EXPECT_EQ(scopes.getVariable("foo")->getValue(), fooValue);
  
  scopes.popScope(NULL);
  EXPECT_EQ(scopes.getVariable("foo")->getValue(), fooValue);
  EXPECT_EQ(scopes.getVariable("bar"), nullptr);
  
  barVariable = new LocalStackVariable("bar", PrimitiveTypes::INT_TYPE, barValue);
  scopes.setVariable(barVariable);
  EXPECT_EQ(scopes.getVariable("foo")->getValue(), fooValue);
  EXPECT_EQ(scopes.getVariable("bar")->getValue(), barValue);
  
  scopes.popScope(NULL);
  EXPECT_EQ(scopes.getVariable("foo"), nullptr);
  EXPECT_EQ(scopes.getVariable("bar"), nullptr);
}

TEST(ScopesTest, TestScopesCorrectlyOrdered) {
  Scopes scopes;
  LLVMContext llvmContext;
  
  scopes.pushScope();
  Value* outerValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 3);
  Value* innerValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 5);
  
  LocalStackVariable* outerVariable =
    new LocalStackVariable("foo", PrimitiveTypes::INT_TYPE, outerValue);
  LocalStackVariable* innerVariable =
    new LocalStackVariable("foo", PrimitiveTypes::INT_TYPE, innerValue);

  scopes.setVariable(outerVariable);
  scopes.pushScope();
  scopes.setVariable(innerVariable);
  
  EXPECT_EQ(scopes.getVariable("foo")->getValue(), innerValue);
  
  scopes.popScope(NULL);
  
  EXPECT_EQ(scopes.getVariable("foo")->getValue(), outerValue);
}

TEST(ScopesTest, TestReturnType) {
  Scopes scopes;
  LLVMContext llvmContext;

  scopes.pushScope();
  scopes.setReturnType(PrimitiveTypes::DOUBLE_TYPE);
  scopes.pushScope();
  scopes.pushScope();
  
  IType* returnType = scopes.getReturnType();
  EXPECT_EQ(returnType, PrimitiveTypes::DOUBLE_TYPE);
}

TEST(ScopesTest, TestGetScopeDeathTest) {
  Scopes scopes;

  EXPECT_EXIT(scopes.getScope(),
              ::testing::ExitedWithCode(1),
              "Error: Can not get scope. Scope list is empty.");
}

TEST(ScopesTest, TestGetScope) {
  Scopes scopes;
  
  scopes.pushScope();
  Scope* scope = scopes.getScope();
  
  EXPECT_NE(scope, nullptr);
}

TEST(ScopesTest, TestClearVariable) {
  Scopes scopes;
  LLVMContext llvmContext;
  Value* fooValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 3);
  
  scopes.pushScope();
  LocalStackVariable* fooVariable =
    new LocalStackVariable("foo", PrimitiveTypes::INT_TYPE, fooValue);
  scopes.setVariable(fooVariable);
  
  EXPECT_EQ(scopes.getVariable("foo")->getValue(), fooValue);
  
  scopes.clearVariable("foo");
  
  EXPECT_EQ(scopes.getVariable("foo"), nullptr);
}

TEST(ScopesTest, TestClearVariableDeathTest) {
  Scopes scopes;
  
  EXPECT_EXIT(scopes.clearVariable("foo"),
              ::testing::ExitedWithCode(1),
              "Error: Could not clear variable 'foo': the Scopes stack is empty");
  
  scopes.pushScope();
  
  EXPECT_EXIT(scopes.clearVariable("foo"),
              ::testing::ExitedWithCode(1),
              "Error: Could not clear variable 'foo': it was not found");
}

TEST(ScopesTest, TestSetHeapVariable) {
  Scopes scopes;
  LLVMContext llvmContext;
  scopes.pushScope();
  Value* fooValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 3);
  LocalHeapVariable* heapVariable =
    new LocalHeapVariable("foo", PrimitiveTypes::INT_TYPE, fooValue);
  scopes.setVariable(heapVariable);
  
  ASSERT_NE(scopes.getVariable("foo"), nullptr);
  EXPECT_EQ(scopes.getVariable("foo")->getValue(), fooValue);
}

TEST(ScopesTest, TestSetUnitializedHeapVariable) {
  Scopes scopes;
  scopes.pushScope();
  
  LocalHeapVariable* unitializedHeapVariable =
    new LocalHeapVariable("foo", PrimitiveTypes::INT_TYPE, NULL);
  scopes.setVariable(unitializedHeapVariable);
  
  ASSERT_NE(scopes.getVariable("foo"), nullptr);
  EXPECT_EQ(scopes.getVariable("foo")->getValue(), nullptr);
}
