//
//  TestExternalMethodDefinition.cpp
//  runtests
//
//  Created by Vladimir Fridman on 9/29/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ExternalMethodDefinition}
//

#include <gtest/gtest.h>

#include "TestPrefix.hpp"
#include "ExternalMethodDefinition.hpp"
#include "IRGenerationContext.hpp"
#include "IModelTypeSpecifier.hpp"
#include "PrimitiveTypes.hpp"
#include "PrimitiveTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct ExternalMethodDefinitionTest : Test {
  IRGenerationContext mContext;
  const PrimitiveTypeSpecifier* mFloatTypeSpecifier;
  const PrimitiveTypeSpecifier* mIntTypeSpecifier;
  Identifier* mIntArgumentIdentifier;
  Identifier* mFloatArgumentIdentifier;
  VariableDeclaration* mIntArgument;
  VariableDeclaration* mFloatArgument;
  VariableList mArguments;
  
  ExternalMethodDefinitionTest() :
  mFloatTypeSpecifier(PrimitiveTypes::FLOAT->newTypeSpecifier(0)),
  mIntTypeSpecifier(PrimitiveTypes::INT->newTypeSpecifier(0)),
  mIntArgumentIdentifier(new Identifier("intargument", 0)),
  mFloatArgumentIdentifier(new Identifier("floatargument", 0)),
  mIntArgument(VariableDeclaration::create(mIntTypeSpecifier, mIntArgumentIdentifier, 0)),
  mFloatArgument(VariableDeclaration::create(mFloatTypeSpecifier, mFloatArgumentIdentifier, 0)) {
    TestPrefix::generateIR(mContext);
  }
};

TEST_F(ExternalMethodDefinitionTest, methodDescriptorExtractTest) {
  mArguments.push_back(mIntArgument);
  mArguments.push_back(mFloatArgument);
  vector<IModelTypeSpecifier*> thrownExceptions;
  const PrimitiveTypeSpecifier* floatTypeSpecifier =
  PrimitiveTypes::FLOAT->newTypeSpecifier(0);
  ExternalMethodDefinition methodDefinition(floatTypeSpecifier,
                                            "foo",
                                            mArguments,
                                            thrownExceptions,
                                            new MethodQualifiers(0),
                                            0);
  IMethod* method = methodDefinition.define(mContext, NULL);
  vector<const wisey::Argument*> arguments = method->getArguments();

  EXPECT_FALSE(methodDefinition.isConstant());
  EXPECT_FALSE(methodDefinition.isField());
  EXPECT_TRUE(methodDefinition.isMethod());
  EXPECT_FALSE(methodDefinition.isStaticMethod());
  EXPECT_FALSE(methodDefinition.isMethodSignature());
  EXPECT_FALSE(methodDefinition.isLLVMFunction());

  EXPECT_STREQ(method->getName().c_str(), "foo");
  EXPECT_FALSE(method->isStatic());
  EXPECT_EQ(method->getReturnType(), PrimitiveTypes::FLOAT);
  EXPECT_EQ(arguments.size(), 2ul);
  EXPECT_EQ(arguments.at(0)->getName(), "intargument");
  EXPECT_EQ(arguments.at(0)->getType(), PrimitiveTypes::INT);
  EXPECT_EQ(arguments.at(1)->getName(), "floatargument");
  EXPECT_EQ(arguments.at(1)->getType(), PrimitiveTypes::FLOAT);
}
