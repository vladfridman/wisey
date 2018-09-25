//
//  TestConstantNegate.cpp
//  runtests
//
//  Created by Vladimir Fridman on 8/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ConstantNegate}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "ConstantNegate.hpp"
#include "FloatConstant.hpp"
#include "IRGenerationContext.hpp"
#include "PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

struct ConstantNegateTest : public ::testing::Test {
  IRGenerationContext mContext;
  ConstantNegate* mConstantNegate;
  
  ConstantNegateTest() {
    FloatConstant* floatConstant = new FloatConstant(3.5, 0);
    mConstantNegate = new ConstantNegate(floatConstant, 11);
  }
  
  ~ConstantNegateTest() {
    delete mConstantNegate;
  }
};

TEST_F(ConstantNegateTest, generateIRTest) {
  string stringBuffer;
  raw_string_ostream* stringStream = new raw_string_ostream(stringBuffer);
  IRGenerationContext context;
  
  Value* irValue = mConstantNegate->generateIR(mContext, PrimitiveTypes::VOID);
  
  *stringStream << *irValue;
  EXPECT_STREQ("float -3.500000e+00", stringStream->str().c_str());
}

TEST_F(ConstantNegateTest, getTypeTest) {
  EXPECT_EQ(mConstantNegate->getType(mContext), PrimitiveTypes::FLOAT);
}

TEST_F(ConstantNegateTest, isConstantTest) {
  EXPECT_TRUE(mConstantNegate->isConstant());
}

TEST_F(ConstantNegateTest, isAssignableTest) {
  EXPECT_FALSE(mConstantNegate->isAssignable());
}

TEST_F(ConstantNegateTest, printToStreamTest) {
  stringstream stringStream;
  mConstantNegate->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("-3.5", stringStream.str().c_str());
}

