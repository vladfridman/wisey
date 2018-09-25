//
//  TestObjectKindGlobal.cpp
//  runtests
//
//  Created by Vladimir Fridman on 5/7/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ObjectKindGlobal}
//

#include <gtest/gtest.h>

#include "IRGenerationContext.hpp"
#include "ObjectKindGlobal.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

using ::testing::Test;

struct ObjectKindGlobalTest : public Test {
  IRGenerationContext mContext;
  
  ObjectKindGlobalTest() {
  }
};

TEST_F(ObjectKindGlobalTest, getController) {
  EXPECT_EQ(nullptr, mContext.getModule()->getNamedGlobal("controller"));
  ObjectKindGlobal::getController(mContext);
  EXPECT_NE(nullptr, mContext.getModule()->getNamedGlobal("controller"));
}

TEST_F(ObjectKindGlobalTest, getModel) {
  EXPECT_EQ(nullptr, mContext.getModule()->getNamedGlobal("model"));
  ObjectKindGlobal::getModel(mContext);
  EXPECT_NE(nullptr, mContext.getModule()->getNamedGlobal("model"));
}

TEST_F(ObjectKindGlobalTest, getNode) {
  EXPECT_EQ(nullptr, mContext.getModule()->getNamedGlobal("node"));
  ObjectKindGlobal::getNode(mContext);
  EXPECT_NE(nullptr, mContext.getModule()->getNamedGlobal("node"));
}
