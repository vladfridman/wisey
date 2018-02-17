//
//  TestUndefinedType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 12/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link UndefinedType}
//

#include <gtest/gtest.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/UndefinedType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct UndefinedTypeTest : public Test {
  
  IRGenerationContext mContext;
  
  UndefinedTypeTest() {
  }
};

TEST_F(UndefinedTypeTest, getNameTest) {
  ASSERT_STREQ("undefined", UndefinedType::UNDEFINED_TYPE->getTypeName().c_str());
}

TEST_F(UndefinedTypeTest, getLLVMTypeTest) {
  ASSERT_EQ(nullptr, UndefinedType::UNDEFINED_TYPE->getLLVMType(mContext));
}

TEST_F(UndefinedTypeTest, getTypeKindTest) {
  ASSERT_EQ(UNDEFINED_TYPE_KIND, UndefinedType::UNDEFINED_TYPE->getTypeKind());
}

TEST_F(UndefinedTypeTest, isOwnerTest) {
  EXPECT_FALSE(UndefinedType::UNDEFINED_TYPE->isOwner());
}

TEST_F(UndefinedTypeTest, isReferenceTest) {
  EXPECT_FALSE(UndefinedType::UNDEFINED_TYPE->isReference());
}
