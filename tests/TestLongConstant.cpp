//
//  TestLong.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Long}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "yazyk/LongConstant.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

TEST(LongConstantTest, LongConstantTest) {
  string stringBuffer;
  raw_string_ostream* stringStream = new raw_string_ostream(stringBuffer);
  IRGenerationContext context;
  LongConstant constantLong(5);
  
  Value* irValue = constantLong.generateIR(context);
  
  *stringStream << *irValue;
  EXPECT_STREQ("i64 5", stringStream->str().c_str());
}

TEST(LongConstantTest, TestLongConstantType) {
  IRGenerationContext context;
  LongConstant constantLong(5);

  EXPECT_EQ(constantLong.getType(context), PrimitiveTypes::LONG_TYPE);
}

TEST(LongConstantTest, releaseOwnershipDeathTest) {
  IRGenerationContext context;
  LongConstant constantLong(5);
  
  EXPECT_EXIT(constantLong.releaseOwnership(context),
              ::testing::ExitedWithCode(1),
              "Error: Can not release ownership of a long constant, it is not a heap pointer");
}
