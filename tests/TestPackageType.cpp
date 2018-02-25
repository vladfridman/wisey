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

TEST_F(PackageTypeTest, getTypeKindTest) {
  ASSERT_EQ(PACKAGE_TYPE, mPackageType->getTypeKind());
}

TEST_F(PackageTypeTest, isPrimitiveTest) {
  EXPECT_FALSE(mPackageType->isPrimitive());
}

TEST_F(PackageTypeTest, isOwnerTest) {
  EXPECT_FALSE(mPackageType->isOwner());
}

TEST_F(PackageTypeTest, isReferenceTest) {
  EXPECT_FALSE(mPackageType->isReference());
}

TEST_F(PackageTypeTest, isArrayTest) {
  EXPECT_FALSE(mPackageType->isArray());
}

TEST_F(PackageTypeTest, isFunctionTest) {
  EXPECT_FALSE(mPackageType->isFunction());
}

TEST_F(PackageTypeTest, isPackageTest) {
  EXPECT_TRUE(mPackageType->isPackage());
}

TEST_F(PackageTypeTest, isObjectTest) {
  EXPECT_FALSE(mPackageType->isController());
  EXPECT_FALSE(mPackageType->isInterface());
  EXPECT_FALSE(mPackageType->isModel());
  EXPECT_FALSE(mPackageType->isNode());
}
