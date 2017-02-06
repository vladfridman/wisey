//
//  TestCharConstant.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link CharConstant}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/CharConstant.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

TEST(CharConstantTest, CharConstantTest) {
  string stringBuffer;
  raw_string_ostream* stringStream = new raw_string_ostream(stringBuffer);
  IRGenerationContext context;
  CharConstant charConstant('y');
  
  Value* irValue = charConstant.generateIR(context);

  *stringStream << *irValue;
  EXPECT_STREQ("i8 121", stringStream->str().c_str());
}

TEST(CharConstantTest, TestCharConstantType) {
  CharConstant charConstant('y');

  EXPECT_EQ(charConstant.getType(), PrimitiveTypes::CHAR_TYPE);
}

TEST_F(TestFileSampleRunner, CharVariableTest) {
  runFile("tests/samples/test_char_variable.yz", "7");
}
