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
#include "MockOwnerVariable.hpp"
#include "MockType.hpp"
#include "MockVariable.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/Catch.hpp"
#include "wisey/EmptyStatement.hpp"
#include "wisey/FinallyBlock.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LocalPrimitiveVariable.hpp"
#include "wisey/LocalReferenceVariable.hpp"
#include "wisey/Model.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
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
  Interface* mInterface;
  
  ScopesTest() : mLLVMContext(mContext.getLLVMContext()) {
    vector<InterfaceTypeSpecifier*> parentInterfaces;
    vector<IObjectElementDeclaration*> objectElements;
    mInterface = Interface::newInterface("systems.vos.wisey.compiler.tests.IInterface",
                                         StructType::create(mLLVMContext, ""),
                                         parentInterfaces,
                                         objectElements);
  }
};

TEST_F(ScopesTest, scopesTest) {
  mScopes.pushScope();
  Value* fooValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  Value* barValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
  
  LocalPrimitiveVariable* fooVariable =
  new LocalPrimitiveVariable("foo", PrimitiveTypes::INT_TYPE, fooValue);
  LocalPrimitiveVariable* barVariable =
  new LocalPrimitiveVariable("bar", PrimitiveTypes::INT_TYPE, barValue);
  mScopes.setVariable(fooVariable);
  mScopes.pushScope();
  mScopes.setVariable(barVariable);
  
  EXPECT_NE(mScopes.getVariable("bar"), nullptr);
  EXPECT_NE(mScopes.getVariable("foo"), nullptr);
  
  mScopes.popScope(mContext);
  EXPECT_NE(mScopes.getVariable("foo"), nullptr);
  EXPECT_EQ(mScopes.getVariable("bar"), nullptr);
  
  barVariable = new LocalPrimitiveVariable("bar", PrimitiveTypes::INT_TYPE, barValue);
  mScopes.setVariable(barVariable);
  EXPECT_NE(mScopes.getVariable("foo"), nullptr);
  EXPECT_NE(mScopes.getVariable("bar"), nullptr);
  
  mScopes.popScope(mContext);
  EXPECT_EQ(mScopes.getVariable("foo"), nullptr);
  EXPECT_EQ(mScopes.getVariable("bar"), nullptr);
}

TEST_F(ScopesTest, scopesCorrectlyOrderedTest) {
  mScopes.pushScope();
  Value* outerValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  Value* innerValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
  
  LocalPrimitiveVariable* outerVariable =
  new LocalPrimitiveVariable("foo", PrimitiveTypes::INT_TYPE, outerValue);
  LocalPrimitiveVariable* innerVariable =
  new LocalPrimitiveVariable("bar", PrimitiveTypes::INT_TYPE, innerValue);
  
  mScopes.setVariable(outerVariable);
  mScopes.pushScope();
  mScopes.setVariable(innerVariable);
  
  EXPECT_NE(mScopes.getVariable("bar"), nullptr);
  
  mScopes.popScope(mContext);
  
  EXPECT_NE(mScopes.getVariable("foo"), nullptr);
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
  LocalPrimitiveVariable* fooVariable =
  new LocalPrimitiveVariable("foo", PrimitiveTypes::INT_TYPE, fooValue);
  mScopes.setVariable(fooVariable);
  
  EXPECT_EQ(mScopes.getVariable("foo"), fooVariable);
  
  mScopes.clearVariable(mContext, "foo");
  
  EXPECT_EQ(mScopes.getVariable("foo"), nullptr);
  EXPECT_EQ(mScopes.getVariableForAssignement("foo"), fooVariable);
}

TEST_F(ScopesTest, getClearedVariablesTest) {
  mScopes.pushScope();
  Value* value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  LocalPrimitiveVariable* fooVariable =
  new LocalPrimitiveVariable("foo", PrimitiveTypes::INT_TYPE, value);
  LocalPrimitiveVariable* barVariable =
  new LocalPrimitiveVariable("bar", PrimitiveTypes::INT_TYPE, value);
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
  LocalPrimitiveVariable* fooVariable =
  new LocalPrimitiveVariable("foo", PrimitiveTypes::INT_TYPE, value);
  LocalPrimitiveVariable* barVariable =
  new LocalPrimitiveVariable("bar", PrimitiveTypes::INT_TYPE, value);
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
  LocalPrimitiveVariable* fooVariable =
  new LocalPrimitiveVariable("foo", PrimitiveTypes::INT_TYPE, value);
  LocalPrimitiveVariable* barVariable =
  new LocalPrimitiveVariable("bar", PrimitiveTypes::INT_TYPE, value);
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

TEST_F(ScopesTest, setLocalReferenceVariableTest) {
  mScopes.pushScope();
  Value* fooValue = ConstantPointerNull::get(mInterface->getLLVMType(mLLVMContext)->getPointerTo());
  IVariable* variable =
    new LocalReferenceVariable("foo", mInterface, fooValue);
  mScopes.setVariable(variable);
  
  EXPECT_NE(mScopes.getVariable("foo"), nullptr);
}

TEST_F(ScopesTest, setUnitializedLocalReferenceVariableTest) {
  mScopes.pushScope();
  
  Value* store = ConstantPointerNull::get(mInterface->getLLVMType(mLLVMContext)->getPointerTo());
  IVariable* unitializedLocalReferenceVariable =
    new LocalReferenceVariable("foo", mInterface, store);
  mScopes.setVariable(unitializedLocalReferenceVariable);
  
  IVariable* variable = mScopes.getVariable("foo");
  
  ASSERT_NE(variable, nullptr);
  EXPECT_EXIT(variable->generateIdentifierIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Variable 'foo' is used before it is initialized");
              
}

TEST_F(ScopesTest, setTryCatchInfoTest) {
  mScopes.pushScope();
  BasicBlock* basicBlock = BasicBlock::Create(mLLVMContext);
  vector<Catch*> catchList;
  FinallyBlock* emptyBlock = new FinallyBlock();
  TryCatchInfo* tryCatchInfo = new TryCatchInfo(basicBlock, emptyBlock, catchList);
  
  mScopes.setTryCatchInfo(tryCatchInfo);
  mScopes.pushScope();
  
  ASSERT_EQ(mScopes.getTryCatchInfo(), tryCatchInfo);
}

TEST_F(ScopesTest, clearTryCatchInfoTest) {
  mScopes.pushScope();
  BasicBlock* basicBlock = BasicBlock::Create(mLLVMContext);
  vector<Catch*> catchList;
  FinallyBlock* emptyBlock = new FinallyBlock();
  TryCatchInfo* tryCatchInfo = new TryCatchInfo(basicBlock, emptyBlock, catchList);
  
  mScopes.setTryCatchInfo(tryCatchInfo);
  mScopes.pushScope();
  mScopes.popScope(mContext);
  mScopes.clearTryCatchInfo();
  
  ASSERT_EQ(mScopes.getTryCatchInfo(), nullptr);
}

TEST_F(ScopesTest, setObjectTypeTest) {
  mScopes.pushScope();
  Model* model = Model::newModel("MModel", NULL);
  
  mScopes.setObjectType(model);
  mScopes.pushScope();
  mScopes.pushScope();
  
  ASSERT_EQ(mScopes.getObjectType(), model);
}

TEST_F(ScopesTest, mergeNestedCatchListsTest) {
  mContext.setPackage("systems.vos.wisey.compiler.tests");
  EmptyStatement* emptyStatement = new EmptyStatement();
  BasicBlock* basicBlock = BasicBlock::Create(mLLVMContext);
  vector<string> package;
  mScopes.pushScope();

  ModelTypeSpecifier* exception1TypeSpecifier = new ModelTypeSpecifier(package, "MException1");
  StructType* exceptionModel1StructType = StructType::create(mLLVMContext, "MException1");
  Model* exceptionModel1 = Model::newModel("systems.vos.wisey.compiler.tests.MException1",
                                           exceptionModel1StructType);
  mContext.addModel(exceptionModel1);

  ModelTypeSpecifier* exception2TypeSpecifier = new ModelTypeSpecifier(package, "MException2");
  StructType* exceptionModel2StructType = StructType::create(mLLVMContext, "MException2");
  Model* exceptionModel2 = Model::newModel("systems.vos.wisey.compiler.tests.MException2",
                                           exceptionModel2StructType);
  mContext.addModel(exceptionModel2);

  ModelTypeSpecifier* exception3TypeSpecifier = new ModelTypeSpecifier(package, "MException3");
  StructType* exceptionModel3StructType = StructType::create(mLLVMContext, "MException3");
  Model* exceptionModel3 = Model::newModel("systems.vos.wisey.compiler.tests.MException3",
                                           exceptionModel3StructType);
  mContext.addModel(exceptionModel3);

  Catch* catch1 = new Catch(exception1TypeSpecifier, "exception", emptyStatement);
  Catch* catch2 = new Catch(exception2TypeSpecifier, "exception", emptyStatement);
  Catch* catch3 = new Catch(exception2TypeSpecifier, "exception", emptyStatement);
  Catch* catch4 = new Catch(exception3TypeSpecifier, "exception", emptyStatement);
  
  vector<Catch*> catchList1;
  catchList1.push_back(catch1);
  FinallyBlock* emptyBlock = new FinallyBlock();
  TryCatchInfo* tryCatchInfo1 = new TryCatchInfo(basicBlock, emptyBlock, catchList1);

  vector<Catch*> catchList2;
  catchList2.push_back(catch2);
  TryCatchInfo* tryCatchInfo2 = new TryCatchInfo(basicBlock, emptyBlock, catchList2);

  vector<Catch*> catchList3;
  catchList3.push_back(catch3);
  catchList3.push_back(catch4);
  
  mScopes.pushScope();
  mScopes.setTryCatchInfo(tryCatchInfo1);
  mScopes.pushScope();
  mScopes.setTryCatchInfo(tryCatchInfo2);
  mScopes.pushScope();
  
  vector<Catch*> mergedCatches = mScopes.mergeNestedCatchLists(mContext, catchList3);
  
  EXPECT_EQ(mergedCatches.size(), 3u);
  EXPECT_EQ(mergedCatches.at(0), catch3);
  EXPECT_EQ(mergedCatches.at(1), catch4);
  EXPECT_EQ(mergedCatches.at(2), catch1);
}

TEST_F(ScopesTest, reportUnhandledExceptionsDeathTest) {
  mScopes.pushScope();
  
  StructType* exceptionModelStructType = StructType::create(mLLVMContext, "MException");
  Model* exceptionModel = Model::newModel("MException", exceptionModelStructType);

  mScopes.getScope()->addException(exceptionModel);
  
  EXPECT_EXIT(mScopes.popScope(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Exception MException is not handled");
}

TEST_F(ScopesTest, freeOwnedMemoryTest) {
  NiceMock<MockOwnerVariable> foo;
  NiceMock<MockOwnerVariable> bar;

  ON_CALL(foo, getName()).WillByDefault(Return("foo"));
  ON_CALL(foo, getType()).WillByDefault(Return(mInterface->getOwner()));
  ON_CALL(bar, getName()).WillByDefault(Return("bar"));
  ON_CALL(bar, getType()).WillByDefault(Return(mInterface->getOwner()));
  
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
  
  EXPECT_EQ(mScopes.getOwnersForReference(&bar).begin()->second, &foo);
}

TEST_F(ScopesTest, variableHidingDeathTest) {
  mScopes.pushScope();
  Value* outerValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  Value* innerValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
  
  LocalPrimitiveVariable* outerVariable =
  new LocalPrimitiveVariable("foo", PrimitiveTypes::INT_TYPE, outerValue);
  LocalPrimitiveVariable* innerVariable =
  new LocalPrimitiveVariable("foo", PrimitiveTypes::INT_TYPE, innerValue);
  
  mScopes.setVariable(outerVariable);
  mScopes.pushScope();
  EXPECT_EXIT(mScopes.setVariable(innerVariable),
              ::testing::ExitedWithCode(1),
              "Error: Already declared variable named 'foo'. Variable hiding is not allowed.");
}

TEST_F(TestFileSampleRunner, variableHidingRunDeathTest) {
  expectFailCompile("tests/samples/test_variable_hiding.yz",
                    1,
                    "Error: Already declared variable named 'var'. "
                    "Variable hiding is not allowed.");
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

TEST_F(TestFileSampleRunner, destroyedObjectStillInUseRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_destroyed_object_still_in_use.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MDestroyedObjectStillInUseException\n"
                               "  at systems.vos.wisey.compiler.tests.CService.clear(tests/samples/test_destroyed_object_still_in_use.yz:22)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_destroyed_object_still_in_use.yz:31)\n"
                               "Details: Object referenced by expression still has 2 active references\n");
}

TEST_F(TestFileSampleRunner, nullPointerExceptionRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_null_pointer_exception.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CService.getValue(tests/samples/test_null_pointer_exception.yz:25)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_null_pointer_exception.yz:36)\n");
}
