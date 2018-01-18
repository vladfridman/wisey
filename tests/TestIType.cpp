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
  
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(NODE_TYPE));
  EXPECT_FALSE(IType::isOwnerType(&mockType));
  
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(MODEL_OWNER_TYPE));
  EXPECT_TRUE(IType::isOwnerType(&mockType));
  
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(INTERFACE_OWNER_TYPE));
  EXPECT_TRUE(IType::isOwnerType(&mockType));
  
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(CONTROLLER_OWNER_TYPE));
  EXPECT_TRUE(IType::isOwnerType(&mockType));
  
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(NODE_OWNER_TYPE));
  EXPECT_TRUE(IType::isOwnerType(&mockType));
  
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(NULL_TYPE_KIND));
  EXPECT_FALSE(IType::isOwnerType(&mockType));
}

TEST_F(ITypeTest, isReferenceTypeTest) {
  NiceMock<MockType> mockType;
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(PRIMITIVE_TYPE));
  EXPECT_FALSE(IType::isReferenceType(&mockType));
  
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(MODEL_TYPE));
  EXPECT_TRUE(IType::isReferenceType(&mockType));
  
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(INTERFACE_TYPE));
  EXPECT_TRUE(IType::isReferenceType(&mockType));
  
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(CONTROLLER_TYPE));
  EXPECT_TRUE(IType::isReferenceType(&mockType));
  
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(NODE_TYPE));
  EXPECT_TRUE(IType::isReferenceType(&mockType));
  
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(MODEL_OWNER_TYPE));
  EXPECT_FALSE(IType::isReferenceType(&mockType));
  
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(INTERFACE_OWNER_TYPE));
  EXPECT_FALSE(IType::isReferenceType(&mockType));
  
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(CONTROLLER_OWNER_TYPE));
  EXPECT_FALSE(IType::isReferenceType(&mockType));
  
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(NODE_OWNER_TYPE));
  EXPECT_FALSE(IType::isReferenceType(&mockType));
  
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(NULL_TYPE_KIND));
  EXPECT_FALSE(IType::isReferenceType(&mockType));
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

TEST_F(ITypeTest, isArrayTypeTest) {
  NiceMock<MockType> mockType;
  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(PRIMITIVE_TYPE));
  EXPECT_FALSE(IType::isArrayType(&mockType));

  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(ARRAY_TYPE));
  EXPECT_TRUE(IType::isArrayType(&mockType));

  ON_CALL(mockType, getTypeKind()).WillByDefault(Return(ARRAY_OWNER_TYPE));
  EXPECT_TRUE(IType::isArrayType(&mockType));
}
