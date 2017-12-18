//
//  TestIntConstant.cpp
//  Wisey
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

#include "wisey/IntConstant.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

struct IntConstantTest : public ::testing::Test {
  IRGenerationContext mContext;
  IntConstant mIntConstant;

  IntConstantTest() : mIntConstant(5) { }
};

TEST_F(IntConstantTest, getVariableTest) {
  vector<const IExpression*> arrayIndices;
  EXPECT_EQ(mIntConstant.getVariable(mContext, arrayIndices), nullptr);
}

TEST_F(IntConstantTest, intConstantTest) {
  string stringBuffer;
  raw_string_ostream* stringStream = new raw_string_ostream(stringBuffer);
  IRGenerationContext context;
  
  Value* irValue = mIntConstant.generateIR(mContext, IR_GENERATION_NORMAL);
  
  *stringStream << *irValue;
  EXPECT_STREQ("i32 5", stringStream->str().c_str());
}

TEST_F(IntConstantTest, intConstantTypeTest) {

  EXPECT_EQ(mIntConstant.getType(mContext), PrimitiveTypes::INT_TYPE);
}

TEST_F(IntConstantTest, isConstantTest) {
  EXPECT_TRUE(mIntConstant.isConstant());
}

TEST_F(IntConstantTest, printToStreamTest) {
  stringstream stringStream;
  mIntConstant.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("5", stringStream.str().c_str());
}

