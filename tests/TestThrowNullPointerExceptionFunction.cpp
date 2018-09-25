//
//  TestThrowNullPointerExceptionFunction.cpp
//  runtests
//
//  Created by Vladimir Fridman on 12/1/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ThrowNullPointerExceptionFunction}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestPrefix.hpp"
#include "ThrowNullPointerExceptionFunction.hpp"
#include "IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct ThrowNullPointerExceptionFunctionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mDeclareBlock;
  BasicBlock* mEntryBlock;
  Function* mFunction;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  ThrowNullPointerExceptionFunctionTest() : mLLVMContext(mContext.getLLVMContext()) {
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
  
  ~ThrowNullPointerExceptionFunctionTest() {
  }
};

TEST_F(ThrowNullPointerExceptionFunctionTest, callTest) {
  ThrowNullPointerExceptionFunction::call(mContext);
  BranchInst::Create(mEntryBlock, mDeclareBlock);
  
  *mStringStream << *mFunction;
  string expected =
  "\ndefine internal i32 @main() personality i32 (...)* @__gxx_personality_v0 {"
  "\ndeclare:"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declare"
  "\n  invoke void @__throwNPE()"
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

TEST_F(ThrowNullPointerExceptionFunctionTest, getTest) {
  mContext.getScopes().popScope(mContext, 0);
  mContext.getScopes().pushScope();
  
  Function* function = ThrowNullPointerExceptionFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "\ndefine void @__throwNPE() personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint (%wisey.lang.MNullPointerException.refCounter* getelementptr (%wisey.lang.MNullPointerException.refCounter, %wisey.lang.MNullPointerException.refCounter* null, i32 1) to i64))"
  "\n  %buildervar = bitcast i8* %malloccall to %wisey.lang.MNullPointerException.refCounter*"
  "\n  %0 = bitcast %wisey.lang.MNullPointerException.refCounter* %buildervar to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %0, i8 0, i64 ptrtoint (%wisey.lang.MNullPointerException.refCounter* getelementptr (%wisey.lang.MNullPointerException.refCounter, %wisey.lang.MNullPointerException.refCounter* null, i32 1) to i64), i1 false)"
  "\n  %1 = getelementptr %wisey.lang.MNullPointerException.refCounter, %wisey.lang.MNullPointerException.refCounter* %buildervar, i32 0, i32 1"
  "\n  %2 = bitcast %wisey.lang.MNullPointerException* %1 to i8*"
  "\n  %3 = getelementptr i8, i8* %2, i64 0"
  "\n  %4 = bitcast i8* %3 to i32 (...)***"
  "\n  %5 = getelementptr { [3 x i8*] }, { [3 x i8*] }* @wisey.lang.MNullPointerException.vtable, i32 0, i32 0, i32 0"
  "\n  %6 = bitcast i8** %5 to i32 (...)**"
  "\n  store i32 (...)** %6, i32 (...)*** %4"
  "\n  %7 = bitcast { i8*, i8* }* @wisey.lang.MNullPointerException.rtti to i8*"
  "\n  %8 = bitcast %wisey.lang.MNullPointerException* %1 to i8*"
  "\n  %9 = getelementptr i8, i8* %8, i64 -8"
  "\n  %10 = call i8* @__cxa_allocate_exception(i64 add (i64 ptrtoint (i1** getelementptr (i1*, i1** null, i32 1) to i64), i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64)))"
  "\n  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %10, i8* %9, i64 add (i64 ptrtoint (i1** getelementptr (i1*, i1** null, i32 1) to i64), i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64)), i1 false)"
  "\n  tail call void @free(i8* %9)"
  "\n  invoke void @__cxa_throw(i8* %10, i8* %7, i8* null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %entry"
  "\n  %11 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %12 = alloca { i8*, i32 }"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { i8*, i32 } %11, { i8*, i32 }* %12"
  "\n  %13 = getelementptr { i8*, i32 }, { i8*, i32 }* %12, i32 0, i32 0"
  "\n  %14 = load i8*, i8** %13"
  "\n  %15 = call i8* @__cxa_get_exception_ptr(i8* %14)"
  "\n  %16 = getelementptr i8, i8* %15, i64 8"
  "\n  resume { i8*, i32 } %11"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n  unreachable"
  "\n}\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}


