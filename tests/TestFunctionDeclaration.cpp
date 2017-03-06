//
//  TestFunctionDeclaration.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link FunctionDeclaration}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/AccessSpecifiers.hpp"
#include "yazyk/FunctionDeclaration.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/PrimitiveTypes.hpp"
#include "yazyk/PrimitiveTypeSpecifier.hpp"
#include "yazyk/VariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

struct FunctionDeclarationTest : Test {
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
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  FunctionDeclarationTest() :
  mFloatTypeSpecifier(PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE)),
  mIntTypeSpecifier(PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE)),
  mIntArgumentIdentifier(Identifier("intargument")),
  mFloatArgumentIdentifier(Identifier("floatargument")),
  mIntArgument(VariableDeclaration(mIntTypeSpecifier, mIntArgumentIdentifier)),
  mFloatArgument(VariableDeclaration(mFloatTypeSpecifier, mFloatArgumentIdentifier)),
  mCompoundStatement(CompoundStatement(mBlock)) {
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~FunctionDeclarationTest() {
    delete mStringStream;
  }
};

TEST_F(FunctionDeclarationTest, MethodFooDeclartaionTest) {
  mArguments.push_back(&mIntArgument);
  FunctionDeclaration functionDeclaration(AccessSpecifiers::PUBLIC_ACCESS,
                                          mFloatTypeSpecifier,
                                          "foo",
                                          mArguments,
                                          mCompoundStatement);
  Value* method = functionDeclaration.generateIR(mContext);
  
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

TEST_F(FunctionDeclarationTest, MethodMainDeclartaionTest) {
  mArguments.push_back(&mFloatArgument);
  FunctionDeclaration functionDeclaration(AccessSpecifiers::PUBLIC_ACCESS,
                                          mIntTypeSpecifier,
                                          "main",
                                          mArguments,
                                          mCompoundStatement);
  Value* method = functionDeclaration.generateIR(mContext);
  
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
