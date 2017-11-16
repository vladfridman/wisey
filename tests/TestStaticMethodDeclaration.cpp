//
//  TestStaticMethodDeclaration.cpp
//  runtests
//
//  Created by Vladimir Fridman on 9/29/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link StaticMethodDeclaration}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>

#include "TestFileSampleRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/AccessLevel.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/StaticMethodDeclaration.hpp"
#include "wisey/VariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct StaticMethodDeclarationTest : Test {
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
  
  StaticMethodDeclarationTest() :
  mFloatTypeSpecifier(new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE)),
  mIntTypeSpecifier(new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE)),
  mIntArgumentIdentifier(new Identifier("intargument")),
  mFloatArgumentIdentifier(new Identifier("floatargument")),
  mIntArgument(new VariableDeclaration(mIntTypeSpecifier, mIntArgumentIdentifier)),
  mFloatArgument(new VariableDeclaration(mFloatTypeSpecifier, mFloatArgumentIdentifier)),
  mBlock(new Block()),
  mCompoundStatement(new CompoundStatement(mBlock)) {
    TestPrefix::generateIR(mContext);
  }
};

TEST_F(StaticMethodDeclarationTest, methodDescriptorExtractTest) {
  mArguments.push_back(mIntArgument);
  mArguments.push_back(mFloatArgument);
  vector<ModelTypeSpecifier*> thrownExceptions;
  StaticMethodDeclaration methodDeclaration(AccessLevel::PUBLIC_ACCESS,
                                            new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE),
                                            "foo",
                                            mArguments,
                                            thrownExceptions,
                                            mCompoundStatement);
  IMethod* method = methodDeclaration.declare(mContext);
  vector<MethodArgument*> arguments = method->getArguments();
  
  EXPECT_TRUE(method->isStatic());
  EXPECT_STREQ(method->getName().c_str(), "foo");
  EXPECT_EQ(method->getReturnType(), PrimitiveTypes::FLOAT_TYPE);
  EXPECT_EQ(arguments.size(), 2ul);
  EXPECT_EQ(arguments.at(0)->getName(), "intargument");
  EXPECT_EQ(arguments.at(0)->getType(), PrimitiveTypes::INT_TYPE);
  EXPECT_EQ(arguments.at(1)->getName(), "floatargument");
  EXPECT_EQ(arguments.at(1)->getType(), PrimitiveTypes::FLOAT_TYPE);
}

