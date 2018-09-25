//
//  TestConstantDefinition.cpp
//  runtests
//
//  Created by Vladimir Fridman on 10/19/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ConstantDefinition}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "TestFileRunner.hpp"
#include "MockExpression.hpp"
#include "ConstantDefinition.hpp"
#include "PrimitiveTypes.hpp"
#include "PrimitiveTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ConstantDefinitionTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockExpression>* mExpression;
  string mName;
  ConstantDefinition* mConstantDefinition;
  
public:
  
  ConstantDefinitionTest() :
  mExpression(new NiceMock<MockExpression>()),
  mName("MYCONSTANT") {
    ON_CALL(*mExpression, printToStream(_, _)).WillByDefault(Invoke(printExpression));
    ON_CALL(*mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));

    const PrimitiveTypeSpecifier* typeSpecifier = PrimitiveTypes::INT->newTypeSpecifier(0);
    mConstantDefinition = new ConstantDefinition(PUBLIC_ACCESS,
                                                 typeSpecifier,
                                                 mName,
                                                 mExpression,
                                                 0);

  }
  
  ~ConstantDefinitionTest() {
    delete mExpression;
  }
  
  static void printExpression(IRGenerationContext& context, iostream& stream) {
    stream << "expression";
  }
};

TEST_F(ConstantDefinitionTest, declareTest) {
  EXPECT_TRUE(mConstantDefinition->isConstant());
  EXPECT_FALSE(mConstantDefinition->isField());
  EXPECT_FALSE(mConstantDefinition->isMethod());
  EXPECT_FALSE(mConstantDefinition->isStaticMethod());
  EXPECT_FALSE(mConstantDefinition->isMethodSignature());
  EXPECT_FALSE(mConstantDefinition->isLLVMFunction());

  EXPECT_NE(mConstantDefinition->define(mContext, NULL), nullptr);
}

TEST_F(TestFileRunner, constantDeclarationInObjectRunTest) {
  runFile("tests/samples/test_constant_declaration_in_object.yz", 1);
}

TEST_F(TestFileRunner, constantDeclarationInInterfaceRunTest) {
  runFile("tests/samples/test_constant_declaration_in_interface.yz", 1);
}
