//
//  TestMethodDeclaration.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/13/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link MethodDeclaration}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>

#include "TestFileSampleRunner.hpp"
#include "wisey/AccessLevel.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodDeclaration.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/ProgramPrefix.hpp"
#include "wisey/VariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct MethodDeclarationTest : Test {
  IRGenerationContext mContext;
  PrimitiveTypeSpecifier* mFloatTypeSpecifier;
  PrimitiveTypeSpecifier* mIntTypeSpecifier;
  Identifier* mIntArgumentIdentifier;
  Identifier* mFloatArgumentIdentifier;
  VariableDeclaration* mIntArgument;
  VariableDeclaration* mFloatArgument;
  VariableList mArguments;
  Block* mBlock;
  CompoundStatement* mCompoundStatement;
  
  MethodDeclarationTest() :
  mFloatTypeSpecifier(new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE)),
  mIntTypeSpecifier(new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE)),
  mIntArgumentIdentifier(new Identifier("intargument")),
  mFloatArgumentIdentifier(new Identifier("floatargument")),
  mIntArgument(new VariableDeclaration(mIntTypeSpecifier, mIntArgumentIdentifier)),
  mFloatArgument(new VariableDeclaration(mFloatTypeSpecifier, mFloatArgumentIdentifier)),
  mBlock(new Block()),
  mCompoundStatement(new CompoundStatement(mBlock)) {
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
  }
};

TEST_F(MethodDeclarationTest, methodDescriptorExtractTest) {
  mArguments.push_back(mIntArgument);
  mArguments.push_back(mFloatArgument);
  vector<ModelTypeSpecifier*> thrownExceptions;
  MethodDeclaration methodDeclaration(AccessLevel::PUBLIC_ACCESS,
                                      new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE),
                                      "foo",
                                      mArguments,
                                      thrownExceptions,
                                      mCompoundStatement);
  IMethod* method = methodDeclaration.createMethod(mContext);
  vector<MethodArgument*> arguments = method->getArguments();
  
  EXPECT_FALSE(method->isStatic());
  EXPECT_STREQ(method->getName().c_str(), "foo");
  EXPECT_EQ(method->getReturnType(), PrimitiveTypes::FLOAT_TYPE);
  EXPECT_EQ(arguments.size(), 2ul);
  EXPECT_EQ(arguments.at(0)->getName(), "intargument");
  EXPECT_EQ(arguments.at(0)->getType(), PrimitiveTypes::INT_TYPE);
  EXPECT_EQ(arguments.at(1)->getName(), "floatargument");
  EXPECT_EQ(arguments.at(1)->getType(), PrimitiveTypes::FLOAT_TYPE);
}

TEST_F(TestFileSampleRunner, methodDecalarationIntFunctionRunTest) {
  runFile("tests/samples/test_int_method.yz", "10");
}

TEST_F(TestFileSampleRunner, methodDecalarationImpliedReturnRunTest) {
  runFile("tests/samples/test_implied_return.yz", "5");
}

TEST_F(TestFileSampleRunner, methodDecalarationMultipleParametersRunTest) {
  runFile("tests/samples/test_method_multiple_arguments.yz", "6");
}
