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
#include "wisey/ConstantDeclaration.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/FixedFieldDeclaration.hpp"
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
#include "wisey/StateFieldDeclaration.hpp"
#include "wisey/ThreadDefinition.hpp"
#include "wisey/ThreadInfrastructure.hpp"

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
    StateFieldDeclaration* field1 = new StateFieldDeclaration(longType, "field1");
    StateFieldDeclaration* field2 = new StateFieldDeclaration(floatType, "field2");
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
  const PrimitiveTypeSpecifier* voidTypeSpecifier = PrimitiveTypes::VOID_TYPE->newTypeSpecifier();
  vector<IObjectDefinition*> innerObjectDefinitions;
  ThreadDefinition threadDefinition(AccessLevel::PUBLIC_ACCESS,
                                    typeSpecifier,
                                    voidTypeSpecifier,
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
  const PrimitiveTypeSpecifier* voidTypeSpecifier = PrimitiveTypes::VOID_TYPE->newTypeSpecifier();
  vector<IObjectDefinition*> innerObjectDefinitions;
  ThreadDefinition threadDefinition(AccessLevel::PUBLIC_ACCESS,
                                    typeSpecifier,
                                    voidTypeSpecifier,
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
  const PrimitiveTypeSpecifier* voidTypeSpecifier = PrimitiveTypes::VOID_TYPE->newTypeSpecifier();
  vector<IObjectDefinition*> innerObjectDefinitions;
  ThreadDefinition threadDefinition(AccessLevel::PUBLIC_ACCESS,
                                    typeSpecifier,
                                    voidTypeSpecifier,
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
  EXPECT_EQ(structType->getNumElements(), 5u);
  NativeType* nativeThreadType = ThreadInfrastructure::createNativeThreadType(mContext);
  EXPECT_EQ(structType->getElementType(0), Type::getInt64Ty(mLLVMContext));
  EXPECT_EQ(structType->getElementType(1), nativeThreadType->getLLVMType(mContext));
  EXPECT_EQ(structType->getElementType(2), Type::getInt32Ty(mLLVMContext));
  EXPECT_EQ(structType->getElementType(3), Type::getInt64Ty(mLLVMContext));
  EXPECT_EQ(structType->getElementType(4), Type::getFloatTy(mLLVMContext));
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
  FixedFieldDeclaration* field = new FixedFieldDeclaration(intType, "field3");
  mElementDeclarations.clear();
  mElementDeclarations.push_back(field);
  vector<IObjectDefinition*> innerObjectDefinitions;
  const PrimitiveTypeSpecifier* voidTypeSpecifier = PrimitiveTypes::VOID_TYPE->newTypeSpecifier();
  ThreadDefinition threadDefinition(AccessLevel::PUBLIC_ACCESS,
                                    typeSpecifier,
                                    voidTypeSpecifier,
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
  StateFieldDeclaration* field = new StateFieldDeclaration(intType, "field3");
  mElementDeclarations.push_back(field);
  vector<IObjectDefinition*> innerObjectDefinitions;
  const PrimitiveTypeSpecifier* voidTypeSpecifier = PrimitiveTypes::VOID_TYPE->newTypeSpecifier();
  ThreadDefinition threadDefinition(AccessLevel::PUBLIC_ACCESS,
                                    typeSpecifier,
                                    voidTypeSpecifier,
                                    mElementDeclarations,
                                    mInterfaces,
                                    innerObjectDefinitions);
  threadDefinition.prototypeObject(mContext);
  
  EXPECT_EXIT(threadDefinition.prototypeMethods(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Fields should be declared before methods");
}

TEST_F(ThreadDefinitionTest, addThreadObjectElementsTest) {
  const PrimitiveTypeSpecifier* intSpecifier = PrimitiveTypes::INT_TYPE->newTypeSpecifier();
  NiceMock<MockExpression>* mockExpression = new NiceMock<MockExpression>();
  ConstantDeclaration* constantDeclaration = new ConstantDeclaration(PUBLIC_ACCESS,
                                                                     intSpecifier,
                                                                     "MY_CONSTANT",
                                                                     mockExpression);
  const PrimitiveTypeSpecifier* voidSpecifier = PrimitiveTypes::VOID_TYPE->newTypeSpecifier();
  VariableList arguments;
  vector<IModelTypeSpecifier*> exceptions;
  Block* block = new Block();
  CompoundStatement* compoundStatement = new CompoundStatement(block, 0);
  MethodDefinition* methodDeclaration = new MethodDefinition(PUBLIC_ACCESS,
                                                               voidSpecifier,
                                                               "work",
                                                               arguments,
                                                               exceptions,
                                                               compoundStatement,
                                                               0);

  Thread* thread = Thread::newThread(PUBLIC_ACCESS, "TWorker", NULL);
  vector<IObjectElementDefinition*> objectElements;
  objectElements.push_back(constantDeclaration);
  objectElements.push_back(methodDeclaration);
  
  vector<IObjectElementDefinition*> newElements =
  ThreadDefinition::addThreadObjectElements(mContext, objectElements, thread);
  
  EXPECT_EQ(6u, newElements.size());
  EXPECT_TRUE(newElements.at(0)->isConstant());
  EXPECT_TRUE(newElements.at(1)->isField());
  EXPECT_TRUE(newElements.at(2)->isField());
  EXPECT_TRUE(newElements.at(3)->isMethod());
  EXPECT_TRUE(newElements.at(4)->isMethod());
  EXPECT_TRUE(newElements.at(5)->isMethod());
}

TEST_F(TestFileSampleRunner, threadDefinitionRunTest) {
  compileFile("tests/samples/test_thread_definition.yz");
}

TEST_F(TestFileSampleRunner, threadRunRunTest) {
  runFileCheckOutput("tests/samples/test_thread_run.yz",
                     "Starting worker thread\n"
                     "Getting call stack!\n"
                     "This is worker thread!\n"
                     "Worker thread started\n"
                     "Worker thread result: 5\n",
                     "");
}
