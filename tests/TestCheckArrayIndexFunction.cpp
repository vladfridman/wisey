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
    TestPrefix::generateIR(mContext);
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    
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
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint (%wisey.lang.MArrayIndexOutOfBoundsException.refCounter* getelementptr (%wisey.lang.MArrayIndexOutOfBoundsException.refCounter, %wisey.lang.MArrayIndexOutOfBoundsException.refCounter* null, i32 1) to i64))"
  "\n  %buildervar = bitcast i8* %malloccall to %wisey.lang.MArrayIndexOutOfBoundsException.refCounter*"
  "\n  %1 = bitcast %wisey.lang.MArrayIndexOutOfBoundsException.refCounter* %buildervar to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %1, i8 0, i64 ptrtoint (%wisey.lang.MArrayIndexOutOfBoundsException.refCounter* getelementptr (%wisey.lang.MArrayIndexOutOfBoundsException.refCounter, %wisey.lang.MArrayIndexOutOfBoundsException.refCounter* null, i32 1) to i64), i32 4, i1 false)"
  "\n  %2 = getelementptr %wisey.lang.MArrayIndexOutOfBoundsException.refCounter, %wisey.lang.MArrayIndexOutOfBoundsException.refCounter* %buildervar, i32 0, i32 1"
  "\n  %3 = getelementptr %wisey.lang.MArrayIndexOutOfBoundsException, %wisey.lang.MArrayIndexOutOfBoundsException* %2, i32 0, i32 1"
  "\n  store i64 %size, i64* %3"
  "\n  %4 = getelementptr %wisey.lang.MArrayIndexOutOfBoundsException, %wisey.lang.MArrayIndexOutOfBoundsException* %2, i32 0, i32 2"
  "\n  store i64 %index, i64* %4"
  "\n  %5 = bitcast %wisey.lang.MArrayIndexOutOfBoundsException* %2 to i8*"
  "\n  %6 = getelementptr i8, i8* %5, i64 0"
  "\n  %7 = bitcast i8* %6 to i32 (...)***"
  "\n  %8 = getelementptr { [3 x i8*] }, { [3 x i8*] }* @wisey.lang.MArrayIndexOutOfBoundsException.vtable, i32 0, i32 0, i32 0"
  "\n  %9 = bitcast i8** %8 to i32 (...)**"
  "\n  store i32 (...)** %9, i32 (...)*** %7"
  "\n  %10 = alloca %wisey.lang.MArrayIndexOutOfBoundsException*"
  "\n  store %wisey.lang.MArrayIndexOutOfBoundsException* %2, %wisey.lang.MArrayIndexOutOfBoundsException** %10"
  "\n  %11 = bitcast { i8*, i8* }* @wisey.lang.MArrayIndexOutOfBoundsException.rtti to i8*"
  "\n  %12 = bitcast %wisey.lang.MArrayIndexOutOfBoundsException* %2 to i8*"
  "\n  %13 = getelementptr i8, i8* %12, i64 -8"
  "\n  %14 = call i8* @__cxa_allocate_exception(i64 add (i64 ptrtoint (%wisey.lang.MArrayIndexOutOfBoundsException* getelementptr (%wisey.lang.MArrayIndexOutOfBoundsException, %wisey.lang.MArrayIndexOutOfBoundsException* null, i32 1) to i64), i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64)))"
  "\n  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %14, i8* %13, i64 add (i64 ptrtoint (%wisey.lang.MArrayIndexOutOfBoundsException* getelementptr (%wisey.lang.MArrayIndexOutOfBoundsException, %wisey.lang.MArrayIndexOutOfBoundsException* null, i32 1) to i64), i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64)), i32 4, i1 false)"
  "\n  invoke void @__cxa_throw(i8* %14, i8* %11, i8* null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\nif.end:                                           ; preds = %entry"
  "\n  ret void"
  "\n"
  "\ncleanup:                                          ; preds = %if.then"
  "\n  %15 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %16 = load %wisey.lang.MArrayIndexOutOfBoundsException*, %wisey.lang.MArrayIndexOutOfBoundsException** %10"
  "\n  %17 = bitcast %wisey.lang.MArrayIndexOutOfBoundsException* %16 to i8*"
  "\n  call void @__destroyOwnerObjectFunction(i8* %17)"
  "\n  resume { i8*, i32 } %15"
  "\n"
  "\ninvoke.continue:                                  ; preds = %if.then"
  "\n  unreachable"
  "\n}\n";

  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

