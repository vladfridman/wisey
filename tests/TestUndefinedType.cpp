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
#include "IRGenerationContext.hpp"
#include "UndefinedType.hpp"

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
  ASSERT_STREQ("undefined", UndefinedType::UNDEFINED->getTypeName().c_str());
}

TEST_F(UndefinedTypeTest, getLLVMTypeTest) {
  ASSERT_EQ(nullptr, UndefinedType::UNDEFINED->getLLVMType(mContext));
}

TEST_F(UndefinedTypeTest, isTypeKindTest) {
  EXPECT_FALSE(UndefinedType::UNDEFINED->isPrimitive());
  EXPECT_FALSE(UndefinedType::UNDEFINED->isOwner());
  EXPECT_FALSE(UndefinedType::UNDEFINED->isReference());
  EXPECT_FALSE(UndefinedType::UNDEFINED->isArray());
  EXPECT_FALSE(UndefinedType::UNDEFINED->isFunction());
  EXPECT_FALSE(UndefinedType::UNDEFINED->isPackage());
  EXPECT_FALSE(UndefinedType::UNDEFINED->isNative());
  EXPECT_FALSE(UndefinedType::UNDEFINED->isPointer());
  EXPECT_FALSE(UndefinedType::UNDEFINED->isPointer());
}

TEST_F(UndefinedTypeTest, isObjectTest) {
  EXPECT_FALSE(UndefinedType::UNDEFINED->isController());
  EXPECT_FALSE(UndefinedType::UNDEFINED->isInterface());
  EXPECT_FALSE(UndefinedType::UNDEFINED->isModel());
  EXPECT_FALSE(UndefinedType::UNDEFINED->isNode());
}

TEST_F(UndefinedTypeTest, injectDeathTest) {
  InjectionArgumentList arguments;
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(UndefinedType::UNDEFINED->inject(mContext, arguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: type undefined is not injectable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}
