//
//  TestWiseyModelTypeSpecifier.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link WiseyModelTypeSpecifier}
//

#include <gtest/gtest.h>

#include "WiseyModelTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

using ::testing::Test;

struct WiseyModelTypeSpecifierTest : public Test {
  IRGenerationContext mContext;
  WiseyModelTypeSpecifier* mWiseyModelTypeSpecifier;
  
  WiseyModelTypeSpecifierTest() {
    mWiseyModelTypeSpecifier = new WiseyModelTypeSpecifier(1);
  }
};

TEST_F(WiseyModelTypeSpecifierTest, getTypeTest) {
  const IType* type = mWiseyModelTypeSpecifier->getType(mContext);
  
  EXPECT_TRUE(type->isNative());
  EXPECT_TRUE(type->isReference());
  EXPECT_FALSE(type->isOwner());
  EXPECT_STREQ("::wisey::model", type->getTypeName().c_str());
  EXPECT_EQ(1, mWiseyModelTypeSpecifier->getLine());
}

TEST_F(WiseyModelTypeSpecifierTest, twoGetsReturnSameTypeObjectTest) {
  const IType* type1 = mWiseyModelTypeSpecifier->getType(mContext);
  const IType* type2 = mWiseyModelTypeSpecifier->getType(mContext);
  
  EXPECT_EQ(type1, type2);
}

TEST_F(WiseyModelTypeSpecifierTest, printToStreamTest) {
  stringstream stringStream;
  mWiseyModelTypeSpecifier->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::wisey::model", stringStream.str().c_str());
}
