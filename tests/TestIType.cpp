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

TEST_F(ITypeTest, isOwnerTypeTest) {
  NiceMock<MockType> mockType;
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(PRIMITIVE_TYPE));
  EXPECT_FALSE(IType::isOwnerType(&mockType));
  
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(MODEL_TYPE));
  EXPECT_FALSE(IType::isOwnerType(&mockType));
  
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(INTERFACE_TYPE));
  EXPECT_FALSE(IType::isOwnerType(&mockType));
  
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(CONTROLLER_TYPE));
  EXPECT_FALSE(IType::isOwnerType(&mockType));
  
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(MODEL_OWNER_TYPE));
  EXPECT_TRUE(IType::isOwnerType(&mockType));
  
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(INTERFACE_OWNER_TYPE));
  EXPECT_TRUE(IType::isOwnerType(&mockType));
  
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(CONTROLLER_OWNER_TYPE));
  EXPECT_TRUE(IType::isOwnerType(&mockType));
}
