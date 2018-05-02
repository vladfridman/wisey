//
//  TestDoubleConstant.cpp
//  Wisey
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

#include "wisey/DoubleConstant.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

struct DoubleConstantTest : public ::testing::Test {
  IRGenerationContext mContext;
  DoubleConstant mDoubleConstant;
  
  DoubleConstantTest() : mDoubleConstant(5.7, 0) { }
};

TEST_F(DoubleConstantTest, doubleConstantTest) {
  string stringBuffer;
  raw_string_ostream* stringStream = new raw_string_ostream(stringBuffer);
  
  Value* irValue = mDoubleConstant.generateIR(mContext, PrimitiveTypes::VOID);
  
  *stringStream << *irValue;
  EXPECT_STREQ("double 5.700000e+00", stringStream->str().c_str());
}

TEST_F(DoubleConstantTest, doubleConstantTypeTest) {
  EXPECT_EQ(mDoubleConstant.getType(mContext), PrimitiveTypes::DOUBLE);
}

TEST_F(DoubleConstantTest, isConstantTest) {
  EXPECT_TRUE(mDoubleConstant.isConstant());
}

TEST_F(DoubleConstantTest, isAssignableTest) {
  EXPECT_FALSE(mDoubleConstant.isAssignable());
}

TEST_F(DoubleConstantTest, printToStreamTest) {
  stringstream stringStream;
  mDoubleConstant.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("5.7d", stringStream.str().c_str());
}

