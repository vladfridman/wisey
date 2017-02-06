//
//  TestBooleanType.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link BooleanType}
//

#include <gtest/gtest.h>

#include <llvm/IR/LLVMContext.h>

#include "yazyk/BooleanType.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

TEST(BooleanType, BooleanType) {
  LLVMContext llvmContext;
  BooleanType booleanType;
  
  EXPECT_EQ(booleanType.getLLVMType(llvmContext), (llvm::Type*) Type::getInt1Ty(llvmContext));
  EXPECT_STREQ(booleanType.getName().c_str(), "boolean");
  EXPECT_EQ(booleanType.getTypeKind(), PRIMITIVE_TYPE);
}
