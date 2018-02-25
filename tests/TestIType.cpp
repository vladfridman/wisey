//
//  TestIType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IType}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MockType.hpp"
#include "wisey/IType.hpp"

using namespace wisey;

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ITypeTest : public Test {
  
  ITypeTest() { }
  
};

TEST_F(ITypeTest, isObjectTypeTest) {
  NiceMock<MockType> mockType;
  ON_CALL(mockType, isController()).WillByDefault(Return(true));
  EXPECT_TRUE(IType::isObjectType(&mockType));

  ON_CALL(mockType, isController()).WillByDefault(Return(false));
  ON_CALL(mockType, isModel()).WillByDefault(Return(true));
  EXPECT_TRUE(IType::isObjectType(&mockType));

  ON_CALL(mockType, isModel()).WillByDefault(Return(false));
  ON_CALL(mockType, isInterface()).WillByDefault(Return(true));
  EXPECT_TRUE(IType::isObjectType(&mockType));

  ON_CALL(mockType, isInterface()).WillByDefault(Return(false));
  ON_CALL(mockType, isNode()).WillByDefault(Return(true));
  EXPECT_TRUE(IType::isObjectType(&mockType));

  ON_CALL(mockType, isNode()).WillByDefault(Return(false));
  ON_CALL(mockType, isArray()).WillByDefault(Return(true));
  EXPECT_FALSE(IType::isObjectType(&mockType));
}

TEST_F(ITypeTest, isConcreteObjectTypeTest) {
  NiceMock<MockType> mockType;
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(PRIMITIVE_TYPE));
  EXPECT_FALSE(IType::isConcreteObjectType(&mockType));
  
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(MODEL_TYPE));
  EXPECT_TRUE(IType::isConcreteObjectType(&mockType));
  
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(INTERFACE_TYPE));
  EXPECT_FALSE(IType::isConcreteObjectType(&mockType));
  
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(CONTROLLER_TYPE));
  EXPECT_TRUE(IType::isConcreteObjectType(&mockType));
  
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(NODE_TYPE));
  EXPECT_TRUE(IType::isConcreteObjectType(&mockType));
  
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(MODEL_OWNER_TYPE));
  EXPECT_FALSE(IType::isConcreteObjectType(&mockType));
  
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(INTERFACE_OWNER_TYPE));
  EXPECT_FALSE(IType::isConcreteObjectType(&mockType));
  
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(CONTROLLER_OWNER_TYPE));
  EXPECT_FALSE(IType::isConcreteObjectType(&mockType));
  
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(NODE_OWNER_TYPE));
  EXPECT_FALSE(IType::isConcreteObjectType(&mockType));
  
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(NULL_TYPE_KIND));
  EXPECT_FALSE(IType::isConcreteObjectType(&mockType));
}

