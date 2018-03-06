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

TEST_F(UndefinedTypeTest, isPrimitiveTest) {
  EXPECT_FALSE(UndefinedType::UNDEFINED_TYPE->isPrimitive());
}

TEST_F(UndefinedTypeTest, isOwnerTest) {
  EXPECT_FALSE(UndefinedType::UNDEFINED_TYPE->isOwner());
}

TEST_F(UndefinedTypeTest, isReferenceTest) {
  EXPECT_FALSE(UndefinedType::UNDEFINED_TYPE->isReference());
}

TEST_F(UndefinedTypeTest, isArrayTest) {
  EXPECT_FALSE(UndefinedType::UNDEFINED_TYPE->isArray());
}

TEST_F(UndefinedTypeTest, isFunctionTest) {
  EXPECT_FALSE(UndefinedType::UNDEFINED_TYPE->isFunction());
}

TEST_F(UndefinedTypeTest, isPackageTest) {
  EXPECT_FALSE(UndefinedType::UNDEFINED_TYPE->isPackage());
}

TEST_F(UndefinedTypeTest, isObjectTest) {
  EXPECT_FALSE(UndefinedType::UNDEFINED_TYPE->isController());
  EXPECT_FALSE(UndefinedType::UNDEFINED_TYPE->isInterface());
  EXPECT_FALSE(UndefinedType::UNDEFINED_TYPE->isModel());
  EXPECT_FALSE(UndefinedType::UNDEFINED_TYPE->isNode());
  EXPECT_FALSE(UndefinedType::UNDEFINED_TYPE->isThread());
}
