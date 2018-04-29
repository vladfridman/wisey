//
//  TestIntrinsicFunctions.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/26/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IntrinsicFunctions}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IntrinsicFunctions.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct IntrinsicFunctionsTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Module* mModule;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;

public:
  
  IntrinsicFunctionsTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mModule(mContext.getModule()) {
    TestPrefix::generateIR(mContext);
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
};

TEST_F(IntrinsicFunctionsTest, getPrintfFunctionTest) {
  EXPECT_NE(IntrinsicFunctions::getPrintfFunction(mContext), nullptr);
}

TEST_F(IntrinsicFunctionsTest, getFprintfFunctionTest) {
  EXPECT_NE(IntrinsicFunctions::getFprintfFunction(mContext), nullptr);
}

TEST_F(IntrinsicFunctionsTest, getThrowFunctionTest) {
  EXPECT_NE(IntrinsicFunctions::getThrowFunction(mContext), nullptr);
}

TEST_F(IntrinsicFunctionsTest, getAllocateExceptionFunctionTest) {
  EXPECT_NE(IntrinsicFunctions::getAllocateExceptionFunction(mContext), nullptr);
}

TEST_F(IntrinsicFunctionsTest, getUnexpectedFunctionTest) {
  EXPECT_NE(IntrinsicFunctions::getUnexpectedFunction(mContext), nullptr);
}

TEST_F(IntrinsicFunctionsTest, getBeginCatchFunctionTest) {
  EXPECT_NE(IntrinsicFunctions::getBeginCatchFunction(mContext), nullptr);
}

TEST_F(IntrinsicFunctionsTest, getEndCatchFunctionTest) {
  EXPECT_NE(IntrinsicFunctions::getEndCatchFunction(mContext), nullptr);
}

TEST_F(IntrinsicFunctionsTest, getPersonalityFunctionTest) {
  EXPECT_NE(IntrinsicFunctions::getPersonalityFunction(mContext), nullptr);
}

TEST_F(IntrinsicFunctionsTest, getTypeIdFunctionTest) {
  EXPECT_NE(IntrinsicFunctions::getTypeIdFunction(mContext), nullptr);
}

TEST_F(IntrinsicFunctionsTest, getMemCopyFunctionTest) {
  EXPECT_NE(IntrinsicFunctions::getMemCopyFunction(mContext), nullptr);
}

TEST_F(IntrinsicFunctionsTest, setMemoryToZeroTest) {
  Type* type = Type::getInt32Ty(mLLVMContext);
  Value* alloca = IRWriter::newAllocaInst(mContext, type, "");
  IntrinsicFunctions::setMemoryToZero(mContext, alloca, ConstantExpr::getSizeOf(type));
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = alloca i32"
  "\n  %1 = bitcast i32* %0 to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %1, i8 0, i64 ptrtoint (i32* getelementptr "
  "(i32, i32* null, i32 1) to i64), i32 4, i1 false)\n";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(IntrinsicFunctionsTest, getMemSetFunctionTest) {
  EXPECT_NE(IntrinsicFunctions::getMemSetFunction(mContext), nullptr);
}

TEST_F(TestFileRunner, terminateRunTest) {
  compileAndRunFile("tests/samples/test_terminate.yz", 3);
}
