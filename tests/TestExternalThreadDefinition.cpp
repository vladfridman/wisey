//
//  TestExternalThreadDefinition.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/10/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ExternalThreadDefinition}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/AccessLevel.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/ExternalMethodDeclaration.hpp"
#include "wisey/ExternalThreadDefinition.hpp"
#include "wisey/FloatConstant.hpp"
#include "wisey/Interface.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/IModelTypeSpecifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/ReceivedFieldDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct ExternalThreadDefinitionTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  vector<IObjectElementDefinition*> mElementDeclarations;
  vector<IInterfaceTypeSpecifier*> mInterfaces;
  string mPackage = "systems.vos.wisey.compiler.tests";
  
  ExternalThreadDefinitionTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    ExternalMethodDeclaration* methodDeclaration;
    
    const PrimitiveTypeSpecifier* intTypeSpecifier = PrimitiveTypes::INT_TYPE->newTypeSpecifier();
    const PrimitiveTypeSpecifier* floatTypeSpecifier =
    PrimitiveTypes::FLOAT_TYPE->newTypeSpecifier();
    Identifier* intArgumentIdentifier = new Identifier("intargument");
    VariableDeclaration* intArgument =
    VariableDeclaration::create(intTypeSpecifier, intArgumentIdentifier, 0);
    VariableList methodArguments;
    methodArguments.push_back(intArgument);
    vector<IModelTypeSpecifier*> thrownExceptions;
    methodDeclaration = new ExternalMethodDeclaration(floatTypeSpecifier,
                                                      "foo",
                                                      methodArguments,
                                                      thrownExceptions);
    
    const PrimitiveTypeSpecifier* longType = PrimitiveTypes::LONG_TYPE->newTypeSpecifier();
    const PrimitiveTypeSpecifier* floatType = PrimitiveTypes::FLOAT_TYPE->newTypeSpecifier();
    InjectionArgumentList arguments;
    ReceivedFieldDeclaration* field1 = new ReceivedFieldDeclaration(longType, "mField1");
    ReceivedFieldDeclaration* field2 = new ReceivedFieldDeclaration(floatType, "mField2");
    mElementDeclarations.push_back(field1);
    mElementDeclarations.push_back(field2);
    mElementDeclarations.push_back(methodDeclaration);
  }
  
  ~ExternalThreadDefinitionTest() {
  }
};

TEST_F(ExternalThreadDefinitionTest, prototypeObjectTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ThreadTypeSpecifierFull* typeSpecifier = new ThreadTypeSpecifierFull(packageExpression,
                                                                       "TWorker");
  vector<IObjectDefinition*> innerObjectDefinitions;
  ExternalThreadDefinition threadDefinition(typeSpecifier,
                                            mElementDeclarations,
                                            mInterfaces,
                                            innerObjectDefinitions);
  
  threadDefinition.prototypeObject(mContext);
  
  ASSERT_NE(mContext.getThread("systems.vos.wisey.compiler.tests.TWorker"), nullptr);
  
  Thread* thread = mContext.getThread("systems.vos.wisey.compiler.tests.TWorker");
  
  EXPECT_STREQ(thread->getShortName().c_str(), "TWorker");
  EXPECT_STREQ(thread->getTypeName().c_str(), "systems.vos.wisey.compiler.tests.TWorker");
  EXPECT_EQ(thread->findMethod("foo"), nullptr);
}

TEST_F(ExternalThreadDefinitionTest, prototypeMethodsTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ThreadTypeSpecifierFull* typeSpecifier = new ThreadTypeSpecifierFull(packageExpression,
                                                                       "TWorker");
  vector<IObjectDefinition*> innerObjectDefinitions;
  ExternalThreadDefinition threadDefinition(typeSpecifier,
                                            mElementDeclarations,
                                            mInterfaces,
                                            innerObjectDefinitions);
  
  threadDefinition.prototypeObject(mContext);
  threadDefinition.prototypeMethods(mContext);
  
  Thread* thread = mContext.getThread("systems.vos.wisey.compiler.tests.TWorker");
  EXPECT_NE(thread->findMethod("foo"), nullptr);
}

TEST_F(TestFileSampleRunner, ExternalThreadDefinitionsRunTest) {
  compileFile("tests/samples/test_external_thread_definitions.yz");
}
