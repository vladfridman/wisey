//
//  TestLLVMObjectTypeSpecifier.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/2/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMObjectTypeSpecifier}
//

#include <gtest/gtest.h>

#include "wisey/LLVMObjectTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

using ::testing::Test;

struct LLVMObjectTypeSpecifierTest : public Test {
  IRGenerationContext mContext;
  LLVMObjectTypeSpecifier* mLLVMObjectTypeSpecifier;
  
  LLVMObjectTypeSpecifierTest() {
    mLLVMObjectTypeSpecifier = new LLVMObjectTypeSpecifier();
  }
};

TEST_F(LLVMObjectTypeSpecifierTest, getTypeTest) {
  const IType* type = mLLVMObjectTypeSpecifier->getType(mContext);
  
  EXPECT_TRUE(type->isNative());
  EXPECT_TRUE(type->isReference());
  EXPECT_FALSE(type->isOwner());
  EXPECT_STREQ("::wisey::object", type->getTypeName().c_str());
}

TEST_F(LLVMObjectTypeSpecifierTest, twoGetsReturnSameTypeObjectTest) {
  const IType* type1 = mLLVMObjectTypeSpecifier->getType(mContext);
  const IType* type2 = mLLVMObjectTypeSpecifier->getType(mContext);
  
  EXPECT_EQ(type1, type2);
}

TEST_F(LLVMObjectTypeSpecifierTest, printToStreamTest) {
  stringstream stringStream;
  mLLVMObjectTypeSpecifier->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::wisey::object", stringStream.str().c_str());
}
