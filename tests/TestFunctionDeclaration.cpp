//
//  TestFunctionDeclaration.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/13/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link FunctionDecalaration}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/FunctionDeclaration.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/PrimitiveTypeSpecifier.hpp"
#include "yazyk/VariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

struct FunctionDecalarationTest : Test {
  IRGenerationContext mContext;
  PrimitiveTypeSpecifier mFloatTypeSpecifier;
  PrimitiveTypeSpecifier mIntTypeSpecifier;
  Identifier mFooFunctionIdentifier;
  Identifier mMainFunctionIdentifier;
  Identifier mArgumentIdentifier;
  VariableDeclaration mIntArgument;
  VariableDeclaration mFloatArgument;
  VariableList mArguments;
  Block mBlock;
  CompoundStatement mCompoundStatement;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  FunctionDecalarationTest() :
    mFloatTypeSpecifier(PrimitiveTypeSpecifier(PRIMITIVE_TYPE_FLOAT)),
    mIntTypeSpecifier(PrimitiveTypeSpecifier(PRIMITIVE_TYPE_INT)),
    mFooFunctionIdentifier(Identifier("foo")),
    mMainFunctionIdentifier(Identifier("main")),
    mArgumentIdentifier(Identifier("bar")),
    mIntArgument(VariableDeclaration(mIntTypeSpecifier, mArgumentIdentifier)),
    mFloatArgument(VariableDeclaration(mFloatTypeSpecifier, mArgumentIdentifier)),
    mCompoundStatement(CompoundStatement(mBlock)) {
      mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~FunctionDecalarationTest() {
    delete mStringStream;
  }
};

TEST_F(FunctionDecalarationTest, FunctionFooDeclartaionTest) {
  mArguments.push_back(&mIntArgument);
  FunctionDeclaration functionDeclaration(mFloatTypeSpecifier,
                                          mFooFunctionIdentifier,
                                          mArguments,
                                          mCompoundStatement);
  Value* function = functionDeclaration.generateIR(mContext);
  
  *mStringStream << *function;
  string expected = string() +
    "\ndefine internal float @foo(i32 %bar) {" +
    "\nentry:" +
    "\n  %bar.param = alloca i32" +
    "\n  store i32 %bar, i32* %bar.param" +
    "\n}" +
    "\n";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  EXPECT_EQ(true, mContext.getMainFunction() == NULL);
}

TEST_F(FunctionDecalarationTest, FunctionMainDeclartaionTest) {
  mArguments.push_back(&mFloatArgument);
  FunctionDeclaration functionDeclaration(mIntTypeSpecifier,
                                          mMainFunctionIdentifier,
                                          mArguments,
                                          mCompoundStatement);
  Value* function = functionDeclaration.generateIR(mContext);
  
  *mStringStream << *function;
  string expected = string() +
    "\ndefine internal i32 @main(float %bar) {" +
    "\nentry:" +
    "\n  %bar.param = alloca float" +
    "\n  store float %bar, float* %bar.param" +
    "\n}" +
    "\n";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  ASSERT_EQ(false, mContext.getMainFunction() == NULL);
  EXPECT_EQ(function, mContext.getMainFunction());
}

TEST_F(TestFileSampleRunner, FunctionDecalarationIntFunctionTest) {
  runFile("tests/samples/test_int_function.yz", "10");
}
