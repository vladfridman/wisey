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
#include "wisey/ExternalStaticMethodDefinition.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"

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
  mFloatTypeSpecifier(PrimitiveTypes::FLOAT_TYPE->newTypeSpecifier()),
  mIntTypeSpecifier(PrimitiveTypes::INT_TYPE->newTypeSpecifier()),
  mIntArgumentIdentifier(new Identifier("intargument")),
  mFloatArgumentIdentifier(new Identifier("floatargument")),
  mIntArgument(VariableDeclaration::create(mIntTypeSpecifier, mIntArgumentIdentifier, 0)),
  mFloatArgument(VariableDeclaration::create(mFloatTypeSpecifier, mFloatArgumentIdentifier, 0)) {
    TestPrefix::generateIR(mContext);
  }
};

TEST_F(ExternalStaticMethodDefinitionTest, methodDescriptorExtractTest) {
  mArguments.push_back(mIntArgument);
  mArguments.push_back(mFloatArgument);
  vector<IModelTypeSpecifier*> thrownExceptions;
  const PrimitiveTypeSpecifier* floatTypeSpecifier = PrimitiveTypes::FLOAT_TYPE->newTypeSpecifier();
  ExternalStaticMethodDefinition methodDefinition(floatTypeSpecifier,
                                                  "foo",
                                                  mArguments,
                                                  thrownExceptions,
                                                  0);
  IMethod* method = methodDefinition.define(mContext, NULL);
  vector<MethodArgument*> arguments = method->getArguments();
  
  EXPECT_FALSE(methodDefinition.isConstant());
  EXPECT_FALSE(methodDefinition.isField());
  EXPECT_FALSE(methodDefinition.isMethod());
  EXPECT_TRUE(methodDefinition.isStaticMethod());
  EXPECT_FALSE(methodDefinition.isMethodSignature());
  EXPECT_FALSE(methodDefinition.isLLVMFunction());

  EXPECT_STREQ(method->getName().c_str(), "foo");
  EXPECT_TRUE(method->isStatic());
  EXPECT_EQ(method->getReturnType(), PrimitiveTypes::FLOAT_TYPE);
  EXPECT_EQ(arguments.size(), 2ul);
  EXPECT_EQ(arguments.at(0)->getName(), "intargument");
  EXPECT_EQ(arguments.at(0)->getType(), PrimitiveTypes::INT_TYPE);
  EXPECT_EQ(arguments.at(1)->getName(), "floatargument");
  EXPECT_EQ(arguments.at(1)->getType(), PrimitiveTypes::FLOAT_TYPE);
}

TEST_F(TestFileRunner, externalStaticMethodDefinitionsRunTest) {
  compileFile("tests/samples/test_extenal_static_method_declarations.yz");
}
