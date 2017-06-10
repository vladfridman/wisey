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

#include "MockObjectTypeSpecifier.hpp"
#include "MockType.hpp"
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
  NiceMock<MockType> mMockType;
  ON_CALL(*mObjectTypeSpecifier, getType(_)).WillByDefault(Return(&mMockType));
  
  OwnerTypeSpecifier ownerTypeSpecifier(mObjectTypeSpecifier);
  
  EXPECT_EQ(ownerTypeSpecifier.getType(mContext), &mMockType);
  EXPECT_EQ(ownerTypeSpecifier.isOwner(), true);
}
