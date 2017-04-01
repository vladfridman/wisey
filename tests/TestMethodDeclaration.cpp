//
//  TestMethodDeclaration.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/13/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link MethodDeclaration}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/AccessLevel.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/MethodArgument.hpp"
#include "yazyk/MethodDeclaration.hpp"
#include "yazyk/PrimitiveTypes.hpp"
#include "yazyk/PrimitiveTypeSpecifier.hpp"
#include "yazyk/VariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

struct MethodDeclarationTest : Test {
  IRGenerationContext mContext;
  PrimitiveTypeSpecifier mFloatTypeSpecifier;
  PrimitiveTypeSpecifier mIntTypeSpecifier;
  Identifier mIntArgumentIdentifier;
  Identifier mFloatArgumentIdentifier;
  VariableDeclaration mIntArgument;
  VariableDeclaration mFloatArgument;
  VariableList mArguments;
  Block mBlock;
  CompoundStatement mCompoundStatement;
  
  MethodDeclarationTest() :
    mFloatTypeSpecifier(PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE)),
    mIntTypeSpecifier(PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE)),
    mIntArgumentIdentifier(Identifier("intargument")),
    mFloatArgumentIdentifier(Identifier("floatargument")),
    mIntArgument(VariableDeclaration(mIntTypeSpecifier, mIntArgumentIdentifier)),
    mFloatArgument(VariableDeclaration(mFloatTypeSpecifier, mFloatArgumentIdentifier)),
    mCompoundStatement(CompoundStatement(mBlock)) {
  }
};

TEST_F(MethodDeclarationTest, methodDescriptorExtractTest) {
  mArguments.push_back(&mIntArgument);
  mArguments.push_back(&mFloatArgument);
  MethodDeclaration methodDeclaration(AccessLevel::PUBLIC_ACCESS,
                                      mFloatTypeSpecifier,
                                      "foo",
                                      mArguments,
                                      mCompoundStatement);
  Method* method = methodDeclaration.createMethod(mContext, 0);
  vector<MethodArgument*> arguments = method->getArguments();
  
  EXPECT_STREQ(method->getName().c_str(), "foo");
  EXPECT_EQ(method->getReturnType(), PrimitiveTypes::FLOAT_TYPE);
  EXPECT_EQ(arguments.size(), 2ul);
  EXPECT_EQ(arguments.at(0)->getName(), "intargument");
  EXPECT_EQ(arguments.at(0)->getType(), PrimitiveTypes::INT_TYPE);
  EXPECT_EQ(arguments.at(1)->getName(), "floatargument");
  EXPECT_EQ(arguments.at(1)->getType(), PrimitiveTypes::FLOAT_TYPE);
}

TEST_F(TestFileSampleRunner, methodDecalarationIntFunctionRunTest) {
  runFile("tests/samples/test_int_function.yz", "10");
}

TEST_F(TestFileSampleRunner, methodDecalarationImpliedReturnRunTest) {
  runFile("tests/samples/test_implied_return.yz", "5");
}

TEST_F(TestFileSampleRunner, methodDecalarationMultipleParametersRunTest) {
  runFile("tests/samples/test_method_multiple_arguments.yz", "6");
}
