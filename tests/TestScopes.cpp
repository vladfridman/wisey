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
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "Catch.hpp"
#include "EmptyStatement.hpp"
#include "IRGenerationContext.hpp"
#include "LocalPrimitiveVariable.hpp"
#include "LocalReferenceVariable.hpp"
#include "Model.hpp"
#include "ModelTypeSpecifier.hpp"
#include "PrimitiveTypes.hpp"

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
    TestPrefix::generateIR(mContext);
    
    vector<IInterfaceTypeSpecifier*> parentInterfaces;
    vector<IObjectElementDefinition*> objectElements;
    mInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                         "systems.vos.wisey.compiler.tests.IInterface",
                                         StructType::create(mLLVMContext, ""),
                                         parentInterfaces,
                                         objectElements,
                                         mContext.getImportProfile(),
                                         0);
  }
};

TEST_F(ScopesTest, scopesTest) {
  mScopes.pushScope();
  Value* fooValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  Value* barValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
  
  LocalPrimitiveVariable* fooVariable =
  new LocalPrimitiveVariable("foo", PrimitiveTypes::INT, fooValue, 0);
  LocalPrimitiveVariable* barVariable =
  new LocalPrimitiveVariable("bar", PrimitiveTypes::INT, barValue, 0);
  mScopes.setVariable(mContext, fooVariable);
  mScopes.pushScope();
  mScopes.setVariable(mContext, barVariable);
  
  EXPECT_NE(mScopes.getVariable("bar"), nullptr);
  EXPECT_NE(mScopes.getVariable("foo"), nullptr);
  
  mScopes.popScope(mContext, 0);
  EXPECT_NE(mScopes.getVariable("foo"), nullptr);
  EXPECT_EQ(mScopes.getVariable("bar"), nullptr);
  
  barVariable = new LocalPrimitiveVariable("bar", PrimitiveTypes::INT, barValue, 0);
  mScopes.setVariable(mContext, barVariable);
  EXPECT_NE(mScopes.getVariable("foo"), nullptr);
  EXPECT_NE(mScopes.getVariable("bar"), nullptr);
  
  mScopes.popScope(mContext, 0);
  EXPECT_EQ(mScopes.getVariable("foo"), nullptr);
  EXPECT_EQ(mScopes.getVariable("bar"), nullptr);
}

TEST_F(ScopesTest, scopesCorrectlyOrderedTest) {
  mScopes.pushScope();
  Value* outerValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  Value* innerValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
  
  LocalPrimitiveVariable* outerVariable =
  new LocalPrimitiveVariable("foo", PrimitiveTypes::INT, outerValue, 0);
  LocalPrimitiveVariable* innerVariable =
  new LocalPrimitiveVariable("bar", PrimitiveTypes::INT, innerValue, 0);
  
  mScopes.setVariable(mContext, outerVariable);
  mScopes.pushScope();
  mScopes.setVariable(mContext, innerVariable);
  
  EXPECT_NE(mScopes.getVariable("bar"), nullptr);
  
  mScopes.popScope(mContext, 0);
  
  EXPECT_NE(mScopes.getVariable("foo"), nullptr);
}

TEST_F(ScopesTest, returnTypeTest) {
  mScopes.pushScope();
  mScopes.setReturnType(PrimitiveTypes::DOUBLE);
  mScopes.pushScope();
  mScopes.pushScope();
  
  const IType* returnType = mScopes.getReturnType();
  EXPECT_EQ(returnType, PrimitiveTypes::DOUBLE);
}

TEST_F(ScopesTest, getScopeTest) {
  mScopes.pushScope();
  Scope* scope = mScopes.getScope();
  
  EXPECT_NE(scope, nullptr);
}

TEST_F(ScopesTest, setLocalReferenceVariableTest) {
  mScopes.pushScope();
  Value* fooValue = ConstantPointerNull::get(mInterface->getLLVMType(mContext)->getPointerTo());
  IVariable* variable =
    new LocalReferenceVariable("foo", mInterface, fooValue, 0);
  mScopes.setVariable(mContext, variable);
  
  EXPECT_NE(mScopes.getVariable("foo"), nullptr);
}

TEST_F(ScopesTest, setUnitializedLocalReferenceVariableTest) {
  mScopes.pushScope();
  
  Value* store = ConstantPointerNull::get(mInterface->getLLVMType(mContext)->getPointerTo());
  IVariable* unitializedLocalReferenceVariable = new LocalReferenceVariable("foo",
                                                                            mInterface,
                                                                            store,
                                                                            0);
  mScopes.setVariable(mContext, unitializedLocalReferenceVariable);
  
  IVariable* variable = mScopes.getVariable("foo");
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  ASSERT_NE(variable, nullptr);
  EXPECT_ANY_THROW(variable->generateIdentifierIR(mContext, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Variable 'foo' is used before it is initialized\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(ScopesTest, beginTryCatchTest) {
  mScopes.pushScope();
  BasicBlock* basicBlock = BasicBlock::Create(mLLVMContext);
  vector<Catch*> catchList;
  TryCatchInfo* tryCatchInfo = new TryCatchInfo(catchList, basicBlock);
  
  mScopes.beginTryCatch(tryCatchInfo);
  mScopes.pushScope();
  
  ASSERT_EQ(mScopes.getTryCatchInfo(), tryCatchInfo);
}

TEST_F(ScopesTest, endTryCatchTest) {
  mScopes.pushScope();
  BasicBlock* basicBlock = BasicBlock::Create(mLLVMContext);
  vector<Catch*> catchList;
  TryCatchInfo* tryCatchInfo = new TryCatchInfo(catchList, basicBlock);
  
  mScopes.beginTryCatch(tryCatchInfo);
  mScopes.pushScope();
  mScopes.popScope(mContext, 0);
  mScopes.endTryCatch(mContext);
  
  ASSERT_EQ(mScopes.getTryCatchInfo(), nullptr);
}

TEST_F(ScopesTest, reportUnhandledExceptionsDeathTest) {
  mScopes.pushScope();
  
  StructType* exceptionModelStructType = StructType::create(mLLVMContext, "MException");
  Model* exceptionModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                                          "MException",
                                          exceptionModelStructType,
                                          mContext.getImportProfile(),
                                          0);

  mScopes.getScope()->addException(exceptionModel, 3);
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mScopes.popScope(mContext, 0));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Exception MException is not handled\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(ScopesTest, freeOwnedMemoryTest) {
  NiceMock<MockOwnerVariable> foo;
  NiceMock<MockOwnerVariable> bar;

  ON_CALL(foo, getName()).WillByDefault(Return("foo"));
  ON_CALL(foo, getType()).WillByDefault(Return(mInterface->getOwner()));
  ON_CALL(bar, getName()).WillByDefault(Return("bar"));
  ON_CALL(bar, getType()).WillByDefault(Return(mInterface->getOwner()));
  
  mScopes.pushScope();
  mScopes.setVariable(mContext, &foo);
  mScopes.pushScope();
  mScopes.setVariable(mContext, &bar);
  
  EXPECT_CALL(foo, free(_, _, _));
  EXPECT_CALL(bar, free(_, _, _));
  
  mScopes.freeOwnedMemory(mContext, NULL, 0);
}

TEST_F(ScopesTest, hasOwnerVariablesTest) {
  NiceMock<MockOwnerVariable> foo;
  ON_CALL(foo, getName()).WillByDefault(Return("foo"));
  ON_CALL(foo, getType()).WillByDefault(Return(mInterface->getOwner()));
  
  mScopes.pushScope();

  EXPECT_FALSE(mScopes.hasOwnerVariables());
  
  mScopes.setVariable(mContext, &foo);
  mScopes.pushScope();

  EXPECT_TRUE(mScopes.hasOwnerVariables());
}

TEST_F(ScopesTest, variableHidingDeathTest) {
  mScopes.pushScope();
  Value* outerValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  Value* innerValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
  
  LocalPrimitiveVariable* outerVariable =
  new LocalPrimitiveVariable("foo", PrimitiveTypes::INT, outerValue, 1);
  LocalPrimitiveVariable* innerVariable =
  new LocalPrimitiveVariable("foo", PrimitiveTypes::INT, innerValue, 3);
  
  mScopes.setVariable(mContext, outerVariable);
  mScopes.pushScope();
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(mScopes.setVariable(mContext, innerVariable));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Variable 'foo' is already defined on line 1, variable hiding is not allowed\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, referenceMemoryDeallocatedByPassingOwnerReuseReferenceRunTest) {
  runFile("tests/samples/test_reference_memory_deallocated_by_passing_owner_reuse_reference.yz", 5);
}

TEST_F(TestFileRunner, variableHidingRunDeathTest) {
  expectFailCompile("tests/samples/test_variable_hiding.yz",
                    1,
                    "tests/samples/test_variable_hiding.yz\\(11\\): Error: "
                    "Variable 'var' is already defined on line 8, variable hiding is not allowed.");
}

TEST_F(TestFileRunner, referenceMemoryDeallocatedByPassingOwnerRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_reference_memory_deallocated_by_passing_owner.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CService.capture(tests/samples/test_reference_memory_deallocated_by_passing_owner.yz:16)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_reference_memory_deallocated_by_passing_owner.yz:26)\n"
                               "Details: Object referenced by expression of type systems.vos.wisey.compiler.tests.MData still has 2 active references\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, referenceMemoryDeallocatedBySettingNullOutsideObjectRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_reference_memory_deallocated_by_setting_null_outside_object.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_reference_memory_deallocated_by_setting_null_outside_object.yz:31)\n"
                               "Details: Object referenced by expression of type systems.vos.wisey.compiler.tests.MData still has 1 active reference\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, referenceMemoryDeallocatedByPassingOwnerInsideIfThenElseRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_reference_memory_deallocated_by_passing_owner_inside_if_then_else.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CService.capture(tests/samples/test_reference_memory_deallocated_by_passing_owner_inside_if_then_else.yz:16)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_reference_memory_deallocated_by_passing_owner_inside_if_then_else.yz:27)\n"
                               "Details: Object referenced by expression of type systems.vos.wisey.compiler.tests.MData still has 2 active references\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, referenceCountExceptionRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_reference_count_exception.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CService.clear(tests/samples/test_reference_count_exception.yz:22)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_reference_count_exception.yz:31)\n"
                               "Details: Object referenced by expression of type systems.vos.wisey.compiler.tests.MData still has 1 active reference\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, throwRceFromFreeOwnedMemoryRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_throw_rce_from_free_owned_memory.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_throw_rce_from_free_owned_memory.yz:18)\n"
                               "Details: Object referenced by expression of type systems.vos.wisey.compiler.tests.MCar still has 1 active reference\n"
                               "Was thrown from memory clean up after wisey.lang.MNullPointerException\n"
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_throw_rce_from_free_owned_memory.yz:18)\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, nullPointerExceptionRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_null_pointer_exception.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CService.getValue(tests/samples/test_null_pointer_exception.yz:25)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_null_pointer_exception.yz:36)\n"
                               "Main thread ended without a result\n");
}
