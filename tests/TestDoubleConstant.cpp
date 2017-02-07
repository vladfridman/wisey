//
//  TestDoubleConstant.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link DoubleConstant}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "yazyk/DoubleConstant.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

TEST(DoubleConstantTest, DoubleConstantTest) {
  string stringBuffer;
  raw_string_ostream* stringStream = new raw_string_ostream(stringBuffer);
  IRGenerationContext context;
  DoubleConstant doubleConstant(5.7);
  
  Value* irValue = doubleConstant.generateIR(context);
  
  *stringStream << *irValue;
  EXPECT_STREQ("double 5.700000e+00", stringStream->str().c_str());
}

TEST(DoubleConstantTest, TestDoubleConstantType) {
  IRGenerationContext context;
  DoubleConstant doubleConstant(5.7);

  EXPECT_EQ(doubleConstant.getType(context), PrimitiveTypes::DOUBLE_TYPE);
}
