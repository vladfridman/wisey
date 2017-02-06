//
//  TestDoubleType.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link DoubleType}
//

#include <gtest/gtest.h>

#include <llvm/IR/LLVMContext.h>

#include "yazyk/DoubleType.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

TEST(DoubleTypeTest, DoubleTypeTest) {
  LLVMContext llvmContext;
  DoubleType doubleType;
  
  EXPECT_EQ(doubleType.getLLVMType(llvmContext), Type::getDoubleTy(llvmContext));
  EXPECT_STREQ(doubleType.getName().c_str(), "double");
  EXPECT_EQ(doubleType.getTypeKind(), PRIMITIVE_TYPE);
}
