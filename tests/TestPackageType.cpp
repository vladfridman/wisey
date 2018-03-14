//
//  TestPackageType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 12/14/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link PackageType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/PackageType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct PackageTypeTest : public Test {
  
  IRGenerationContext mContext;
  string mPackage = "systems.vos.wisey.compiler.tests";
  PackageType* mPackageType;
  
  PackageTypeTest() {
    mPackageType = new PackageType(mPackage);
  }
};

TEST_F(PackageTypeTest, getTypeNameTest) {
  ASSERT_STREQ("systems.vos.wisey.compiler.tests", mPackageType->getTypeName().c_str());
}

TEST_F(PackageTypeTest, getLLVMTypeTest) {
  ASSERT_EQ(nullptr, mPackageType->getLLVMType(mContext));
}

TEST_F(PackageTypeTest, isTypeKindTest) {
  EXPECT_FALSE(mPackageType->isPrimitive());
  EXPECT_FALSE(mPackageType->isOwner());
  EXPECT_FALSE(mPackageType->isReference());
  EXPECT_FALSE(mPackageType->isArray());
  EXPECT_FALSE(mPackageType->isFunction());
  EXPECT_TRUE(mPackageType->isPackage());
  EXPECT_FALSE(mPackageType->isNative());
}

TEST_F(PackageTypeTest, isObjectTest) {
  EXPECT_FALSE(mPackageType->isController());
  EXPECT_FALSE(mPackageType->isInterface());
  EXPECT_FALSE(mPackageType->isModel());
  EXPECT_FALSE(mPackageType->isNode());
}
