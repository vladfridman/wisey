//
//  TestExternalMethodDeclaration.cpp
//  runtests
//
//  Created by Vladimir Fridman on 9/29/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ExternalMethodDeclaration}
//

#include <gtest/gtest.h>

#include "TestPrefix.hpp"
#include "wisey/ExternalMethodDeclaration.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IModelTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct ExternalMethodDeclarationTest : Test {
  IRGenerationContext mContext;
  const PrimitiveTypeSpecifier* mFloatTypeSpecifier;
  const PrimitiveTypeSpecifier* mIntTypeSpecifier;
  Identifier* mIntArgumentIdentifier;
  Identifier* mFloatArgumentIdentifier;
  VariableDeclaration* mIntArgument;
  VariableDeclaration* mFloatArgument;
  VariableList mArguments;
  
  ExternalMethodDeclarationTest() :
  mFloatTypeSpecifier(PrimitiveTypes::FLOAT_TYPE->newTypeSpecifier()),
  mIntTypeSpecifier(PrimitiveTypes::INT_TYPE->newTypeSpecifier()),
  mIntArgumentIdentifier(new Identifier("intargument")),
  mFloatArgumentIdentifier(new Identifier("floatargument")),
  mIntArgument(VariableDeclaration::create(mIntTypeSpecifier, mIntArgumentIdentifier, 0)),
  mFloatArgument(VariableDeclaration::create(mFloatTypeSpecifier, mFloatArgumentIdentifier, 0)) {
    TestPrefix::generateIR(mContext);
  }
};

TEST_F(ExternalMethodDeclarationTest, methodDescriptorExtractTest) {
  mArguments.push_back(mIntArgument);
  mArguments.push_back(mFloatArgument);
  vector<IModelTypeSpecifier*> thrownExceptions;
  const PrimitiveTypeSpecifier* floatTypeSpecifier = PrimitiveTypes::FLOAT_TYPE->newTypeSpecifier();
  ExternalMethodDeclaration methodDeclaration(floatTypeSpecifier,
                                              "foo",
                                              mArguments,
                                              thrownExceptions);
  IMethod* method = methodDeclaration.declare(mContext, NULL);
  vector<MethodArgument*> arguments = method->getArguments();

  EXPECT_FALSE(methodDeclaration.isConstant());
  EXPECT_FALSE(methodDeclaration.isField());
  EXPECT_TRUE(methodDeclaration.isMethod());
  EXPECT_FALSE(methodDeclaration.isStaticMethod());
  EXPECT_FALSE(methodDeclaration.isMethodSignature());

  EXPECT_STREQ(method->getName().c_str(), "foo");
  EXPECT_FALSE(method->isStatic());
  EXPECT_EQ(method->getReturnType(), PrimitiveTypes::FLOAT_TYPE);
  EXPECT_EQ(arguments.size(), 2ul);
  EXPECT_EQ(arguments.at(0)->getName(), "intargument");
  EXPECT_EQ(arguments.at(0)->getType(), PrimitiveTypes::INT_TYPE);
  EXPECT_EQ(arguments.at(1)->getName(), "floatargument");
  EXPECT_EQ(arguments.at(1)->getType(), PrimitiveTypes::FLOAT_TYPE);
}
