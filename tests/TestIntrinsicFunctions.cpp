//
//  TestIntrinsicFunctions.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 4/26/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IntrinsicFunctions}

#include <gtest/gtest.h>

#include "wisey/IntrinsicFunctions.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct IntrinsicFunctionsTest : public Test {
  IRGenerationContext mContext;
  Module* mModule;
  
public:
  
  IntrinsicFunctionsTest() {
    mModule = mContext.getModule();
  }
};

TEST_F(IntrinsicFunctionsTest, getThrowFunctionTest) {
  EXPECT_EQ(mModule->getFunction("__cxa_throw"), nullptr);
  EXPECT_NE(IntrinsicFunctions::getThrowFunction(mContext), nullptr);
  EXPECT_NE(mModule->getFunction("__cxa_throw"), nullptr);
}

TEST_F(IntrinsicFunctionsTest, getAllocateExceptionFunctionTest) {
  EXPECT_EQ(mModule->getFunction("__cxa_allocate_exception"), nullptr);
  EXPECT_NE(IntrinsicFunctions::getAllocateExceptionFunction(mContext), nullptr);
  EXPECT_NE(mModule->getFunction("__cxa_allocate_exception"), nullptr);
}

TEST_F(IntrinsicFunctionsTest, getMemCopyFunctionTest) {
  EXPECT_EQ(mModule->getFunction("llvm.memcpy.p0i8.p0i8.i64"), nullptr);
  EXPECT_NE(IntrinsicFunctions::getMemCopyFunction(mContext), nullptr);
  EXPECT_NE(mModule->getFunction("llvm.memcpy.p0i8.p0i8.i64"), nullptr);
}

TEST_F(IntrinsicFunctionsTest, getPersonalityFunctionTest) {
  EXPECT_EQ(mModule->getFunction("__gxx_personality_v0"), nullptr);
  EXPECT_NE(IntrinsicFunctions::getPersonalityFunction(mContext), nullptr);
  EXPECT_NE(mModule->getFunction("__gxx_personality_v0"), nullptr);
}

TEST_F(IntrinsicFunctionsTest, getTypeIdFunctionTest) {
  EXPECT_EQ(mModule->getFunction("llvm.eh.typeid.for"), nullptr);
  EXPECT_NE(IntrinsicFunctions::getTypeIdFunction(mContext), nullptr);
  EXPECT_NE(mModule->getFunction("llvm.eh.typeid.for"), nullptr);
}

TEST_F(IntrinsicFunctionsTest, getUnexpectedFunctionTest) {
  EXPECT_EQ(mModule->getFunction("__cxa_call_unexpected"), nullptr);
  EXPECT_NE(IntrinsicFunctions::getUnexpectedFunction(mContext), nullptr);
  EXPECT_NE(mModule->getFunction("__cxa_call_unexpected"), nullptr);
}

TEST_F(IntrinsicFunctionsTest, getBeginCatchFunctionTest) {
  EXPECT_EQ(mModule->getFunction("__cxa_begin_catch"), nullptr);
  EXPECT_NE(IntrinsicFunctions::getBeginCatchFunction(mContext), nullptr);
  EXPECT_NE(mModule->getFunction("__cxa_begin_catch"), nullptr);
}

TEST_F(IntrinsicFunctionsTest, getEndCatchFunctionTest) {
  EXPECT_EQ(mModule->getFunction("__cxa_end_catch"), nullptr);
  EXPECT_NE(IntrinsicFunctions::getEndCatchFunction(mContext), nullptr);
  EXPECT_NE(mModule->getFunction("__cxa_end_catch"), nullptr);
}

TEST_F(IntrinsicFunctionsTest, getPrintfFunctionTest) {
  EXPECT_EQ(mModule->getFunction("printf"), nullptr);
  EXPECT_NE(IntrinsicFunctions::getPrintfFunction(mContext), nullptr);
  EXPECT_NE(mModule->getFunction("printf"), nullptr);
}

