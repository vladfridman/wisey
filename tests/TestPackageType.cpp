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

#include "TestPrefix.hpp"
#include "IRGenerationContext.hpp"
#include "PackageType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct PackageTypeTest : public Test {
  
  IRGenerationContext mContext;
  string mPackage = "systems.vos.wisey.compiler.tests";
  PackageType* mPackageType;
  
  PackageTypeTest() {
    TestPrefix::generateIR(mContext);
    
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
  EXPECT_FALSE(mPackageType->isPointer());
  EXPECT_FALSE(mPackageType->isPointer());
}

TEST_F(PackageTypeTest, isObjectTest) {
  EXPECT_FALSE(mPackageType->isController());
  EXPECT_FALSE(mPackageType->isInterface());
  EXPECT_FALSE(mPackageType->isModel());
  EXPECT_FALSE(mPackageType->isNode());
}

TEST_F(PackageTypeTest, injectDeathTest) {
  InjectionArgumentList arguments;
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mPackageType->inject(mContext, arguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: type systems.vos.wisey.compiler.tests is not injectable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}
