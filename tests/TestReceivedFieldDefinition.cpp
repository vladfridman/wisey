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
#include "TestFileRunner.hpp"
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
    mFieldDeclaration = ReceivedFieldDefinition::create(intSpecifier, mName, 0);
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

  EXPECT_FALSE(field->isInjected());
  EXPECT_TRUE(field->isReceived());
  EXPECT_FALSE(field->isState());
}

TEST_F(ReceivedFieldDefinitionTest, isAssignableTest) {
  IField* field = mFieldDeclaration->define(mContext, mObject);

  ON_CALL(*mObject, isModel()).WillByDefault(Return(false));
  EXPECT_TRUE(field->isAssignable(mObject));

  ON_CALL(*mObject, isModel()).WillByDefault(Return(true));
  EXPECT_FALSE(field->isAssignable(mObject));
}

TEST_F(TestFileRunner, impliedReceivedFieldInNodeRunDeathTest) {
  expectFailCompile("tests/samples/test_implied_received_field_in_node.yz",
                    1,
                    "tests/samples/test_implied_received_field_in_node.yz\\(4\\): Error: "
                    "Received field delcarations in nodes must have 'receive' keyword preceding the field type");
}

TEST_F(TestFileRunner, impliedReceivedFieldInControllerRunDeathTest) {
  expectFailCompile("tests/samples/test_implied_received_field_in_controller.yz",
                    1,
                    "tests/samples/test_implied_received_field_in_controller.yz\\(4\\): Error: "
                    "Received field delcarations in controllers must have 'receive' keyword preceding the field type");
}
