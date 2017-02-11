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
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/AccessSpecifiers.hpp"
#include "yazyk/IRGenerationContext.hpp"
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
  Identifier mFooFunctionIdentifier;
  Identifier mMainFunctionIdentifier;
  Identifier mIntArgumentIdentifier;
  Identifier mFloatArgumentIdentifier;
  VariableDeclaration mIntArgument;
  VariableDeclaration mFloatArgument;
  VariableList mArguments;
  Block mBlock;
  CompoundStatement mCompoundStatement;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  MethodDeclarationTest() :
    mFloatTypeSpecifier(PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE)),
    mIntTypeSpecifier(PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE)),
    mFooFunctionIdentifier(Identifier("foo")),
    mMainFunctionIdentifier(Identifier("main")),
    mIntArgumentIdentifier(Identifier("intargument")),
    mFloatArgumentIdentifier(Identifier("floatargument")),
    mIntArgument(VariableDeclaration(mIntTypeSpecifier, mIntArgumentIdentifier)),
    mFloatArgument(VariableDeclaration(mFloatTypeSpecifier, mFloatArgumentIdentifier)),
    mCompoundStatement(CompoundStatement(mBlock)) {
      mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~MethodDeclarationTest() {
    delete mStringStream;
  }
};

TEST_F(MethodDeclarationTest, MethodFooDeclartaionTest) {
  mArguments.push_back(&mIntArgument);
  MethodDeclaration methodDeclaration(AccessSpecifiers::PUBLIC_ACCESS,
                                      mFloatTypeSpecifier,
                                      mFooFunctionIdentifier,
                                      mArguments,
                                      mCompoundStatement);
  Value* method = methodDeclaration.generateIR(mContext);
  
  *mStringStream << *method;
  string expected = string() +
    "\ndefine internal float @foo(i32 %intargument) {" +
    "\nentry:" +
    "\n  %intargument.param = alloca i32" +
    "\n  store i32 %intargument, i32* %intargument.param" +
    "\n  ret void" +
    "\n}" +
    "\n";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  EXPECT_EQ(mContext.getMainFunction(), nullptr);
}

TEST_F(MethodDeclarationTest, MethodMainDeclartaionTest) {
  mArguments.push_back(&mFloatArgument);
  MethodDeclaration methodDeclaration(AccessSpecifiers::PUBLIC_ACCESS,
                                          mIntTypeSpecifier,
                                          mMainFunctionIdentifier,
                                          mArguments,
                                          mCompoundStatement);
  Value* method = methodDeclaration.generateIR(mContext);
  
  *mStringStream << *method;
  string expected = string() +
    "\ndefine internal i32 @main(float %floatargument) {" +
    "\nentry:" +
    "\n  %floatargument.param = alloca float" +
    "\n  store float %floatargument, float* %floatargument.param" +
    "\n  ret void" +
    "\n}" +
    "\n";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  ASSERT_NE(mContext.getMainFunction(), nullptr);
  EXPECT_EQ(method, mContext.getMainFunction());
}

TEST_F(MethodDeclarationTest, MethodDescriptorExtractTest) {
  mArguments.push_back(&mIntArgument);
  mArguments.push_back(&mFloatArgument);
  MethodDeclaration methodDeclaration(AccessSpecifiers::PUBLIC_ACCESS,
                                      mFloatTypeSpecifier,
                                      mFooFunctionIdentifier,
                                      mArguments,
                                      mCompoundStatement);
  Method* method = methodDeclaration.getMethod(mContext);
  vector<MethodArgument*> arguments = method->getArguments();
  
  EXPECT_STREQ(method->getName().c_str(), "foo");
  EXPECT_EQ(method->getReturnType(), PrimitiveTypes::FLOAT_TYPE);
  EXPECT_EQ(arguments.size(), 2ul);
  EXPECT_EQ(arguments.at(0)->getName(), "intargument");
  EXPECT_EQ(arguments.at(0)->getType(), PrimitiveTypes::INT_TYPE);
  EXPECT_EQ(arguments.at(1)->getName(), "floatargument");
  EXPECT_EQ(arguments.at(1)->getType(), PrimitiveTypes::FLOAT_TYPE);
}

TEST_F(TestFileSampleRunner, MethodDecalarationIntFunctionTest) {
  runFile("tests/samples/test_int_function.yz", "10");
}

TEST_F(TestFileSampleRunner, MethodDecalarationImpliedReturnTest) {
  runFile("tests/samples/test_implied_return.yz", "5");
}

TEST_F(TestFileSampleRunner, MethodDecalarationMultipleParametersTest) {
  runFile("tests/samples/test_method_multiple_arguments.yz", "6");
}
