//
//  TestFloatType.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link FloatType}
//

#include <gtest/gtest.h>

#include <llvm/IR/LLVMContext.h>

#include "yazyk/FloatType.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

TEST(FloatTypeTest, FloatTypeTest) {
  LLVMContext llvmContext;
  FloatType floatType;
  
  EXPECT_EQ(floatType.getLLVMType(llvmContext), Type::getFloatTy(llvmContext));
  EXPECT_STREQ(floatType.getName().c_str(), "float");
}
