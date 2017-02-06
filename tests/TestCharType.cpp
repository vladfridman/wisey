//
//  TestCharType.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link CharType}
//

#include <gtest/gtest.h>

#include <llvm/IR/LLVMContext.h>

#include "yazyk/CharType.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

TEST(CharTypeTest, CharTypeTest) {
  LLVMContext llvmContext;
  CharType charType;
  
  EXPECT_EQ(charType.getLLVMType(llvmContext), (llvm::Type*) Type::getInt8Ty(llvmContext));
  EXPECT_STREQ(charType.getName().c_str(), "char");
}
