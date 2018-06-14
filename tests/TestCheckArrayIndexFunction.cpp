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
  CheckArrayIndexFunction::call(mContext, index, arraySize);
  BranchInst::Create(mEntryBlock, mDeclareBlock);
  
  *mStringStream << *mFunction;
  string expected =
  "\ndefine internal i32 @main() personality i32 (...)* @__gxx_personality_v0 {"
  "\ndeclare:"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declare"
  "\n  invoke void @__checkArrayIndexFunction(i64 -1, i64 5)"
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
  "\ndefine void @__checkArrayIndexFunction(i64 %index, i64 %size) personality i32 (...)* @__gxx_personality_v0 {"
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
  "\n  %10 = bitcast { i8*, i8* }* @wisey.lang.MArrayIndexOutOfBoundsException.rtti to i8*"
  "\n  %11 = bitcast %wisey.lang.MArrayIndexOutOfBoundsException* %2 to i8*"
  "\n  %12 = getelementptr i8, i8* %11, i64 -8"
  "\n  %13 = call i8* @__cxa_allocate_exception(i64 add (i64 ptrtoint (%wisey.lang.MArrayIndexOutOfBoundsException* getelementptr (%wisey.lang.MArrayIndexOutOfBoundsException, %wisey.lang.MArrayIndexOutOfBoundsException* null, i32 1) to i64), i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64)))"
  "\n  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %13, i8* %12, i64 add (i64 ptrtoint (%wisey.lang.MArrayIndexOutOfBoundsException* getelementptr (%wisey.lang.MArrayIndexOutOfBoundsException, %wisey.lang.MArrayIndexOutOfBoundsException* null, i32 1) to i64), i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64)), i32 4, i1 false)"
  "\n  tail call void @free(i8* %12)"
  "\n  invoke void @__cxa_throw(i8* %13, i8* %10, i8* null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\nif.end:                                           ; preds = %entry"
  "\n  ret void"
  "\n"
  "\ncleanup:                                          ; preds = %if.then"
  "\n  %14 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %15 = alloca { i8*, i32 }"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { i8*, i32 } %14, { i8*, i32 }* %15"
  "\n  %16 = getelementptr { i8*, i32 }, { i8*, i32 }* %15, i32 0, i32 0"
  "\n  %17 = load i8*, i8** %16"
  "\n  %18 = call i8* @__cxa_get_exception_ptr(i8* %17)"
  "\n  %19 = getelementptr i8, i8* %18, i64 8"
  "\n  resume { i8*, i32 } %14"
  "\n"
  "\ninvoke.continue:                                  ; preds = %if.then"
  "\n  unreachable"
  "\n}\n";

  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

