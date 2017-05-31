//
//  TestIConcreteObjectType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/31/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IConcreteObjectType}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MockConcreteObjectType.hpp"
#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct IConcreteObjectTypeTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockConcreteObjectType> mMockObject;
  
  IConcreteObjectTypeTest() {
    mContext.getScopes().pushScope();
    ON_CALL(mMockObject, getName()).WillByDefault(Return("Object"));
    ON_CALL(mMockObject, getObjectNameGlobalVariableName()).WillByDefault(Return("Object.name"));
  }
  
  ~IConcreteObjectTypeTest() { }
};

TEST_F(IConcreteObjectTypeTest, generateNameGlobalTest) {
  ASSERT_EQ(mContext.getModule()->getGlobalVariable("Object.name"), nullptr);

  IConcreteObjectType::generateNameGlobal(mContext, &mMockObject);

  ASSERT_NE(mContext.getModule()->getGlobalVariable("Object.name"), nullptr);
}

