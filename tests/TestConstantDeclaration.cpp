//
//  TestConstantDeclaration.cpp
//  runtests
//
//  Created by Vladimir Fridman on 10/19/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ConstantDeclaration}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestFileSampleRunner.hpp"
#include "MockExpression.hpp"
#include "wisey/ConstantDeclaration.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Test;

struct ConstantDeclarationTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockExpression>* mExpression;
  string mName;
  ConstantDeclaration* mConstantDeclaration;
  
public:
  
  ConstantDeclarationTest() :
  mExpression(new NiceMock<MockExpression>()),
  mName("MYCONSTANT") {
    ON_CALL(*mExpression, printToStream(_, _)).WillByDefault(Invoke(printExpression));
    
    const PrimitiveTypeSpecifier* typeSpecifier = PrimitiveTypes::INT_TYPE->newTypeSpecifier();
    mConstantDeclaration = new ConstantDeclaration(PUBLIC_ACCESS, typeSpecifier, mName, mExpression);

  }
  
  ~ConstantDeclarationTest() {
    delete mExpression;
  }
  
  static void printExpression(IRGenerationContext& context, iostream& stream) {
    stream << "expression";
  }
};

TEST_F(ConstantDeclarationTest, declareTest) {
  EXPECT_TRUE(mConstantDeclaration->isConstant());
  EXPECT_FALSE(mConstantDeclaration->isField());
  EXPECT_FALSE(mConstantDeclaration->isMethod());
  EXPECT_FALSE(mConstantDeclaration->isStaticMethod());
  EXPECT_FALSE(mConstantDeclaration->isMethodSignature());

  EXPECT_NE(mConstantDeclaration->define(mContext, NULL), nullptr);
}

TEST_F(TestFileSampleRunner, constantDeclarationInObjectRunTest) {
  runFile("tests/samples/test_constant_declaration_in_object.yz", "1");
}

TEST_F(TestFileSampleRunner, constantDeclarationInInterfaceRunTest) {
  runFile("tests/samples/test_constant_declaration_in_interface.yz", "1");
}
