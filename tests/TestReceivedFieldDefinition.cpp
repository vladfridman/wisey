//
//  TestReceivedFieldDefinition.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ReceivedFieldDefinition}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MockExpression.hpp"
#include "MockConcreteObjectType.hpp"
#include "MockType.hpp"
#include "TestPrefix.hpp"
#include "wisey/ControllerTypeSpecifierFull.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/ReceivedFieldDefinition.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ReceivedFieldDefinitionTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockType>* mType;
  NiceMock<MockExpression>* mExpression;
  NiceMock<MockConcreteObjectType>* mObject;
  string mName;
  ReceivedFieldDefinition* mFieldDeclaration;
  
public:
  
  ReceivedFieldDefinitionTest() :
  mType(new NiceMock<MockType>()),
  mExpression(new NiceMock<MockExpression>()),
  mObject(new NiceMock<MockConcreteObjectType>()),
  mName("mField") {
    TestPrefix::generateIR(mContext);
    
    const PrimitiveTypeSpecifier* intSpecifier = PrimitiveTypes::INT->newTypeSpecifier(0);
    mFieldDeclaration = new ReceivedFieldDefinition(intSpecifier, mName, 0);
  }
  
  ~ReceivedFieldDefinitionTest() {
    delete mFieldDeclaration;
    delete mObject;
  }
};

TEST_F(ReceivedFieldDefinitionTest, declareTest) {
  IField* field = mFieldDeclaration->define(mContext, mObject);
  
  EXPECT_FALSE(field->isConstant());
  EXPECT_TRUE(field->isField());
  EXPECT_FALSE(field->isMethod());
  EXPECT_FALSE(field->isStaticMethod());
  EXPECT_FALSE(field->isMethodSignature());
  EXPECT_FALSE(field->isLLVMFunction());

  EXPECT_EQ(field->getType(), PrimitiveTypes::INT);
  EXPECT_STREQ(field->getName().c_str(), "mField");

  EXPECT_FALSE(field->isFixed());
  EXPECT_FALSE(field->isInjected());
  EXPECT_TRUE(field->isReceived());
  EXPECT_FALSE(field->isState());
}

TEST_F(ReceivedFieldDefinitionTest, isAssignableTest) {
  IField* field = mFieldDeclaration->define(mContext, mObject);

  ON_CALL(*mObject, isController()).WillByDefault(Return(true));
  EXPECT_TRUE(field->isAssignable(mObject));

  ON_CALL(*mObject, isController()).WillByDefault(Return(false));
  EXPECT_FALSE(field->isAssignable(mObject));
}
