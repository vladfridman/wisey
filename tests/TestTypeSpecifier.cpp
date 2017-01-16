//
//  TestTypeSpecifier.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/18/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link TypeSpecifier}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "yazyk/TypeSpecifier.hpp"

using namespace llvm;
using namespace yazyk;

TEST(TypeSpecifierTest, CreationTest) {
  IRGenerationContext context;
  LLVMContext& llvmContext = context.getLLVMContext();
  TypeSpecifier typeSpecifier(PRIMITIVE_TYPE_INT32);
  
  EXPECT_EQ(typeSpecifier.getType(llvmContext), Type::getInt32Ty(llvmContext));
}
