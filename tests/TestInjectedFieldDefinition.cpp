//
//  TestFieldDeclaration.cpp
//  runtests
//
//  Created by Vladimir Fridman on 10/19/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link InjectedFieldDefinition}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MockExpression.hpp"
#include "MockType.hpp"
#include "wisey/InjectedFieldDefinition.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct InjectedFieldDefinitionTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockType>* mType;
  NiceMock<MockExpression>* mExpression;
  string mName;
  InjectionArgumentList mArguments;
  InjectionArgument* mInjectionArgument;
  InjectedFieldDefinition* mFieldDeclaration;
  
public:
  
  InjectedFieldDefinitionTest() :
  mType(new NiceMock<MockType>()),
  mExpression(new NiceMock<MockExpression>()),
  mName("mField") {
    mInjectionArgument = new InjectionArgument("withFoo", mExpression);
    mArguments.push_back(mInjectionArgument);
    
    const PrimitiveTypeSpecifier* intSpecifier = PrimitiveTypes::INT_TYPE->newTypeSpecifier();
    mFieldDeclaration = new InjectedFieldDefinition(intSpecifier, mName, mArguments);
  }
  
  ~InjectedFieldDefinitionTest() {
    delete mFieldDeclaration;
  }
};

TEST_F(InjectedFieldDefinitionTest, declareTest) {
  IField* field = mFieldDeclaration->define(mContext, NULL);
  
  EXPECT_FALSE(field->isConstant());
  EXPECT_TRUE(field->isField());
  EXPECT_FALSE(field->isMethod());
  EXPECT_FALSE(field->isStaticMethod());
  EXPECT_FALSE(field->isMethodSignature());

  EXPECT_EQ(field->getType(), PrimitiveTypes::INT_TYPE);
  EXPECT_STREQ(field->getName().c_str(), "mField");
  EXPECT_TRUE(field->isAssignable());

  EXPECT_FALSE(field->isFixed());
  EXPECT_TRUE(field->isInjected());
  EXPECT_FALSE(field->isReceived());
  EXPECT_FALSE(field->isState());

  InjectedField* injetedField = (InjectedField*) field;
  EXPECT_EQ(injetedField->getInjectionArguments().size(), 1u);
  EXPECT_EQ(injetedField->getInjectionArguments().at(0), mInjectionArgument);
}

