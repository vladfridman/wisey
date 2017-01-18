//
//  TestModelTypeSpecifier.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/18/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ModelTypeSpecifier}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "yazyk/ModelTypeSpecifier.hpp"

using namespace llvm;
using namespace yazyk;

TEST(ModelTypeSpecifierTest, CreationTest) {
  ModelTypeSpecifier modelTypeSpecifier("a");
  
  EXPECT_EQ(modelTypeSpecifier.getName(), "model.a");
  EXPECT_EQ(modelTypeSpecifier.getStorageType(), HEAP_VARIABLE);
}
