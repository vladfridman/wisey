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

#include "MockStatement.hpp"
#include "TestFileSampleRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/AccessLevel.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/FieldDeclaration.hpp"
#include "wisey/FloatConstant.hpp"
#include "wisey/Interface.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/IModelTypeSpecifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodDeclaration.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/ProgramPrefix.hpp"
#include "wisey/ReturnStatement.hpp"
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
  vector<IObjectElementDeclaration*> mElementDeclarations;
  vector<IInterfaceTypeSpecifier*> mInterfaces;
  string mPackage = "systems.vos.wisey.compiler.tests";
  
  ThreadDefinitionTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mMockStatement(new NiceMock<MockStatement>()) {
    TestPrefix::generateIR(mContext);
    
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    
    MethodDeclaration *methodDeclaration;
    Block* block = new Block();
    
    block->getStatements().push_back(mMockStatement);
    block->getStatements().push_back(new ReturnStatement(new FloatConstant(0.5), 0));
    CompoundStatement* compoundStatement = new CompoundStatement(block, 0);
    PrimitiveTypeSpecifier* intTypeSpecifier =
    new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
    PrimitiveTypeSpecifier* floatTypeSpecifier =
    new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
    Identifier* intArgumentIdentifier = new Identifier("intargument");
    VariableDeclaration* intArgument =
    VariableDeclaration::create(intTypeSpecifier, intArgumentIdentifier, 0);
    VariableList methodArguments;
    methodArguments.push_back(intArgument);
    vector<IModelTypeSpecifier*> thrownExceptions;
    methodDeclaration = new MethodDeclaration(AccessLevel::PUBLIC_ACCESS,
                                              floatTypeSpecifier,
                                              "foo",
                                              methodArguments,
                                              thrownExceptions,
                                              compoundStatement,
                                              0);
    
    PrimitiveTypeSpecifier* longType = new PrimitiveTypeSpecifier(PrimitiveTypes::LONG_TYPE);
    PrimitiveTypeSpecifier* floatType = new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
    InjectionArgumentList arguments;
    FieldDeclaration* field1 = new FieldDeclaration(FIXED_FIELD, longType, "field1", arguments);
    FieldDeclaration* field2 = new FieldDeclaration(FIXED_FIELD, floatType, "field2", arguments);
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
  PrimitiveTypeSpecifier* voidTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::VOID_TYPE);
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
  PrimitiveTypeSpecifier* voidTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::VOID_TYPE);
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
  PrimitiveTypeSpecifier* voidTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::VOID_TYPE);
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
  EXPECT_EQ(structType->getNumElements(), 3u);
  // reference counter
  EXPECT_EQ(structType->getElementType(0), Type::getInt64Ty(mLLVMContext));
  EXPECT_EQ(structType->getElementType(1), Type::getInt64Ty(mLLVMContext));
  EXPECT_EQ(structType->getElementType(2), Type::getFloatTy(mLLVMContext));
  EXPECT_STREQ(thread->getShortName().c_str(), "TWorker");
  EXPECT_STREQ(thread->getTypeName().c_str(), "systems.vos.wisey.compiler.tests.TWorker");
  EXPECT_NE(thread->findMethod("foo"), nullptr);
}

TEST_F(ThreadDefinitionTest, threadWithStateFieldDeathTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ThreadTypeSpecifierFull* typeSpecifier = new ThreadTypeSpecifierFull(packageExpression,
                                                                       "TWorker");
  InjectionArgumentList arguments;
  PrimitiveTypeSpecifier* intType = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
  FieldDeclaration* field = new FieldDeclaration(STATE_FIELD, intType, "field3", arguments);
  mElementDeclarations.clear();
  mElementDeclarations.push_back(field);
  vector<IObjectDefinition*> innerObjectDefinitions;
  PrimitiveTypeSpecifier* voidTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::VOID_TYPE);
  ThreadDefinition threadDefinition(AccessLevel::PUBLIC_ACCESS,
                                    typeSpecifier,
                                    voidTypeSpecifier,
                                    mElementDeclarations,
                                    mInterfaces,
                                    innerObjectDefinitions);
  threadDefinition.prototypeObject(mContext);
  
  EXPECT_EXIT(threadDefinition.prototypeMethods(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Threads can only have fixed fields");
}

TEST_F(ThreadDefinitionTest, fieldsDeclaredAfterMethodsDeathTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ThreadTypeSpecifierFull* typeSpecifier = new ThreadTypeSpecifierFull(packageExpression,
                                                                       "TWorker");
  InjectionArgumentList arguments;
  PrimitiveTypeSpecifier* intType = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
  FieldDeclaration* field = new FieldDeclaration(FIXED_FIELD, intType, "field3", arguments);
  mElementDeclarations.push_back(field);
  vector<IObjectDefinition*> innerObjectDefinitions;
  PrimitiveTypeSpecifier* voidTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::VOID_TYPE);
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

TEST_F(TestFileSampleRunner, threadDefinitionRunTest) {
  compileFile("tests/samples/test_thread_definition.yz");
}

