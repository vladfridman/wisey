//
//  TestVoidType.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link VoidType}
//

#include <gtest/gtest.h>

#include <llvm/IR/LLVMContext.h>

#include "yazyk/VoidType.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

TEST(VoidTypeTest, VoidTypeTest) {
  LLVMContext llvmContext;
  VoidType voidType;
  
  EXPECT_EQ(voidType.getLLVMType(llvmContext), Type::getVoidTy(llvmContext));
  EXPECT_STREQ(voidType.getName().c_str(), "void");
}
