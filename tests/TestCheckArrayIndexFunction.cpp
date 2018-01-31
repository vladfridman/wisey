//
//  TestCheckArrayIndexFunction.cpp
//  runtests
//
//  Created by Vladimir Fridman on 1/31/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link CheckArrayIndexFunction}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestPrefix.hpp"
#include "wisey/CheckArrayIndexFunction.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/ProgramPrefix.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct CheckArrayIndexFunctionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  Function* mFunction;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  CheckArrayIndexFunctionTest() : mLLVMContext(mContext.getLLVMContext()) {
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    TestPrefix::generateIR(mContext);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "main",
                                 mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~CheckArrayIndexFunctionTest() {
  }
};

TEST_F(CheckArrayIndexFunctionTest, callTest) {
  Value* index = ConstantInt::get(Type::getInt64Ty(mLLVMContext), -1);
  Value* arraySize = ConstantInt::get(Type::getInt64Ty(mLLVMContext), 5);
  CheckArrayIndexFunction::call(mContext, index, arraySize);
  
  *mStringStream << *mFunction;
  string expected =
  "\ndefine internal i32 @main() personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  invoke void @__checkArrayIndexFunction(i64 -1, i64 5)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %entry"
  "\n  %0 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  resume { i8*, i32 } %0"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n}\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(CheckArrayIndexFunctionTest, getTest) {
  Function* function = CheckArrayIndexFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "\ndefine internal void @__checkArrayIndexFunction(i64 %index, i64 %size) personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %cmp = icmp sge i64 %index, %size"
  "\n  %cmp1 = icmp slt i64 %index, 0"
  "\n  %0 = or i1 %cmp, %cmp1"
  "\n  br i1 %0, label %if.then, label %if.end"
  "\n"
  "\nif.then:                                          ; preds = %entry"
  "\n  %malloccall = tail call i8* @malloc(i64 mul nuw (i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64), i64 3))"
  "\n  %buildervar = bitcast i8* %malloccall to %wisey.lang.MArrayIndexOutOfBoundsException*"
  "\n  %1 = bitcast %wisey.lang.MArrayIndexOutOfBoundsException* %buildervar to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %1, i8 0, i64 mul nuw (i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64), i64 3), i32 4, i1 false)"
  "\n  %2 = getelementptr %wisey.lang.MArrayIndexOutOfBoundsException, %wisey.lang.MArrayIndexOutOfBoundsException* %buildervar, i32 0, i32 1"
  "\n  store i64 %size, i64* %2"
  "\n  %3 = getelementptr %wisey.lang.MArrayIndexOutOfBoundsException, %wisey.lang.MArrayIndexOutOfBoundsException* %buildervar, i32 0, i32 2"
  "\n  store i64 %index, i64* %3"
  "\n  %4 = alloca %wisey.lang.MArrayIndexOutOfBoundsException*"
  "\n  store %wisey.lang.MArrayIndexOutOfBoundsException* %buildervar, %wisey.lang.MArrayIndexOutOfBoundsException** %4"
  "\n  %5 = bitcast %wisey.lang.MArrayIndexOutOfBoundsException* %buildervar to i8*"
  "\n  %6 = bitcast { i8*, i8* }* @wisey.lang.MArrayIndexOutOfBoundsException.rtti to i8*"
  "\n  %7 = call i8* @__cxa_allocate_exception(i64 mul nuw (i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64), i64 3))"
  "\n  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %7, i8* %5, i64 mul nuw (i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64), i64 3), i32 4, i1 false)"
  "\n  invoke void @__cxa_throw(i8* %7, i8* %6, i8* null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\nif.end:                                           ; preds = %entry"
  "\n  ret void"
  "\n"
  "\ncleanup:                                          ; preds = %if.then"
  "\n  %8 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %9 = load %wisey.lang.MArrayIndexOutOfBoundsException*, %wisey.lang.MArrayIndexOutOfBoundsException** %4"
  "\n  %10 = bitcast %wisey.lang.MArrayIndexOutOfBoundsException* %9 to i8*"
  "\n  call void @destructor.wisey.lang.MArrayIndexOutOfBoundsException(i8* %10)"
  "\n  resume { i8*, i32 } %8"
  "\n"
  "\ninvoke.continue:                                  ; preds = %if.then"
  "\n  unreachable"
  "\n}\n";

  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

