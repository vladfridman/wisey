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

struct FloatConstantTest : public ::testing::Test {
  IRGenerationContext mContext;
  FloatConstant mFloatConstant;

  FloatConstantTest() : mFloatConstant(3.5, 0) { }
};

TEST_F(FloatConstantTest, floatConstantTest) {
  string stringBuffer;
  raw_string_ostream* stringStream = new raw_string_ostream(stringBuffer);
  IRGenerationContext context;
  
  Value* irValue = mFloatConstant.generateIR(mContext, PrimitiveTypes::VOID);
  
  *stringStream << *irValue;
  EXPECT_STREQ("float 3.500000e+00", stringStream->str().c_str());
}

TEST_F(FloatConstantTest, floatConstantTypeTest) {
  EXPECT_EQ(mFloatConstant.getType(mContext), PrimitiveTypes::FLOAT);
}

TEST_F(FloatConstantTest, isConstantTest) {
  EXPECT_TRUE(mFloatConstant.isConstant());
}

TEST_F(FloatConstantTest, isAssignableTest) {
  EXPECT_FALSE(mFloatConstant.isAssignable());
}

TEST_F(FloatConstantTest, printToStreamTest) {
  stringstream stringStream;
  mFloatConstant.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("3.5", stringStream.str().c_str());
}

