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
#include "wisey/CharConstant.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

TEST(CharConstantTest, charConstantTest) {
  string stringBuffer;
  raw_string_ostream* stringStream = new raw_string_ostream(stringBuffer);
  IRGenerationContext context;
  CharConstant charConstant('y');
  
  Value* irValue = charConstant.generateIR(context);

  *stringStream << *irValue;
  EXPECT_STREQ("i16 121", stringStream->str().c_str());
}

TEST(CharConstantTest, charConstantTypeTest) {
  IRGenerationContext context;
  CharConstant charConstant('y');

  EXPECT_EQ(charConstant.getType(context), PrimitiveTypes::CHAR_TYPE);
}

TEST(CharConstantTest, releaseOwnershipDeathTest) {
  IRGenerationContext context;
  CharConstant charConstant('y');
  
  EXPECT_EXIT(charConstant.releaseOwnership(context),
              ::testing::ExitedWithCode(1),
              "Error: Can not release ownership of a char constant, it is not a heap pointer");
}

TEST_F(TestFileSampleRunner, charVariableRunTest) {
  runFile("tests/samples/test_char_variable.yz", "7");
}
