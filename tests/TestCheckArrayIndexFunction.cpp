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

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct CheckArrayIndexFunctionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mEntryBlock;
  BasicBlock* mDeclareBlock;
  Function* mFunction;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  CheckArrayIndexFunctionTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "main",
                                 mContext.getModule());
    mDeclareBlock = BasicBlock::Create(mLLVMContext, "declare", mFunction);
    mEntryBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setDeclarationsBlock(mDeclareBlock);
    mContext.setBasicBlock(mEntryBlock);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~CheckArrayIndexFunctionTest() {
  }
};

TEST_F(CheckArrayIndexFunctionTest, callTest) {
  Value* index = ConstantInt::get(Type::getInt64Ty(mLLVMContext), -1);
  Value* arraySize = ConstantInt::get(Type::getInt64Ty(mLLVMContext), 5);
  CheckArrayIndexFunction::call(mContext, index, arraySize, 11);
  BranchInst::Create(mEntryBlock, mDeclareBlock);
  
  *mStringStream << *mFunction;
  string expected =
  "\ndefine internal i32 @main() personality i32 (...)* @__gxx_personality_v0 {"
  "\ndeclare:"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declare"
  "\n  invoke void @__checkArrayIndexFunction(%wisey.threads.CCallStack* null, i32 11, i64 -1, i64 5)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %entry"
  "\n  %0 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %1 = alloca { i8*, i32 }"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { i8*, i32 } %0, { i8*, i32 }* %1"
  "\n  %2 = getelementptr { i8*, i32 }, { i8*, i32 }* %1, i32 0, i32 0"
  "\n  %3 = load i8*, i8** %2"
  "\n  %4 = call i8* @__cxa_get_exception_ptr(i8* %3)"
  "\n  %5 = getelementptr i8, i8* %4, i64 8"
  "\n  resume { i8*, i32 } %0"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n}\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(CheckArrayIndexFunctionTest, getTest) {
  mContext.getScopes().popScope(mContext, 0);
  mContext.getScopes().pushScope();
  
  Function* function = CheckArrayIndexFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "\ndefine void @__checkArrayIndexFunction(%wisey.threads.CCallStack* %callstack, i32 %lineNumber, i64 %index, i64 %size) personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %cmp = icmp sge i64 %index, %size"
  "\n  %cmp1 = icmp slt i64 %index, 0"
  "\n  %0 = or i1 %cmp, %cmp1"
  "\n  br i1 %0, label %throw, label %return"
  "\n"
  "\nthrow:                                            ; preds = %entry"
  "\n  %1 = bitcast %wisey.threads.CCallStack* %callstack to %CCallStack*"
  "\n  %2 = getelementptr %CCallStack, %CCallStack* %1, i32 0, i32 3"
  "\n  %3 = load i32, i32* %2"
  "\n  %4 = sub i32 %3, 1"
  "\n  %5 = getelementptr %CCallStack, %CCallStack* %1, i32 0, i32 2"
  "\n  %6 = load { i64, i64, i64, [0 x i32] }*, { i64, i64, i64, [0 x i32] }** %5"
  "\n  %7 = getelementptr { i64, i64, i64, [0 x i32] }, { i64, i64, i64, [0 x i32] }* %6, i32 0, i32 3"
  "\n  %8 = getelementptr [0 x i32], [0 x i32]* %7, i32 0, i32 %4"
  "\n  store i32 %lineNumber, i32* %8"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint (%wisey.lang.MArrayIndexOutOfBoundsException.refCounter* getelementptr (%wisey.lang.MArrayIndexOutOfBoundsException.refCounter, %wisey.lang.MArrayIndexOutOfBoundsException.refCounter* null, i32 1) to i64))"
  "\n  %buildervar = bitcast i8* %malloccall to %wisey.lang.MArrayIndexOutOfBoundsException.refCounter*"
  "\n  %9 = bitcast %wisey.lang.MArrayIndexOutOfBoundsException.refCounter* %buildervar to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %9, i8 0, i64 ptrtoint (%wisey.lang.MArrayIndexOutOfBoundsException.refCounter* getelementptr (%wisey.lang.MArrayIndexOutOfBoundsException.refCounter, %wisey.lang.MArrayIndexOutOfBoundsException.refCounter* null, i32 1) to i64), i32 4, i1 false)"
  "\n  %10 = getelementptr %wisey.lang.MArrayIndexOutOfBoundsException.refCounter, %wisey.lang.MArrayIndexOutOfBoundsException.refCounter* %buildervar, i32 0, i32 1"
  "\n  %11 = getelementptr %wisey.lang.MArrayIndexOutOfBoundsException, %wisey.lang.MArrayIndexOutOfBoundsException* %10, i32 0, i32 1"
  "\n  store i64 %size, i64* %11"
  "\n  %12 = getelementptr %wisey.lang.MArrayIndexOutOfBoundsException, %wisey.lang.MArrayIndexOutOfBoundsException* %10, i32 0, i32 2"
  "\n  store i64 %index, i64* %12"
  "\n  %13 = bitcast %wisey.lang.MArrayIndexOutOfBoundsException* %10 to i8*"
  "\n  %14 = getelementptr i8, i8* %13, i64 0"
  "\n  %15 = bitcast i8* %14 to i32 (...)***"
  "\n  %16 = getelementptr { [3 x i8*] }, { [3 x i8*] }* @wisey.lang.MArrayIndexOutOfBoundsException.vtable, i32 0, i32 0, i32 0"
  "\n  %17 = bitcast i8** %16 to i32 (...)**"
  "\n  store i32 (...)** %17, i32 (...)*** %15"
  "\n  %18 = bitcast { i8*, i8* }* @wisey.lang.MArrayIndexOutOfBoundsException.rtti to i8*"
  "\n  %19 = bitcast %wisey.lang.MArrayIndexOutOfBoundsException* %10 to i8*"
  "\n  %20 = getelementptr i8, i8* %19, i64 -8"
  "\n  %21 = call i8* @__cxa_allocate_exception(i64 add (i64 ptrtoint (%wisey.lang.MArrayIndexOutOfBoundsException* getelementptr (%wisey.lang.MArrayIndexOutOfBoundsException, %wisey.lang.MArrayIndexOutOfBoundsException* null, i32 1) to i64), i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64)))"
  "\n  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %21, i8* %20, i64 add (i64 ptrtoint (%wisey.lang.MArrayIndexOutOfBoundsException* getelementptr (%wisey.lang.MArrayIndexOutOfBoundsException, %wisey.lang.MArrayIndexOutOfBoundsException* null, i32 1) to i64), i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64)), i32 4, i1 false)"
  "\n  tail call void @free(i8* %20)"
  "\n  invoke void @__cxa_throw(i8* %21, i8* %18, i8* null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\nreturn:                                           ; preds = %entry"
  "\n  ret void"
  "\n"
  "\ncleanup:                                          ; preds = %throw"
  "\n  %22 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %23 = alloca { i8*, i32 }"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { i8*, i32 } %22, { i8*, i32 }* %23"
  "\n  %24 = getelementptr { i8*, i32 }, { i8*, i32 }* %23, i32 0, i32 0"
  "\n  %25 = load i8*, i8** %24"
  "\n  %26 = call i8* @__cxa_get_exception_ptr(i8* %25)"
  "\n  %27 = getelementptr i8, i8* %26, i64 8"
  "\n  resume { i8*, i32 } %22"
  "\n"
  "\ninvoke.continue:                                  ; preds = %throw"
  "\n  unreachable"
  "\n}\n";

  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

