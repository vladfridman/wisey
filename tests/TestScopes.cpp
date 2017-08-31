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
#include "TestFileSampleRunner.hpp"
#include "wisey/Catch.hpp"
#include "wisey/EmptyStatement.hpp"
#include "wisey/HeapReferenceVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/Model.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/StackVariable.hpp"

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
  Interface* mInterface;
  
  ScopesTest() : mLLVMContext(mContext.getLLVMContext()) {
    mInterface = new Interface("systems.vos.wisey.compiler.tests.IInterface", NULL);
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
  
  mScopes.clearVariable(mContext, "foo");
  
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

  mScopes.clearVariable(mContext, "foo");
  
  map<string, IVariable*> clearedVariables = mScopes.getClearedVariables();
  EXPECT_EQ(clearedVariables.size(), 1u);
  EXPECT_EQ(clearedVariables.count("foo"), 1u);
  EXPECT_EQ(clearedVariables.at("foo"), fooVariable);

  mScopes.clearVariable(mContext, "bar");
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
  
  mScopes.clearVariable(mContext, "foo");
  
  map<string, IVariable*> clearedVariables = mScopes.getClearedVariables();
  EXPECT_EQ(clearedVariables.size(), 1u);
  EXPECT_EQ(clearedVariables.count("foo"), 1u);
  EXPECT_EQ(clearedVariables.at("foo"), fooVariable);
  
  mScopes.eraseFromClearedVariables(fooVariable);
  EXPECT_EQ(mScopes.getClearedVariables().size(), 0u);
}


TEST_F(ScopesTest, clearVariableDeathTest) {
  EXPECT_EXIT(mScopes.clearVariable(mContext, "foo"),
              ::testing::ExitedWithCode(1),
              "Error: Could not clear variable 'foo': the Scopes stack is empty");
  
  mScopes.pushScope();
  
  EXPECT_EXIT(mScopes.clearVariable(mContext, "foo"),
              ::testing::ExitedWithCode(1),
              "Error: Could not clear variable 'foo': it was not found");
}

TEST_F(ScopesTest, setHeapVariableTest) {
  mScopes.pushScope();
  Value* fooValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  IVariable* heapVariable =
    new HeapReferenceVariable("foo", PrimitiveTypes::INT_TYPE, fooValue);
  mScopes.setVariable(heapVariable);
  
  ASSERT_NE(mScopes.getVariable("foo"), nullptr);
  EXPECT_EQ(mScopes.getVariable("foo")->getValue(), fooValue);
}

TEST_F(ScopesTest, setUnitializedHeapVariableTest) {
  mScopes.pushScope();
  
  IVariable* unitializedHeapVariable =
    new HeapReferenceVariable("foo", PrimitiveTypes::INT_TYPE, NULL);
  mScopes.setVariable(unitializedHeapVariable);
  
  ASSERT_NE(mScopes.getVariable("foo"), nullptr);
  EXPECT_EQ(mScopes.getVariable("foo")->getValue(), nullptr);
}

TEST_F(ScopesTest, setTryCatchInfoTest) {
  mScopes.pushScope();
  BasicBlock* basicBlock = BasicBlock::Create(mLLVMContext);
  vector<Catch*> catchList;
  TryCatchInfo* tryCatchInfo = new TryCatchInfo(basicBlock, basicBlock, NULL, catchList);
  
  mScopes.setTryCatchInfo(tryCatchInfo);
  mScopes.pushScope();
  
  ASSERT_EQ(mScopes.getTryCatchInfo(), tryCatchInfo);
}

TEST_F(ScopesTest, crearTryCatchInfoTest) {
  mScopes.pushScope();
  BasicBlock* basicBlock = BasicBlock::Create(mLLVMContext);
  vector<Catch*> catchList;
  TryCatchInfo* tryCatchInfo = new TryCatchInfo(basicBlock, basicBlock, NULL, catchList);
  
  mScopes.setTryCatchInfo(tryCatchInfo);
  mScopes.pushScope();
  mScopes.popScope(mContext);
  mScopes.clearTryCatchInfo();
  
  ASSERT_EQ(mScopes.getTryCatchInfo(), nullptr);
}

TEST_F(ScopesTest, mergeNestedCatchListsTest) {
  mContext.setPackage("systems.vos.wisey.compiler.tests");
  EmptyStatement* emptyStatement = new EmptyStatement();
  BasicBlock* basicBlock = BasicBlock::Create(mLLVMContext);
  vector<string> package;
  mScopes.pushScope();

  ModelTypeSpecifier* exception1TypeSpecifier = new ModelTypeSpecifier(package, "MException1");
  StructType* exceptionModel1StructType = StructType::create(mLLVMContext, "MException1");
  Model* exceptionModel1 = new Model("systems.vos.wisey.compiler.tests.MException1",
                                     exceptionModel1StructType);
  mContext.addModel(exceptionModel1);

  ModelTypeSpecifier* exception2TypeSpecifier = new ModelTypeSpecifier(package, "MException2");
  StructType* exceptionModel2StructType = StructType::create(mLLVMContext, "MException2");
  Model* exceptionModel2 = new Model("systems.vos.wisey.compiler.tests.MException2",
                                     exceptionModel2StructType);
  mContext.addModel(exceptionModel2);

  ModelTypeSpecifier* exception3TypeSpecifier = new ModelTypeSpecifier(package, "MException3");
  StructType* exceptionModel3StructType = StructType::create(mLLVMContext, "MException3");
  Model* exceptionModel3 = new Model("systems.vos.wisey.compiler.tests.MException3",
                                     exceptionModel3StructType);
  mContext.addModel(exceptionModel3);

  Catch* catch1 = new Catch(exception1TypeSpecifier, "exception", emptyStatement);
  Catch* catch2 = new Catch(exception2TypeSpecifier, "exception", emptyStatement);
  Catch* catch3 = new Catch(exception2TypeSpecifier, "exception", emptyStatement);
  Catch* catch4 = new Catch(exception3TypeSpecifier, "exception", emptyStatement);
  
  vector<Catch*> catchList1;
  catchList1.push_back(catch1);
  TryCatchInfo* tryCatchInfo1 = new TryCatchInfo(basicBlock, basicBlock, NULL, catchList1);

  vector<Catch*> catchList2;
  catchList2.push_back(catch2);
  TryCatchInfo* tryCatchInfo2 = new TryCatchInfo(basicBlock, basicBlock, NULL, catchList2);

  vector<Catch*> catchList3;
  catchList3.push_back(catch3);
  catchList3.push_back(catch4);
  TryCatchInfo* tryCatchInfo3 = new TryCatchInfo(basicBlock, basicBlock, NULL, catchList3);
  
  mScopes.pushScope();
  mScopes.setTryCatchInfo(tryCatchInfo1);
  mScopes.pushScope();
  mScopes.setTryCatchInfo(tryCatchInfo2);
  mScopes.pushScope();
  mScopes.setTryCatchInfo(tryCatchInfo3);
  
  vector<Catch*> mergedCatches = mScopes.mergeNestedCatchLists(mContext);
  
  EXPECT_EQ(mergedCatches.size(), 3u);
  EXPECT_EQ(mergedCatches.at(0), catch3);
  EXPECT_EQ(mergedCatches.at(1), catch4);
  EXPECT_EQ(mergedCatches.at(2), catch1);
}

TEST_F(ScopesTest, reportUnhandledExceptionsDeathTest) {
  mScopes.pushScope();
  
  StructType* exceptionModelStructType = StructType::create(mLLVMContext, "MException");
  Model* exceptionModel = new Model("MException", exceptionModelStructType);

  mScopes.getScope()->addException(exceptionModel);
  
  EXPECT_EXIT(mScopes.popScope(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Exception MException is not handled");
}

TEST_F(ScopesTest, freeOwnedMemoryTest) {
  NiceMock<MockVariable> foo;
  NiceMock<MockVariable> bar;

  ON_CALL(foo, getName()).WillByDefault(Return("foo"));
  ON_CALL(foo, getType()).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  ON_CALL(bar, getName()).WillByDefault(Return("bar"));
  ON_CALL(bar, getType()).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  
  mScopes.pushScope();
  mScopes.setVariable(&foo);
  mScopes.pushScope();
  mScopes.setVariable(&bar);
  
  EXPECT_CALL(foo, free(_));
  EXPECT_CALL(bar, free(_));
  
  mScopes.freeOwnedMemory(mContext);
}

TEST_F(ScopesTest, addReferenceToOwnerVariableTest) {
  NiceMock<MockVariable> foo;
  NiceMock<MockVariable> bar;
  
  ON_CALL(foo, getName()).WillByDefault(Return("foo"));
  ON_CALL(foo, getType()).WillByDefault(Return(mInterface->getOwner()));
  ON_CALL(bar, getName()).WillByDefault(Return("bar"));
  ON_CALL(bar, getType()).WillByDefault(Return(mInterface));
  
  mScopes.addReferenceToOwnerVariable(&foo, &bar);
  
  EXPECT_EQ(mScopes.getOwnersForReference(&bar).front(), &foo);
}

TEST_F(TestFileSampleRunner, referenceMemoryDeallocatedByPassingOwnerRunDeathTest) {
  expectFailCompile("tests/samples/test_reference_memory_deallocated_by_passing_owner.yz",
                    1,
                    "Error: Variable 'anotherRef' was previously cleared and can not be used");
}

TEST_F(TestFileSampleRunner, referenceMemoryDeallocatedBySettingNullOutsideObjectRunDeathTest) {
  expectFailCompile("tests/samples/test_reference_memory_deallocated_by_setting_null_outside_object.yz",
                    1,
                    "Error: Can not store a reference because its owner does not exist in "
                    "the current scope");
}


TEST_F(TestFileSampleRunner, referenceMemoryDeallocatedByPassingOwnerInsideIfThenElseRunDeathTest) {
  expectFailCompile("tests/samples/"
                    "test_reference_memory_deallocated_by_passing_owner_inside_if_then_else.yz",
                    1,
                    "Error: Variable 'data' was previously cleared and can not be used");
}

TEST_F(TestFileSampleRunner, referenceMemoryDeallocatedByPassingOwnerReuseReferenceRunTest) {
  runFile("tests/samples/"
          "test_reference_memory_deallocated_by_passing_owner_reuse_reference.yz",
          "5");
}

TEST_F(TestFileSampleRunner, referenceMemoryDeallocatedBySettingNullTest) {
  compileAndRunFile("tests/samples/test_reference_memory_deallocated_by_setting_null.yz", 11);
}
