//
//  TestMethodDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/13/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link MethodDefinition}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>

#include "MockObjectType.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/AccessLevel.hpp"
#include "wisey/Argument.hpp"
#include "wisey/IModelTypeSpecifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/MethodDefinition.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/VariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::NiceMock;
using ::testing::Test;

struct MethodDefinitionTest : Test {
  IRGenerationContext mContext;
  const PrimitiveTypeSpecifier* mFloatTypeSpecifier;
  const PrimitiveTypeSpecifier* mIntTypeSpecifier;
  Identifier* mIntArgumentIdentifier;
  Identifier* mFloatArgumentIdentifier;
  VariableDeclaration* mIntArgument;
  VariableDeclaration* mFloatArgument;
  VariableList mArguments;
  Block* mBlock;
  CompoundStatement* mCompoundStatement;
  NiceMock<MockObjectType>* mObject;
  
  MethodDefinitionTest() :
  mFloatTypeSpecifier(PrimitiveTypes::FLOAT_TYPE->newTypeSpecifier(0)),
  mIntTypeSpecifier(PrimitiveTypes::INT_TYPE->newTypeSpecifier(0)),
  mIntArgumentIdentifier(new Identifier("intargument", 0)),
  mFloatArgumentIdentifier(new Identifier("floatargument", 0)),
  mIntArgument(VariableDeclaration::create(mIntTypeSpecifier, mIntArgumentIdentifier, 0)),
  mFloatArgument(VariableDeclaration::create(mFloatTypeSpecifier, mFloatArgumentIdentifier, 0)),
  mBlock(new Block()),
  mCompoundStatement(new CompoundStatement(mBlock, 0)),
  mObject(new NiceMock<MockObjectType>()) {
    TestPrefix::generateIR(mContext);
  }
};

TEST_F(MethodDefinitionTest, methodDescriptorExtractTest) {
  mArguments.push_back(mIntArgument);
  mArguments.push_back(mFloatArgument);
  vector<IModelTypeSpecifier*> thrownExceptions;
  MethodDefinition methodDefinition(AccessLevel::PUBLIC_ACCESS,
                                    PrimitiveTypes::FLOAT_TYPE->newTypeSpecifier(0),
                                    "foo",
                                    mArguments,
                                    thrownExceptions,
                                    new MethodQualifiers(0),
                                    mCompoundStatement,
                                    0);
  IMethod* method = methodDefinition.define(mContext, mObject);
  vector<const wisey::Argument*> arguments = method->getArguments();
  
  EXPECT_FALSE(methodDefinition.isConstant());
  EXPECT_FALSE(methodDefinition.isField());
  EXPECT_TRUE(methodDefinition.isMethod());
  EXPECT_FALSE(methodDefinition.isStaticMethod());
  EXPECT_FALSE(methodDefinition.isMethodSignature());
  EXPECT_FALSE(methodDefinition.isLLVMFunction());
  
  EXPECT_FALSE(method->isStatic());
  EXPECT_STREQ(method->getName().c_str(), "foo");
  EXPECT_EQ(method->getReturnType(), PrimitiveTypes::FLOAT_TYPE);
  EXPECT_EQ(arguments.size(), 2ul);
  EXPECT_EQ(arguments.at(0)->getName(), "intargument");
  EXPECT_EQ(arguments.at(0)->getType(), PrimitiveTypes::INT_TYPE);
  EXPECT_EQ(arguments.at(1)->getName(), "floatargument");
  EXPECT_EQ(arguments.at(1)->getType(), PrimitiveTypes::FLOAT_TYPE);
}

TEST_F(TestFileRunner, methodDecalarationIntFunctionRunTest) {
  runFile("tests/samples/test_int_method.yz", "10");
}

TEST_F(TestFileRunner, methodDecalarationImpliedReturnRunTest) {
  runFile("tests/samples/test_implied_return.yz", "5");
}

TEST_F(TestFileRunner, methodDecalarationMultipleParametersRunTest) {
  runFile("tests/samples/test_method_multiple_arguments.yz", "6");
}

