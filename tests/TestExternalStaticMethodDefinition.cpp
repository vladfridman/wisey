//
//  TestExternalStaticMethodDefinition.cpp
//  runtests
//
//  Created by Vladimir Fridman on 9/29/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ExternalStaticMethodDefinition}
//

#include <gtest/gtest.h>

#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "ExternalStaticMethodDefinition.hpp"
#include "IRGenerationContext.hpp"
#include "PrimitiveTypes.hpp"
#include "PrimitiveTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct ExternalStaticMethodDefinitionTest : Test {
  IRGenerationContext mContext;
  const PrimitiveTypeSpecifier* mFloatTypeSpecifier;
  const PrimitiveTypeSpecifier* mIntTypeSpecifier;
  Identifier* mIntArgumentIdentifier;
  Identifier* mFloatArgumentIdentifier;
  VariableDeclaration* mIntArgument;
  VariableDeclaration* mFloatArgument;
  VariableList mArguments;
  
  ExternalStaticMethodDefinitionTest() :
  mFloatTypeSpecifier(PrimitiveTypes::FLOAT->newTypeSpecifier(0)),
  mIntTypeSpecifier(PrimitiveTypes::INT->newTypeSpecifier(0)),
  mIntArgumentIdentifier(new Identifier("intargument", 0)),
  mFloatArgumentIdentifier(new Identifier("floatargument", 0)),
  mIntArgument(VariableDeclaration::create(mIntTypeSpecifier, mIntArgumentIdentifier, 0)),
  mFloatArgument(VariableDeclaration::create(mFloatTypeSpecifier, mFloatArgumentIdentifier, 0)) {
    TestPrefix::generateIR(mContext);
  }
};

TEST_F(ExternalStaticMethodDefinitionTest, methodDescriptorExtractTest) {
  mArguments.push_back(mIntArgument);
  mArguments.push_back(mFloatArgument);
  vector<IModelTypeSpecifier*> thrownExceptions;
  const PrimitiveTypeSpecifier* floatTypeSpecifier =
  PrimitiveTypes::FLOAT->newTypeSpecifier(0);
  ExternalStaticMethodDefinition methodDefinition(floatTypeSpecifier,
                                                  "foo",
                                                  mArguments,
                                                  thrownExceptions,
                                                  0);
  IMethod* method = methodDefinition.define(mContext, NULL);
  vector<const wisey::Argument*> arguments = method->getArguments();
  
  EXPECT_FALSE(methodDefinition.isConstant());
  EXPECT_FALSE(methodDefinition.isField());
  EXPECT_FALSE(methodDefinition.isMethod());
  EXPECT_TRUE(methodDefinition.isStaticMethod());
  EXPECT_FALSE(methodDefinition.isMethodSignature());
  EXPECT_FALSE(methodDefinition.isLLVMFunction());

  EXPECT_STREQ(method->getName().c_str(), "foo");
  EXPECT_TRUE(method->isStatic());
  EXPECT_EQ(method->getReturnType(), PrimitiveTypes::FLOAT);
  EXPECT_EQ(arguments.size(), 2ul);
  EXPECT_EQ(arguments.at(0)->getName(), "intargument");
  EXPECT_EQ(arguments.at(0)->getType(), PrimitiveTypes::INT);
  EXPECT_EQ(arguments.at(1)->getName(), "floatargument");
  EXPECT_EQ(arguments.at(1)->getType(), PrimitiveTypes::FLOAT);
}

TEST_F(TestFileRunner, externalStaticMethodDefinitionsRunTest) {
  compileFile("tests/samples/test_extenal_static_method_declarations.yz");
}
