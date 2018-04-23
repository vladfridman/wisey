//
//  TestThreadDefinition.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/2/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ThreadDefinition}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "MockStatement.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/AccessLevel.hpp"
#include "wisey/Argument.hpp"
#include "wisey/ConstantDefinition.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/FixedFieldDefinition.hpp"
#include "wisey/FloatConstant.hpp"
#include "wisey/Interface.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/IModelTypeSpecifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/MethodDefinition.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/ReturnStatement.hpp"
#include "wisey/StateFieldDefinition.hpp"
#include "wisey/ThreadDefinition.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ThreadDefinitionTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockStatement>* mMockStatement;
  vector<IObjectElementDefinition*> mElementDeclarations;
  vector<IInterfaceTypeSpecifier*> mInterfaces;
  string mPackage = "systems.vos.wisey.compiler.tests";
  
  ThreadDefinitionTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mMockStatement(new NiceMock<MockStatement>()) {
    TestPrefix::generateIR(mContext);
    
    MethodDefinition *methodDeclaration;
    Block* block = new Block();
    
    block->getStatements().push_back(mMockStatement);
    block->getStatements().push_back(new ReturnStatement(new FloatConstant(0.5, 0), 0));
    CompoundStatement* compoundStatement = new CompoundStatement(block, 0);
    const PrimitiveTypeSpecifier* intTypeSpecifier = PrimitiveTypes::INT_TYPE->newTypeSpecifier(0);
    const PrimitiveTypeSpecifier* floatTypeSpecifier =
    PrimitiveTypes::FLOAT_TYPE->newTypeSpecifier(0);
    Identifier* intArgumentIdentifier = new Identifier("intargument", 0);
    VariableDeclaration* intArgument =
    VariableDeclaration::create(intTypeSpecifier, intArgumentIdentifier, 0);
    VariableList methodArguments;
    methodArguments.push_back(intArgument);
    vector<IModelTypeSpecifier*> thrownExceptions;
    MethodQualifiers* methodQualifiers = new MethodQualifiers(0);
    methodQualifiers->getMethodQualifierSet().insert(MethodQualifier::CONCEAL);
    methodDeclaration = new MethodDefinition(AccessLevel::PUBLIC_ACCESS,
                                             floatTypeSpecifier,
                                             "foo",
                                             methodArguments,
                                             thrownExceptions,
                                             methodQualifiers,
                                             compoundStatement,
                                             0);

    const PrimitiveTypeSpecifier* longType = PrimitiveTypes::LONG_TYPE->newTypeSpecifier(0);
    const PrimitiveTypeSpecifier* floatType = PrimitiveTypes::FLOAT_TYPE->newTypeSpecifier(0);
    StateFieldDefinition* field1 = new StateFieldDefinition(longType, "field1", 0);
    StateFieldDefinition* field2 = new StateFieldDefinition(floatType, "field2", 0);
    mElementDeclarations.push_back(field1);
    mElementDeclarations.push_back(field2);
    mElementDeclarations.push_back(methodDeclaration);
  }
  
  ~ThreadDefinitionTest() {
  }
};

TEST_F(ThreadDefinitionTest, threadDefinitionPrototypeObjectTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ThreadTypeSpecifierFull* typeSpecifier = new ThreadTypeSpecifierFull(packageExpression,
                                                                       "TWorker",
                                                                       0);
  vector<IObjectDefinition*> innerObjectDefinitions;
  ThreadDefinition threadDefinition(AccessLevel::PUBLIC_ACCESS,
                                    typeSpecifier,
                                    mElementDeclarations,
                                    mInterfaces,
                                    innerObjectDefinitions,
                                    0);
  
  EXPECT_CALL(*mMockStatement, generateIR(_)).Times(0);
  
  threadDefinition.prototypeObject(mContext);
  
  ASSERT_NE(mContext.getThread("systems.vos.wisey.compiler.tests.TWorker", 0), nullptr);
  
  Thread* thread = mContext.getThread("systems.vos.wisey.compiler.tests.TWorker", 0);
  
  EXPECT_STREQ(thread->getShortName().c_str(), "TWorker");
  EXPECT_STREQ(thread->getTypeName().c_str(), "systems.vos.wisey.compiler.tests.TWorker");
  EXPECT_EQ(thread->findMethod("foo"), nullptr);
}

TEST_F(ThreadDefinitionTest, threadDefinitionPrototypeMethodsTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ThreadTypeSpecifierFull* typeSpecifier =
  new ThreadTypeSpecifierFull(packageExpression, "TWorker", 0);
  vector<IObjectDefinition*> innerObjectDefinitions;
  ThreadDefinition threadDefinition(AccessLevel::PUBLIC_ACCESS,
                                    typeSpecifier,
                                    mElementDeclarations,
                                    mInterfaces,
                                    innerObjectDefinitions,
                                    0);
  
  EXPECT_CALL(*mMockStatement, generateIR(_)).Times(0);
  
  threadDefinition.prototypeObject(mContext);
  threadDefinition.prototypeMethods(mContext);
  
  Thread* thread = mContext.getThread("systems.vos.wisey.compiler.tests.TWorker", 0);
  EXPECT_NE(thread->findMethod("foo"), nullptr);
}

TEST_F(ThreadDefinitionTest, threadDefinitionGenerateIRTest) {
  PackageType* packageType = new PackageType(Names::getThreadsPackageName());
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  InterfaceTypeSpecifierFull* interfaceSpecifier =
  new InterfaceTypeSpecifierFull(packageExpression, Names::getThreadInterfaceName(), 0);
  mInterfaces.push_back(interfaceSpecifier);
  packageType = new PackageType(mPackage);
  packageExpression = new FakeExpression(NULL, packageType);
  ThreadTypeSpecifierFull* typeSpecifier =
  new ThreadTypeSpecifierFull(packageExpression, "TWorker", 0);
  vector<IObjectDefinition*> innerObjectDefinitions;
  ThreadDefinition threadDefinition(AccessLevel::PUBLIC_ACCESS,
                                    typeSpecifier,
                                    mElementDeclarations,
                                    mInterfaces,
                                    innerObjectDefinitions,
                                    0);
  
  EXPECT_CALL(*mMockStatement, generateIR(_));
  
  threadDefinition.prototypeObject(mContext);
  threadDefinition.prototypeMethods(mContext);
  threadDefinition.generateIR(mContext);
  
  ASSERT_NE(mContext.getThread("systems.vos.wisey.compiler.tests.TWorker", 0), nullptr);
  
  Thread* thread = mContext.getThread("systems.vos.wisey.compiler.tests.TWorker", 0);
  StructType* structType = (StructType*) thread->getLLVMType(mContext)->getPointerElementType();
  
  Type* threadInterfaceType =
  mContext.getInterface(Names::getThreadInterfaceFullName(), 0)->getLLVMType(mContext)
  ->getPointerElementType();
  ASSERT_NE(structType, nullptr);
  EXPECT_EQ(structType->getNumElements(), 3u);
  EXPECT_EQ(threadInterfaceType, structType->getElementType(0));
  EXPECT_EQ(Type::getInt64Ty(mLLVMContext), structType->getElementType(1));
  EXPECT_EQ(Type::getFloatTy(mLLVMContext), structType->getElementType(2));
  EXPECT_STREQ(thread->getShortName().c_str(), "TWorker");
  EXPECT_STREQ(thread->getTypeName().c_str(), "systems.vos.wisey.compiler.tests.TWorker");
  EXPECT_NE(thread->findMethod("foo"), nullptr);
}

TEST_F(ThreadDefinitionTest, threadWithFixedFieldDeathTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ThreadTypeSpecifierFull* typeSpecifier =
  new ThreadTypeSpecifierFull(packageExpression, "TWorker", 0);
  const PrimitiveTypeSpecifier* intType = PrimitiveTypes::INT_TYPE->newTypeSpecifier(0);
  FixedFieldDefinition* field = new FixedFieldDefinition(intType, "field3", 0);
  mElementDeclarations.clear();
  mElementDeclarations.push_back(field);
  vector<IObjectDefinition*> innerObjectDefinitions;
  ThreadDefinition threadDefinition(AccessLevel::PUBLIC_ACCESS,
                                    typeSpecifier,
                                    mElementDeclarations,
                                    mInterfaces,
                                    innerObjectDefinitions,
                                    0);
  threadDefinition.prototypeObject(mContext);
  
  EXPECT_EXIT(threadDefinition.prototypeMethods(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Threads can only have received, injected or state fields");
}

TEST_F(ThreadDefinitionTest, fieldsDeclaredAfterMethodsDeathTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ThreadTypeSpecifierFull* typeSpecifier =
  new ThreadTypeSpecifierFull(packageExpression, "TWorker", 0);
  const PrimitiveTypeSpecifier* intType = PrimitiveTypes::INT_TYPE->newTypeSpecifier(0);
  StateFieldDefinition* field = new StateFieldDefinition(intType, "field3", 0);
  mElementDeclarations.push_back(field);
  vector<IObjectDefinition*> innerObjectDefinitions;
  ThreadDefinition threadDefinition(AccessLevel::PUBLIC_ACCESS,
                                    typeSpecifier,
                                    mElementDeclarations,
                                    mInterfaces,
                                    innerObjectDefinitions,
                                    0);
  threadDefinition.prototypeObject(mContext);
  
  EXPECT_EXIT(threadDefinition.prototypeMethods(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Fields should be declared before methods");
}

TEST_F(TestFileRunner, threadDefinitionRunTest) {
  compileFile("tests/samples/test_thread_definition.yz");
}

TEST_F(TestFileRunner, threadMethodNotConcealedOrRevealedDeathTest) {
  expectFailCompile("tests/samples/test_thread_method_not_concealed_or_revealed.yz",
                    1,
                    "Error: Method 'foo' in object systems.vos.wisey.compiler.tests.TThread must "
                    "either have have conceal or reveal qualifier because the object is a thread");
}

TEST_F(TestFileRunner, threadMethodBothConcealedAndRevealedDeathTest) {
  expectFailCompile("tests/samples/test_thread_method_both_concealed_and_revealed.yz",
                    1,
                    "Error: Method 'exit' in object systems.vos.wisey.compiler.tests.TThread can "
                    "either be marked with a conceal or reveal qualifier but not both");
}

TEST_F(TestFileRunner, threadRunRunTest) {
  runFileCheckOutput("tests/samples/test_thread_run.yz",
                     "Thread started = 0, has finished = 0\n"
                     "Starting worker thread\n"
                     "Worker thread started\n"
                     "Thread started = 1, has finished = 0\n"
                     "This is worker thread!\n"
                     "Worker thread finished with result: 5\n"
                     "Thread started = 1, has finished = 1\n",
                     "");
}

TEST_F(TestFileRunner, threadResetRunTest) {
  runFileCheckOutput("tests/samples/test_thread_reset.yz",
                     "Thread started = 0, has finished = 0\n"
                     "Starting worker thread\n"
                     "Worker thread started\n"
                     "Thread started = 1, has finished = 0\n"
                     "This is worker thread!\n"
                     "Worker thread finished with result: 5\n"
                     "Thread started = 1, has finished = 1\n"
                     "Thread started = 0, has finished = 0\n"
                     "Starting worker thread\n"
                     "Worker thread started\n"
                     "Thread started = 1, has finished = 0\n"
                     "This is worker thread!\n"
                     "Worker thread finished with result: 5\n"
                     "Thread started = 1, has finished = 1\n",
                     "");
}

TEST_F(TestFileRunner, threadProducerRunRunTest) {
  runFileCheckOutput("tests/samples/test_thread_producer_run.yz",
                     "Thread started = 0, has result = 0\n"
                     "Starting producer thread\n"
                     "Producer thread started\n"
                     "Thread started = 1, has result = 0\n"
                     "This is producer thread!\n"
                     "Adding result 0\n"
                     "Adding result 1\n"
                     "Producer thread finished with result: 0\n"
                     "Thread started = 1, has result = 1\n"
                     "Producer thread finished with result: 1\n"
                     "Thread started = 1, has result = 0\n",
                     "");
}

TEST_F(TestFileRunner, threadProducerResetRunTest) {
  runFileCheckOutput("tests/samples/test_thread_producer_reset.yz",
                     "Thread started = 0, has result = 0\n"
                     "Starting producer thread\n"
                     "Producer thread started\n"
                     "Thread started = 1, has result = 0\n"
                     "This is producer thread!\n"
                     "Adding result 0\n"
                     "Adding result 1\n"
                     "Producer thread finished with result: 0\n"
                     "Thread started = 1, has result = 1\n"
                     "Producer thread finished with result: 1\n"
                     "Thread started = 1, has result = 0\n"
                     "Thread started = 0, has result = 0\n"
                     "Starting producer thread\n"
                     "Producer thread started\n"
                     "Thread started = 1, has result = 0\n"
                     "This is producer thread!\n"
                     "Adding result 0\n"
                     "Adding result 1\n"
                     "Producer thread finished with result: 0\n"
                     "Thread started = 1, has result = 1\n"
                     "Producer thread finished with result: 1\n"
                     "Thread started = 1, has result = 0\n",
                     "");
}

TEST_F(TestFileRunner, threadCancelRunTest) {
  runFileCheckOutput("tests/samples/test_thread_cancel.yz",
                     "Thread started = 0, has result = 0, was cancelled = 0\n"
                     "Starting worker thread\n"
                     "Worker thread started\n"
                     "Thread started = 1, has result = 0, was cancelled = 0\n"
                     "Worker thread cancelled\n"
                     "Thread started = 1, has result = 0, was cancelled = 1\n"
                     "Thread result is null\n",
                     "");
}

TEST_F(TestFileRunner, threadThrowConcealedCallExceptionDeathRunTest) {
  compileAndRunFileCheckOutput("tests/samples/test_thread_throw_concealed_call_exception.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.threads.MThreadConcealedMethodException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_thread_throw_concealed_call_exception.yz:19)\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, threadThrowRevealedCallExceptionDeathRunTest) {
  compileAndRunFileCheckOutput("tests/samples/test_thread_throw_revealed_call_exception.yz",
                               0,
                               "",
                               "Unhandled exception wisey.lang.threads.MThreadRevealedMethodException\n"
                               "  at systems.vos.wisey.compiler.tests.MWorker.work(tests/samples/test_thread_throw_revealed_call_exception.yz:10)\n");
}
