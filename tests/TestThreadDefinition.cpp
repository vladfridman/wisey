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
#include "TestFileSampleRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/AccessLevel.hpp"
#include "wisey/ConstantDefinition.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/FixedFieldDefinition.hpp"
#include "wisey/FloatConstant.hpp"
#include "wisey/Interface.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/IModelTypeSpecifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodDefinition.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/ProgramPrefix.hpp"
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
    
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    
    MethodDefinition *methodDeclaration;
    Block* block = new Block();
    
    block->getStatements().push_back(mMockStatement);
    block->getStatements().push_back(new ReturnStatement(new FloatConstant(0.5), 0));
    CompoundStatement* compoundStatement = new CompoundStatement(block, 0);
    const PrimitiveTypeSpecifier* intTypeSpecifier = PrimitiveTypes::INT_TYPE->newTypeSpecifier();
    const PrimitiveTypeSpecifier* floatTypeSpecifier =
    PrimitiveTypes::FLOAT_TYPE->newTypeSpecifier();
    Identifier* intArgumentIdentifier = new Identifier("intargument");
    VariableDeclaration* intArgument =
    VariableDeclaration::create(intTypeSpecifier, intArgumentIdentifier, 0);
    VariableList methodArguments;
    methodArguments.push_back(intArgument);
    vector<IModelTypeSpecifier*> thrownExceptions;
    methodDeclaration = new MethodDefinition(AccessLevel::PUBLIC_ACCESS,
                                              floatTypeSpecifier,
                                              "foo",
                                              methodArguments,
                                              thrownExceptions,
                                              compoundStatement,
                                              0);
    
    const PrimitiveTypeSpecifier* longType = PrimitiveTypes::LONG_TYPE->newTypeSpecifier();
    const PrimitiveTypeSpecifier* floatType = PrimitiveTypes::FLOAT_TYPE->newTypeSpecifier();
    StateFieldDefinition* field1 = new StateFieldDefinition(longType, "field1");
    StateFieldDefinition* field2 = new StateFieldDefinition(floatType, "field2");
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
                                                                       "TWorker");
  vector<IObjectDefinition*> innerObjectDefinitions;
  ThreadDefinition threadDefinition(AccessLevel::PUBLIC_ACCESS,
                                    typeSpecifier,
                                    mElementDeclarations,
                                    mInterfaces,
                                    innerObjectDefinitions);
  
  EXPECT_CALL(*mMockStatement, generateIR(_)).Times(0);
  
  threadDefinition.prototypeObject(mContext);
  
  ASSERT_NE(mContext.getThread("systems.vos.wisey.compiler.tests.TWorker"), nullptr);
  
  Thread* thread = mContext.getThread("systems.vos.wisey.compiler.tests.TWorker");
  
  EXPECT_STREQ(thread->getShortName().c_str(), "TWorker");
  EXPECT_STREQ(thread->getTypeName().c_str(), "systems.vos.wisey.compiler.tests.TWorker");
  EXPECT_EQ(thread->findMethod("foo"), nullptr);
}

TEST_F(ThreadDefinitionTest, threadDefinitionPrototypeMethodsTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ThreadTypeSpecifierFull* typeSpecifier = new ThreadTypeSpecifierFull(packageExpression,
                                                                       "TWorker");
  vector<IObjectDefinition*> innerObjectDefinitions;
  ThreadDefinition threadDefinition(AccessLevel::PUBLIC_ACCESS,
                                    typeSpecifier,
                                    mElementDeclarations,
                                    mInterfaces,
                                    innerObjectDefinitions);
  
  EXPECT_CALL(*mMockStatement, generateIR(_)).Times(0);
  
  threadDefinition.prototypeObject(mContext);
  threadDefinition.prototypeMethods(mContext);
  
  Thread* thread = mContext.getThread("systems.vos.wisey.compiler.tests.TWorker");
  EXPECT_NE(thread->findMethod("foo"), nullptr);
}

TEST_F(ThreadDefinitionTest, threadDefinitionGenerateIRTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ThreadTypeSpecifierFull* typeSpecifier = new ThreadTypeSpecifierFull(packageExpression,
                                                                       "TWorker");
  vector<IObjectDefinition*> innerObjectDefinitions;
  ThreadDefinition threadDefinition(AccessLevel::PUBLIC_ACCESS,
                                    typeSpecifier,
                                    mElementDeclarations,
                                    mInterfaces,
                                    innerObjectDefinitions);
  
  EXPECT_CALL(*mMockStatement, generateIR(_));
  
  threadDefinition.prototypeObject(mContext);
  threadDefinition.prototypeMethods(mContext);
  threadDefinition.generateIR(mContext);
  
  ASSERT_NE(mContext.getThread("systems.vos.wisey.compiler.tests.TWorker"), nullptr);
  
  Thread* thread = mContext.getThread("systems.vos.wisey.compiler.tests.TWorker");
  StructType* structType = (StructType*) thread->getLLVMType(mContext)->getPointerElementType();
  
  ASSERT_NE(structType, nullptr);
  EXPECT_EQ(structType->getNumElements(), 3u);
  EXPECT_EQ(structType->getElementType(0), Type::getInt64Ty(mLLVMContext));
  EXPECT_EQ(structType->getElementType(1), Type::getInt64Ty(mLLVMContext));
  EXPECT_EQ(structType->getElementType(2), Type::getFloatTy(mLLVMContext));
  EXPECT_STREQ(thread->getShortName().c_str(), "TWorker");
  EXPECT_STREQ(thread->getTypeName().c_str(), "systems.vos.wisey.compiler.tests.TWorker");
  EXPECT_NE(thread->findMethod("foo"), nullptr);
}

TEST_F(ThreadDefinitionTest, threadWithFixedFieldDeathTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ThreadTypeSpecifierFull* typeSpecifier = new ThreadTypeSpecifierFull(packageExpression,
                                                                       "TWorker");
  const PrimitiveTypeSpecifier* intType = PrimitiveTypes::INT_TYPE->newTypeSpecifier();
  FixedFieldDefinition* field = new FixedFieldDefinition(intType, "field3");
  mElementDeclarations.clear();
  mElementDeclarations.push_back(field);
  vector<IObjectDefinition*> innerObjectDefinitions;
  ThreadDefinition threadDefinition(AccessLevel::PUBLIC_ACCESS,
                                    typeSpecifier,
                                    mElementDeclarations,
                                    mInterfaces,
                                    innerObjectDefinitions);
  threadDefinition.prototypeObject(mContext);
  
  EXPECT_EXIT(threadDefinition.prototypeMethods(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Threads can only have received, injected or state fields");
}

TEST_F(ThreadDefinitionTest, fieldsDeclaredAfterMethodsDeathTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ThreadTypeSpecifierFull* typeSpecifier = new ThreadTypeSpecifierFull(packageExpression,
                                                                       "TWorker");
  const PrimitiveTypeSpecifier* intType = PrimitiveTypes::INT_TYPE->newTypeSpecifier();
  StateFieldDefinition* field = new StateFieldDefinition(intType, "field3");
  mElementDeclarations.push_back(field);
  vector<IObjectDefinition*> innerObjectDefinitions;
  ThreadDefinition threadDefinition(AccessLevel::PUBLIC_ACCESS,
                                    typeSpecifier,
                                    mElementDeclarations,
                                    mInterfaces,
                                    innerObjectDefinitions);
  threadDefinition.prototypeObject(mContext);
  
  EXPECT_EXIT(threadDefinition.prototypeMethods(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Fields should be declared before methods");
}

TEST_F(TestFileSampleRunner, threadDefinitionRunTest) {
  compileFile("tests/samples/test_thread_definition.yz");
}

TEST_F(TestFileSampleRunner, threadRunRunTest) {
  runFileCheckOutput("tests/samples/test_thread_run.yz",
                     "Thread started = 0, has result = 0\n"
                     "Starting worker thread\n"
                     "Worker thread started\n"
                     "Thread started = 1, has result = 0\n"
                     "Getting call stack!\n"
                     "This is worker thread!\n"
                     "Worker thread finished with result: 5\n"
                     "Thread started = 1, has result = 1\n",
                     "");
}

TEST_F(TestFileSampleRunner, threadProducerRunRunTest) {
  runFileCheckOutput("tests/samples/test_thread_producer_run.yz",
                     "Thread started = 0, has result = 0\n"
                     "Starting producer thread\n"
                     "Producer thread started\n"
                     "Thread started = 1, has result = 0\n"
                     "Getting call stack!\n"
                     "This is producer thread!\n"
                     "Adding result 0\n"
                     "Adding result 1\n"
                     "Producer thread finished with result: 0\n"
                     "Thread started = 1, has result = 1\n"
                     "Producer thread finished with result: 1\n"
                     "Thread started = 1, has result = 0\n",
                     "");
}

TEST_F(TestFileSampleRunner, threadCancelRunTest) {
  runFileCheckOutput("tests/samples/test_thread_cancel.yz",
                     "Thread started = 0, has result = 0, was cancelled = 0\n"
                     "Starting worker thread\n"
                     "Worker thread started\n"
                     "Thread started = 1, has result = 0, was cancelled = 0\n"
                     "Getting call stack!\n"
                     "Worker thread cancelled\n"
                     "Thread started = 1, has result = 0, was cancelled = 1\n"
                     "Thread result is null\n",
                     "");
}
