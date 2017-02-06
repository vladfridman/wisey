//
//  TestLongType.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LongType}
//

#include <gtest/gtest.h>

#include <llvm/IR/LLVMContext.h>

#include "yazyk/LongType.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

TEST(LongTypeTest, LongTypeTest) {
  LLVMContext llvmContext;
  LongType longType;
  
  EXPECT_EQ(longType.getLLVMType(llvmContext), (llvm::Type*) Type::getInt64Ty(llvmContext));
  EXPECT_STREQ(longType.getName().c_str(), "long");
  EXPECT_EQ(longType.getTypeKind(), PRIMITIVE_TYPE);
}
