//
//  TestIntType.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IntType}
//

#include <gtest/gtest.h>

#include <llvm/IR/LLVMContext.h>

#include "yazyk/IntType.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

TEST(IntTypeTest, IntTypeTest) {
  LLVMContext llvmContext;
  IntType intType;
  
  EXPECT_EQ(intType.getLLVMType(llvmContext), (llvm::Type*) Type::getInt32Ty(llvmContext));
  EXPECT_STREQ(intType.getName().c_str(), "int");
}
