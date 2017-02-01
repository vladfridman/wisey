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
  
  scopes.setStackVariable("foo", fooValue);
  scopes.pushScope();
  scopes.setStackVariable("bar", barValue);
  
  EXPECT_EQ(scopes.getVariable("bar")->getValue(), barValue);
  EXPECT_EQ(scopes.getVariable("foo")->getValue(), fooValue);
  
  scopes.popScope(NULL);
  EXPECT_EQ(scopes.getVariable("foo")->getValue(), fooValue);
  EXPECT_EQ(scopes.getVariable("bar") == NULL, true);
  
  scopes.setStackVariable("bar", barValue);
  EXPECT_EQ(scopes.getVariable("foo")->getValue(), fooValue);
  EXPECT_EQ(scopes.getVariable("bar")->getValue(), barValue);
  
  scopes.popScope(NULL);
  EXPECT_EQ(scopes.getVariable("foo") == NULL, true);
  EXPECT_EQ(scopes.getVariable("bar") == NULL, true);
}

TEST(ScopesTest, TestScopesCorrectlyOrdered) {
  Scopes scopes;
  LLVMContext llvmContext;
  
  scopes.pushScope();
  Value* outerValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 3);
  Value* innerValue = ConstantInt::get(Type::getInt32Ty(llvmContext), 5);
  
  scopes.setStackVariable("foo", outerValue);
  scopes.pushScope();
  scopes.setStackVariable("foo", innerValue);
  
  EXPECT_EQ(scopes.getVariable("foo")->getValue(), innerValue);
  
  scopes.popScope(NULL);
  
  EXPECT_EQ(scopes.getVariable("foo")->getValue(), outerValue);
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
  
  EXPECT_EQ(scope == NULL, false);
}
