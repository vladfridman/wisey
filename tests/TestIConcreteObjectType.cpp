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
  Interface* mInterface3;
  
  IConcreteObjectTypeTest() {
    mContext.getScopes().pushScope();
    ON_CALL(mMockObject, getName()).WillByDefault(Return("Object"));
    ON_CALL(mMockObject, getObjectNameGlobalVariableName()).WillByDefault(Return("Object.name"));

    vector<Interface*> interfaces;
    Interface* interface1 = new Interface("Interface1", NULL);
    Interface* interface2 = new Interface("Interface2", NULL);
    mInterface3 = new Interface("Interface3", NULL);
    Interface* interface4 = new Interface("Interface4", NULL);
    interfaces.push_back(interface1);
    interfaces.push_back(interface2);
    interfaces.push_back(mInterface3);
    interfaces.push_back(interface4);
    ON_CALL(mMockObject, getFlattenedInterfaceHierarchy()).WillByDefault(Return(interfaces));
  }
  
  ~IConcreteObjectTypeTest() { }
};

TEST_F(IConcreteObjectTypeTest, generateNameGlobalTest) {
  ASSERT_EQ(mContext.getModule()->getGlobalVariable("Object.name"), nullptr);

  IConcreteObjectType::generateNameGlobal(mContext, &mMockObject);

  ASSERT_NE(mContext.getModule()->getGlobalVariable("Object.name"), nullptr);
}

TEST_F(IConcreteObjectTypeTest, getInterfaceIndexTest) {
  ASSERT_EQ(IConcreteObjectType::getInterfaceIndex(&mMockObject, mInterface3), 3u);
}

