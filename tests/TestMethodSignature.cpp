//
//  TestMethodSignature.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link MethodSignature}
//

#include <gtest/gtest.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/MethodSignature.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct MethodSignatureTest : Test {
  IRGenerationContext mContext;
  MethodSignature* mMethodSignature;
  
  MethodSignatureTest() {
    vector<MethodArgument*> arguments;
    vector<const Model*> thrownExceptions;
    mMethodSignature = new MethodSignature("foo",
                                           PrimitiveTypes::LONG_TYPE,
                                           arguments,
                                           thrownExceptions);
  }
};

TEST_F(MethodSignatureTest, methodSignatureTest) {
  EXPECT_STREQ(mMethodSignature->getName().c_str(), "foo");
  EXPECT_EQ(mMethodSignature->getReturnType(), PrimitiveTypes::LONG_TYPE);
  EXPECT_EQ(mMethodSignature->getArguments().size(), 0u);
}

TEST_F(MethodSignatureTest, createCopyTest) {
  MethodSignature* copy = mMethodSignature->createCopy();
  
  EXPECT_STREQ(copy->getName().c_str(), "foo");
  EXPECT_EQ(copy->getReturnType(), PrimitiveTypes::LONG_TYPE);
  EXPECT_EQ(copy->getArguments().size(), 0u);
}

TEST_F(MethodSignatureTest, printToStreamTest) {
  stringstream stringStream;
  mMethodSignature->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("  long foo();\n", stringStream.str().c_str());
}
