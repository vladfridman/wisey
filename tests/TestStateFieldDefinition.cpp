//
//  TestStateFieldDefinition.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link StateFieldDefinition}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MockExpression.hpp"
#include "MockConcreteObjectType.hpp"
#include "MockType.hpp"
#include "PrimitiveTypes.hpp"
#include "PrimitiveTypeSpecifier.hpp"
#include "StateFieldDefinition.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct StateFieldDefinitionTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockConcreteObjectType>* mObject;
  string mName;
  StateFieldDefinition* mFieldDeclaration;
  
public:
  
  StateFieldDefinitionTest() :
  mObject(new NiceMock<MockConcreteObjectType>()),
  mName("mField") {
    const PrimitiveTypeSpecifier* intSpecifier = PrimitiveTypes::INT->newTypeSpecifier(0);
    mFieldDeclaration = new StateFieldDefinition(intSpecifier, mName, 0);
    ON_CALL(*mObject, isNode()).WillByDefault(Return(false));
  }
  
  ~StateFieldDefinitionTest() {
    delete mFieldDeclaration;
    delete mObject;
  }
};

TEST_F(StateFieldDefinitionTest, declareTest) {
  IField* field = mFieldDeclaration->define(mContext, mObject);
  
  EXPECT_FALSE(field->isConstant());
  EXPECT_TRUE(field->isField());
  EXPECT_FALSE(field->isMethod());
  EXPECT_FALSE(field->isStaticMethod());
  EXPECT_FALSE(field->isMethodSignature());
  EXPECT_FALSE(field->isLLVMFunction());

  EXPECT_EQ(field->getType(), PrimitiveTypes::INT);
  EXPECT_STREQ(field->getName().c_str(), "mField");
  EXPECT_TRUE(field->isAssignable(mObject));
  
  EXPECT_FALSE(field->isInjected());
  EXPECT_FALSE(field->isReceived());
  EXPECT_TRUE(field->isState());
}

