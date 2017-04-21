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

#include "MockType.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/LocalHeapVariable.hpp"
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

TEST(ScopesTest, scopesTest) {
  Scopes scopes;
  IRGenerationContext context;
  LLVMContext& llvmContext = context.getLLVMContext();
  
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
  
  scopes.popScope(context);
  EXPECT_EQ(scopes.getVariable("foo")->getValue(), fooValue);
  EXPECT_EQ(scopes.getVariable("bar"), nullptr);
  
  barVariable = new LocalStackVariable("bar", PrimitiveTypes::INT_TYPE, barValue);
  scopes.setVariable(barVariable);
  EXPECT_EQ(scopes.getVariable("foo")->getValue(), fooValue);
  EXPECT_EQ(scopes.getVariable("bar")->getValue(), barValue);
  
  scopes.popScope(context);
  EXPECT_EQ(scopes.getVariable("foo"), nullptr);
  EXPECT_EQ(scopes.getVariable("bar"), nullptr);
}

TEST(ScopesTest, scopesCorrectlyOrderedTest) {
  Scopes scopes;
  IRGenerationContext context;
  LLVMContext& llvmContext = context.getLLVMContext();
  
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
  
  scopes.popScope(context);
  
  EXPECT_EQ(scopes.getVariable("foo")->getValue(), outerValue);
}

TEST(ScopesTest, returnTypeTest) {
  Scopes scopes;
  LLVMContext llvmContext;

  scopes.pushScope();
  scopes.setReturnType(PrimitiveTypes::DOUBLE_TYPE);
  scopes.pushScope();
  scopes.pushScope();
  
  IType* returnType = scopes.getReturnType();
  EXPECT_EQ(returnType, PrimitiveTypes::DOUBLE_TYPE);
}

TEST(ScopesTest, getScopeDeathTest) {
  Scopes scopes;

  EXPECT_EXIT(scopes.getScope(),
              ::testing::ExitedWithCode(1),
              "Error: Can not get scope. Scope list is empty.");
}

TEST(ScopesTest, getScopeTest) {
  Scopes scopes;
  
  scopes.pushScope();
  Scope* scope = scopes.getScope();
  
  EXPECT_NE(scope, nullptr);
}

TEST(ScopesTest, clearVariableTest) {
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

TEST(ScopesTest, clearVariableDeathTest) {
  Scopes scopes;
  
  EXPECT_EXIT(scopes.clearVariable("foo"),
              ::testing::ExitedWithCode(1),
              "Error: Could not clear variable 'foo': the Scopes stack is empty");
  
  scopes.pushScope();
  
  EXPECT_EXIT(scopes.clearVariable("foo"),
              ::testing::ExitedWithCode(1),
              "Error: Could not clear variable 'foo': it was not found");
}

TEST(ScopesTest, setHeapVariableTest) {
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

TEST(ScopesTest, setUnitializedHeapVariableTest) {
  Scopes scopes;
  scopes.pushScope();
  
  LocalHeapVariable* unitializedHeapVariable =
    new LocalHeapVariable("foo", PrimitiveTypes::INT_TYPE, NULL);
  scopes.setVariable(unitializedHeapVariable);
  
  ASSERT_NE(scopes.getVariable("foo"), nullptr);
  EXPECT_EQ(scopes.getVariable("foo")->getValue(), nullptr);
}

TEST(ScopesTest, reportUnhandledExceptionsDeathTest) {
  IRGenerationContext context;
  Scopes scopes;
  scopes.pushScope();

  NiceMock<MockType> mockExceptionType;
  Mock::AllowLeak(&mockExceptionType);
  ON_CALL(mockExceptionType, getName()).WillByDefault(Return("MExceptions"));
  scopes.getScope()->addException(&mockExceptionType);
  
  EXPECT_EXIT(scopes.popScope(context),
              ::testing::ExitedWithCode(1),
              "Error: Exception MExceptions is not handled");
}
