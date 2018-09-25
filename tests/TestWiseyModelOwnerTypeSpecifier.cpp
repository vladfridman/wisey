//
//  WiseyModelOwnerTypeSpecifier.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link WiseyModelOwnerTypeSpecifier}
//

#include <gtest/gtest.h>

#include "WiseyModelOwnerTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

using ::testing::Test;

struct WiseyModelOwnerTypeSpecifierTest : public Test {
  IRGenerationContext mContext;
  WiseyModelOwnerTypeSpecifier* mWiseyModelOwnerTypeSpecifier;
  
  WiseyModelOwnerTypeSpecifierTest() {
    mWiseyModelOwnerTypeSpecifier = new WiseyModelOwnerTypeSpecifier(3);
  }
};

TEST_F(WiseyModelOwnerTypeSpecifierTest, getTypeTest) {
  const IType* type = mWiseyModelOwnerTypeSpecifier->getType(mContext);
  
  EXPECT_TRUE(type->isNative());
  EXPECT_FALSE(type->isReference());
  EXPECT_TRUE(type->isOwner());
  EXPECT_STREQ("::wisey::model*", type->getTypeName().c_str());
  EXPECT_EQ(3, mWiseyModelOwnerTypeSpecifier->getLine());
}

TEST_F(WiseyModelOwnerTypeSpecifierTest, twoGetsReturnSameTypeObjectTest) {
  const IType* type1 = mWiseyModelOwnerTypeSpecifier->getType(mContext);
  const IType* type2 = mWiseyModelOwnerTypeSpecifier->getType(mContext);
  
  EXPECT_EQ(type1, type2);
}

TEST_F(WiseyModelOwnerTypeSpecifierTest, printToStreamTest) {
  stringstream stringStream;
  mWiseyModelOwnerTypeSpecifier->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::wisey::model*", stringStream.str().c_str());
}
