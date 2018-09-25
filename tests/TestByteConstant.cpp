//
//  TestByteConstant.cpp
//  runtests
//
//  Created by Vladimir Fridman on 8/10/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ByteConstant}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileRunner.hpp"
#include "ByteConstant.hpp"
#include "IRGenerationContext.hpp"
#include "PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

struct ByteConstantTest : public ::testing::Test {
  IRGenerationContext mContext;
  ByteConstant mByteConstant;
  
  ByteConstantTest() : mByteConstant(3, 0) { }
};

TEST_F(ByteConstantTest, byteConstantTest) {
  string stringBuffer;
  raw_string_ostream* stringStream = new raw_string_ostream(stringBuffer);
  IRGenerationContext context;
  
  Value* irValue = mByteConstant.generateIR(mContext, PrimitiveTypes::VOID);
  
  *stringStream << *irValue;
  EXPECT_STREQ("i8 3", stringStream->str().c_str());
}

TEST_F(ByteConstantTest, byteConstantTypeTest) {
  
  EXPECT_EQ(mByteConstant.getType(mContext), PrimitiveTypes::BYTE);
}

TEST_F(ByteConstantTest, isConstantTest) {
  EXPECT_TRUE(mByteConstant.isConstant());
}

TEST_F(ByteConstantTest, isAssignableTest) {
  EXPECT_FALSE(mByteConstant.isAssignable());
}

TEST_F(ByteConstantTest, printToStreamTest) {
  stringstream stringStream;
  mByteConstant.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("3", stringStream.str().c_str());
}

TEST_F(TestFileRunner, signedCastByteToIntRunTest) {
  runFile("tests/samples/test_signed_cast_byte_to_int.yz", -1);
}
