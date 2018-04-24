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

#include "TestPrefix.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/UndefinedType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct UndefinedTypeTest : public Test {
  
  IRGenerationContext mContext;
  
  UndefinedTypeTest() {
    TestPrefix::generateIR(mContext);
  }
};

TEST_F(UndefinedTypeTest, getNameTest) {
  ASSERT_STREQ("undefined", UndefinedType::UNDEFINED_TYPE->getTypeName().c_str());
}

TEST_F(UndefinedTypeTest, getLLVMTypeTest) {
  ASSERT_EQ(nullptr, UndefinedType::UNDEFINED_TYPE->getLLVMType(mContext));
}

TEST_F(UndefinedTypeTest, isTypeKindTest) {
  EXPECT_FALSE(UndefinedType::UNDEFINED_TYPE->isPrimitive());
  EXPECT_FALSE(UndefinedType::UNDEFINED_TYPE->isOwner());
  EXPECT_FALSE(UndefinedType::UNDEFINED_TYPE->isReference());
  EXPECT_FALSE(UndefinedType::UNDEFINED_TYPE->isArray());
  EXPECT_FALSE(UndefinedType::UNDEFINED_TYPE->isFunction());
  EXPECT_FALSE(UndefinedType::UNDEFINED_TYPE->isPackage());
  EXPECT_FALSE(UndefinedType::UNDEFINED_TYPE->isNative());
  EXPECT_FALSE(UndefinedType::UNDEFINED_TYPE->isPointer());
  EXPECT_FALSE(UndefinedType::UNDEFINED_TYPE->isPointer());
}

TEST_F(UndefinedTypeTest, isObjectTest) {
  EXPECT_FALSE(UndefinedType::UNDEFINED_TYPE->isController());
  EXPECT_FALSE(UndefinedType::UNDEFINED_TYPE->isInterface());
  EXPECT_FALSE(UndefinedType::UNDEFINED_TYPE->isModel());
  EXPECT_FALSE(UndefinedType::UNDEFINED_TYPE->isNode());
}

TEST_F(UndefinedTypeTest, injectDeathTest) {
  InjectionArgumentList arguments;
  EXPECT_EXIT(UndefinedType::UNDEFINED_TYPE->inject(mContext, arguments, 3),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(3\\): Error: type undefined is not injectable");
}
