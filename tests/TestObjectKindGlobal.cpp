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

#include "wisey/IRGenerationContext.hpp"
#include "wisey/ObjectKindGlobal.hpp"

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
  EXPECT_EQ(nullptr, mContext.getModule()->getGlobalVariable("controller"));
  ObjectKindGlobal::getController(mContext);
  EXPECT_NE(nullptr, mContext.getModule()->getGlobalVariable("controller"));
}

TEST_F(ObjectKindGlobalTest, getModel) {
  EXPECT_EQ(nullptr, mContext.getModule()->getGlobalVariable("model"));
  ObjectKindGlobal::getModel(mContext);
  EXPECT_NE(nullptr, mContext.getModule()->getGlobalVariable("model"));
}

TEST_F(ObjectKindGlobalTest, getNode) {
  EXPECT_EQ(nullptr, mContext.getModule()->getGlobalVariable("node"));
  ObjectKindGlobal::getNode(mContext);
  EXPECT_NE(nullptr, mContext.getModule()->getGlobalVariable("node"));
}
