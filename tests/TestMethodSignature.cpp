//
//  TestMethodSignature.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link MethodSignature}
//

#include <gtest/gtest.h>

#include "wisey/MethodSignature.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct MethodSignatureTest : Test {
  MethodSignature* mMethodSignature;
  
  MethodSignatureTest() {
    vector<MethodArgument*> arguments;
    vector<IType*> thrownExceptions;
    mMethodSignature = new MethodSignature("foo",
                                           AccessLevel::PUBLIC_ACCESS,
                                           PrimitiveTypes::LONG_TYPE,
                                           arguments,
                                           thrownExceptions,
                                           1);
  }
};

TEST_F(MethodSignatureTest, methodSignatureTest) {
  EXPECT_STREQ(mMethodSignature->getName().c_str(), "foo");
  EXPECT_EQ(mMethodSignature->getReturnType(), PrimitiveTypes::LONG_TYPE);
  EXPECT_EQ(mMethodSignature->getArguments().size(), 0u);
  EXPECT_EQ(mMethodSignature->getIndex(), 1u);
}

TEST_F(MethodSignatureTest, createCopyWithIndexTest) {
  MethodSignature* copy = mMethodSignature->createCopyWithIndex(55);
  
  EXPECT_STREQ(copy->getName().c_str(), "foo");
  EXPECT_EQ(copy->getReturnType(), PrimitiveTypes::LONG_TYPE);
  EXPECT_EQ(copy->getArguments().size(), 0u);
  EXPECT_EQ(copy->getIndex(), 55u);
}
