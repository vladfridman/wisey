//
//  TestExternalConstantDeclaration.cpp
//  runtests
//
//  Created by Vladimir Fridman on 5/16/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ExternalConstantDeclaration}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestFileRunner.hpp"
#include "wisey/ExternalConstantDeclaration.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Test;

struct ExternalConstantDeclarationTest : public Test {
  IRGenerationContext mContext;
  string mName;
  ExternalConstantDeclaration* mExternalConstantDeclaration;
  
public:
  
  ExternalConstantDeclarationTest() :
  mName("MYCONSTANT") {
    const PrimitiveTypeSpecifier* typeSpecifier = PrimitiveTypes::INT->newTypeSpecifier(0);
    mExternalConstantDeclaration = new ExternalConstantDeclaration(typeSpecifier, mName, 0);
    
  }
  
  static void printExpression(IRGenerationContext& context, iostream& stream) {
    stream << "expression";
  }
};

TEST_F(ExternalConstantDeclarationTest, declareTest) {
  EXPECT_TRUE(mExternalConstantDeclaration->isConstant());
  EXPECT_FALSE(mExternalConstantDeclaration->isField());
  EXPECT_FALSE(mExternalConstantDeclaration->isMethod());
  EXPECT_FALSE(mExternalConstantDeclaration->isStaticMethod());
  EXPECT_FALSE(mExternalConstantDeclaration->isMethodSignature());
  EXPECT_FALSE(mExternalConstantDeclaration->isLLVMFunction());
  
  EXPECT_NE(mExternalConstantDeclaration->define(mContext, NULL), nullptr);
}
