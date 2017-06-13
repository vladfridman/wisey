//
//  TestOwnerTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link OwnerTypeSpecifier}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MockObjectOwnerType.hpp"
#include "MockObjectTypeSpecifier.hpp"
#include "MockObjectType.hpp"
#include "wisey/OwnerTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Test;

struct OwnerTypeSpecifierTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockObjectTypeSpecifier>* mObjectTypeSpecifier;
  
  OwnerTypeSpecifierTest() : mObjectTypeSpecifier(new NiceMock<MockObjectTypeSpecifier>()) {
  }
};

TEST_F(OwnerTypeSpecifierTest, getTypeTest) {
  NiceMock<MockObjectType> mockObjectType;
  NiceMock<MockObjectOwnerType> mockOwnerObjectType;
  ON_CALL(mockObjectType, getOwner()).WillByDefault(Return(&mockOwnerObjectType));
  ON_CALL(*mObjectTypeSpecifier, getType(_)).WillByDefault(Return(&mockObjectType));
  
  OwnerTypeSpecifier ownerTypeSpecifier(mObjectTypeSpecifier);
  
  EXPECT_EQ(ownerTypeSpecifier.getType(mContext), &mockOwnerObjectType);
}
