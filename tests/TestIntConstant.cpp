//
//  TestIntConstant.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IntConstant}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "yazyk/IntConstant.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

TEST(IntConstantTest, IntConstantTest) {
  string stringBuffer;
  raw_string_ostream* stringStream = new raw_string_ostream(stringBuffer);
  IRGenerationContext context;
  IntConstant intConstant(5);
  
  Value* irValue = intConstant.generateIR(context);
  
  *stringStream << *irValue;
  EXPECT_STREQ("i32 5", stringStream->str().c_str());
}

TEST(IntConstantTest, TestIntConstantType) {
  IRGenerationContext context;
  IntConstant intConstant(5);

  EXPECT_EQ(intConstant.getType(context), PrimitiveTypes::INT_TYPE);
}
