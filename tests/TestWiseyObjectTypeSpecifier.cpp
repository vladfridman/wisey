//
//  TestWiseyObjectTypeSpecifier.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/2/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link WiseyObjectTypeSpecifier}
//

#include <gtest/gtest.h>

#include "wisey/WiseyObjectTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

using ::testing::Test;

struct WiseyObjectTypeSpecifierTest : public Test {
  IRGenerationContext mContext;
  WiseyObjectTypeSpecifier* mWiseyObjectTypeSpecifier;
  
  WiseyObjectTypeSpecifierTest() {
    mWiseyObjectTypeSpecifier = new WiseyObjectTypeSpecifier();
  }
};

TEST_F(WiseyObjectTypeSpecifierTest, getTypeTest) {
  const IType* type = mWiseyObjectTypeSpecifier->getType(mContext);
  
  EXPECT_TRUE(type->isNative());
  EXPECT_TRUE(type->isReference());
  EXPECT_FALSE(type->isOwner());
  EXPECT_STREQ("::wisey::object", type->getTypeName().c_str());
}

TEST_F(WiseyObjectTypeSpecifierTest, twoGetsReturnSameTypeObjectTest) {
  const IType* type1 = mWiseyObjectTypeSpecifier->getType(mContext);
  const IType* type2 = mWiseyObjectTypeSpecifier->getType(mContext);
  
  EXPECT_EQ(type1, type2);
}

TEST_F(WiseyObjectTypeSpecifierTest, printToStreamTest) {
  stringstream stringStream;
  mWiseyObjectTypeSpecifier->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::wisey::object", stringStream.str().c_str());
}
