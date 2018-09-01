//
//  TestThrowArrayIndexExceptionFunction.cpp
//  runtests
//
//  Created by Vladimir Fridman on 8/23/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ThrowArrayIndexExceptionFunction}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestPrefix.hpp"
#include "wisey/ThrowArrayIndexExceptionFunction.hpp"
#include "wisey/IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct ThrowArrayIndexExceptionFunctionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mEntryBlock;
  BasicBlock* mDeclareBlock;
  Function* mFunction;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  ThrowArrayIndexExceptionFunctionTest() : mLLVMContext(mContext.getLLVMContext()) {
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
  
  ~ThrowArrayIndexExceptionFunctionTest() {
  }
};

TEST_F(ThrowArrayIndexExceptionFunctionTest, callTest) {
  Value* index = ConstantInt::get(Type::getInt64Ty(mLLVMContext), -1);
  Value* arraySize = ConstantInt::get(Type::getInt64Ty(mLLVMContext), 5);
  ThrowArrayIndexExceptionFunction::call(mContext, arraySize, index, 11);
  BranchInst::Create(mEntryBlock, mDeclareBlock);
  
  *mStringStream << *mFunction;
  string expected =
  "\ndefine internal i32 @main() personality i32 (...)* @__gxx_personality_v0 {"
  "\ndeclare:"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declare"
  "\n  invoke void @__throwArrayIndexException(%wisey.threads.CCallStack* null, i32 11, i64 -1, i64 5)"
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

TEST_F(ThrowArrayIndexExceptionFunctionTest, getTest) {
  mContext.getScopes().popScope(mContext, 0);
  mContext.getScopes().pushScope();
  
  Function* function = ThrowArrayIndexExceptionFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "\ndefine void @__throwArrayIndexException(%wisey.threads.CCallStack* %callstack, i32 %lineNumber, i64 %index, i64 %size) personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %0 = bitcast %wisey.threads.CCallStack* %callstack to %CCallStack*"
  "\n  %1 = getelementptr %CCallStack, %CCallStack* %0, i32 0, i32 3"
  "\n  %2 = load i32, i32* %1"
  "\n  %3 = sub i32 %2, 1"
  "\n  %4 = getelementptr %CCallStack, %CCallStack* %0, i32 0, i32 2"
  "\n  %5 = load { i64, i64, i64, [0 x i32] }*, { i64, i64, i64, [0 x i32] }** %4"
  "\n  %6 = getelementptr { i64, i64, i64, [0 x i32] }, { i64, i64, i64, [0 x i32] }* %5, i32 0, i32 3"
  "\n  %7 = getelementptr [0 x i32], [0 x i32]* %6, i32 0, i32 %3"
  "\n  store i32 %lineNumber, i32* %7"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint (%wisey.lang.MArrayIndexOutOfBoundsException.refCounter* getelementptr (%wisey.lang.MArrayIndexOutOfBoundsException.refCounter, %wisey.lang.MArrayIndexOutOfBoundsException.refCounter* null, i32 1) to i64))"
  "\n  %buildervar = bitcast i8* %malloccall to %wisey.lang.MArrayIndexOutOfBoundsException.refCounter*"
  "\n  %8 = bitcast %wisey.lang.MArrayIndexOutOfBoundsException.refCounter* %buildervar to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %8, i8 0, i64 ptrtoint (%wisey.lang.MArrayIndexOutOfBoundsException.refCounter* getelementptr (%wisey.lang.MArrayIndexOutOfBoundsException.refCounter, %wisey.lang.MArrayIndexOutOfBoundsException.refCounter* null, i32 1) to i64), i1 false)"
  "\n  %9 = getelementptr %wisey.lang.MArrayIndexOutOfBoundsException.refCounter, %wisey.lang.MArrayIndexOutOfBoundsException.refCounter* %buildervar, i32 0, i32 1"
  "\n  %10 = getelementptr %wisey.lang.MArrayIndexOutOfBoundsException, %wisey.lang.MArrayIndexOutOfBoundsException* %9, i32 0, i32 1"
  "\n  store i64 %size, i64* %10"
  "\n  %11 = getelementptr %wisey.lang.MArrayIndexOutOfBoundsException, %wisey.lang.MArrayIndexOutOfBoundsException* %9, i32 0, i32 2"
  "\n  store i64 %index, i64* %11"
  "\n  %12 = bitcast %wisey.lang.MArrayIndexOutOfBoundsException* %9 to i8*"
  "\n  %13 = getelementptr i8, i8* %12, i64 0"
  "\n  %14 = bitcast i8* %13 to i32 (...)***"
  "\n  %15 = getelementptr { [3 x i8*] }, { [3 x i8*] }* @wisey.lang.MArrayIndexOutOfBoundsException.vtable, i32 0, i32 0, i32 0"
  "\n  %16 = bitcast i8** %15 to i32 (...)**"
  "\n  store i32 (...)** %16, i32 (...)*** %14"
  "\n  %17 = bitcast { i8*, i8* }* @wisey.lang.MArrayIndexOutOfBoundsException.rtti to i8*"
  "\n  %18 = bitcast %wisey.lang.MArrayIndexOutOfBoundsException* %9 to i8*"
  "\n  %19 = getelementptr i8, i8* %18, i64 -8"
  "\n  %20 = call i8* @__cxa_allocate_exception(i64 add (i64 ptrtoint (%wisey.lang.MArrayIndexOutOfBoundsException* getelementptr (%wisey.lang.MArrayIndexOutOfBoundsException, %wisey.lang.MArrayIndexOutOfBoundsException* null, i32 1) to i64), i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64)))"
  "\n  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %20, i8* %19, i64 add (i64 ptrtoint (%wisey.lang.MArrayIndexOutOfBoundsException* getelementptr (%wisey.lang.MArrayIndexOutOfBoundsException, %wisey.lang.MArrayIndexOutOfBoundsException* null, i32 1) to i64), i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64)), i1 false)"
  "\n  tail call void @free(i8* %19)"
  "\n  invoke void @__cxa_throw(i8* %20, i8* %17, i8* null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %entry"
  "\n  %21 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %22 = alloca { i8*, i32 }"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { i8*, i32 } %21, { i8*, i32 }* %22"
  "\n  %23 = getelementptr { i8*, i32 }, { i8*, i32 }* %22, i32 0, i32 0"
  "\n  %24 = load i8*, i8** %23"
  "\n  %25 = call i8* @__cxa_get_exception_ptr(i8* %24)"
  "\n  %26 = getelementptr i8, i8* %25, i64 8"
  "\n  resume { i8*, i32 } %21"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n  unreachable"
  "\n}\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

