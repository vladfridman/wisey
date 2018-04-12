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
#include "MockObjectType.hpp"
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
  NiceMock<MockObjectType>* mObject;
  string mName;
  ReceivedFieldDefinition* mFieldDeclaration;
  
public:
  
  ReceivedFieldDefinitionTest() :
  mType(new NiceMock<MockType>()),
  mExpression(new NiceMock<MockExpression>()),
  mObject(new NiceMock<MockObjectType>()),
  mName("mField") {
    TestPrefix::generateIR(mContext);
    
    const PrimitiveTypeSpecifier* intSpecifier = PrimitiveTypes::INT_TYPE->newTypeSpecifier();
    mFieldDeclaration = new ReceivedFieldDefinition(intSpecifier, mName, 0);
  }
  
  ~ReceivedFieldDefinitionTest() {
    delete mFieldDeclaration;
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

  EXPECT_EQ(field->getType(), PrimitiveTypes::INT_TYPE);
  EXPECT_STREQ(field->getName().c_str(), "mField");
  EXPECT_TRUE(field->isAssignable());

  EXPECT_FALSE(field->isFixed());
  EXPECT_FALSE(field->isInjected());
  EXPECT_TRUE(field->isReceived());
  EXPECT_FALSE(field->isState());
}

TEST_F(ReceivedFieldDefinitionTest, threadReceivedControllerFieldDeathTest) {
  Mock::AllowLeak(mObject);
  Mock::AllowLeak(mType);
  Mock::AllowLeak(mExpression);

  ON_CALL(*mObject, isThread()).WillByDefault(Return(true));
  PackageType* packageType = new PackageType("wisey.lang");
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  ControllerTypeSpecifierFull* specifier = new ControllerTypeSpecifierFull(packageExpression,
                                                                           "CCallStack",
                                                                           0);
  ReceivedFieldDefinition* field = new ReceivedFieldDefinition(specifier, "mProgram", 3);

  EXPECT_EXIT(field->define(mContext, mObject),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(3\\): Error: Threads are only allowed to receive "
              "models or primitives");
}
