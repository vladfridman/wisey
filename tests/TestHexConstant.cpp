//
//  TestHexConstant.cpp
//  runtests
//
//  Created by Vladimir Fridman on 8/6/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link HexConstant}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "HexConstant.hpp"
#include "IRGenerationContext.hpp"
#include "PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

struct HexConstantTest : public ::testing::Test {
  IRGenerationContext mContext;
  HexConstant mHexConstant;
  
  HexConstantTest() : mHexConstant(11ul, 0) { }
};

TEST_F(HexConstantTest, HexConstantTest) {
  string stringBuffer;
  raw_string_ostream* stringStream = new raw_string_ostream(stringBuffer);
  IRGenerationContext context;
  
  Value* irValue = mHexConstant.generateIR(mContext, PrimitiveTypes::VOID);
  
  *stringStream << *irValue;
  EXPECT_STREQ("i64 11", stringStream->str().c_str());
}

TEST_F(HexConstantTest, HexConstantTypeTest) {
  
  EXPECT_EQ(mHexConstant.getType(mContext), PrimitiveTypes::LONG);
}

TEST_F(HexConstantTest, isConstantTest) {
  EXPECT_TRUE(mHexConstant.isConstant());
}

TEST_F(HexConstantTest, isAssignableTest) {
  EXPECT_FALSE(mHexConstant.isAssignable());
}

TEST_F(HexConstantTest, printToStreamTest) {
  stringstream stringStream;
  mHexConstant.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("0xb", stringStream.str().c_str());
}
