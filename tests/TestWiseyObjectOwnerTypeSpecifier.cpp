//
//  TestWiseyObjectOwnerTypeSpecifier.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/28/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link WiseyObjectOwnerTypeSpecifier}
//

#include <gtest/gtest.h>

#include "wisey/WiseyObjectOwnerTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

using ::testing::Test;

struct WiseyObjectOwnerTypeSpecifierTest : public Test {
  IRGenerationContext mContext;
  WiseyObjectOwnerTypeSpecifier* mWiseyObjectOwnerTypeSpecifier;
  
  WiseyObjectOwnerTypeSpecifierTest() {
    mWiseyObjectOwnerTypeSpecifier = new WiseyObjectOwnerTypeSpecifier(9);
  }
};

TEST_F(WiseyObjectOwnerTypeSpecifierTest, getTypeTest) {
  const IType* type = mWiseyObjectOwnerTypeSpecifier->getType(mContext);
  
  EXPECT_TRUE(type->isNative());
  EXPECT_FALSE(type->isReference());
  EXPECT_TRUE(type->isOwner());
  EXPECT_STREQ("::wisey::object*", type->getTypeName().c_str());
  EXPECT_EQ(9, mWiseyObjectOwnerTypeSpecifier->getLine());
}

TEST_F(WiseyObjectOwnerTypeSpecifierTest, twoGetsReturnSameTypeObjectTest) {
  const IType* type1 = mWiseyObjectOwnerTypeSpecifier->getType(mContext);
  const IType* type2 = mWiseyObjectOwnerTypeSpecifier->getType(mContext);
  
  EXPECT_EQ(type1, type2);
}

TEST_F(WiseyObjectOwnerTypeSpecifierTest, printToStreamTest) {
  stringstream stringStream;
  mWiseyObjectOwnerTypeSpecifier->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::wisey::object*", stringStream.str().c_str());
}
