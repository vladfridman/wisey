//
//  TestLong.cpp
//  Wisey
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

#include "LongConstant.hpp"
#include "IRGenerationContext.hpp"
#include "PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

struct LongConstantTest : public ::testing::Test {
  IRGenerationContext mContext;
  LongConstant mLongConstant;

  LongConstantTest() : mLongConstant(5, 0) { }
};

TEST_F(LongConstantTest, longConstantTest) {
  string stringBuffer;
  raw_string_ostream* stringStream = new raw_string_ostream(stringBuffer);
  
  Value* irValue = mLongConstant.generateIR(mContext, PrimitiveTypes::VOID);
  
  *stringStream << *irValue;
  EXPECT_STREQ("i64 5", stringStream->str().c_str());
}

TEST_F(LongConstantTest, longConstantTypeTest) {
  EXPECT_EQ(mLongConstant.getType(mContext), PrimitiveTypes::LONG);
}

TEST_F(LongConstantTest, isConstantTest) {
  EXPECT_TRUE(mLongConstant.isConstant());
}

TEST_F(LongConstantTest, isAssignableTest) {
  EXPECT_FALSE(mLongConstant.isAssignable());
}

TEST_F(LongConstantTest, printToStreamTest) {
  stringstream stringStream;
  mLongConstant.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("5l", stringStream.str().c_str());
}

