//
//  TestFloatConstant.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link FloatConstant}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "wisey/FloatConstant.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

TEST(FloatConstantTest, floatConstantTest) {
  string stringBuffer;
  raw_string_ostream* stringStream = new raw_string_ostream(stringBuffer);
  IRGenerationContext context;
  FloatConstant floatConstant(3.5);
  
  Value* irValue = floatConstant.generateIR(context);
  
  *stringStream << *irValue;
  EXPECT_STREQ("float 3.500000e+00", stringStream->str().c_str());
}

TEST(FloatConstantTest, floatConstantTypeTest) {
  IRGenerationContext context;
  FloatConstant floatConstant(3.5);

  EXPECT_EQ(floatConstant.getType(context), PrimitiveTypes::FLOAT_TYPE);
}

TEST(FloatConstantTest, releaseOwnershipDeathTest) {
  IRGenerationContext context;
  FloatConstant floatConstant(3.5);
  
  EXPECT_EXIT(floatConstant.releaseOwnership(context),
              ::testing::ExitedWithCode(1),
              "Error: Can not release ownership of a float constant, it is not a heap pointer");
}
