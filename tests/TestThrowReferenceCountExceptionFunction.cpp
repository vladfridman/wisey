//
//  TestThrowReferenceCountExceptionFunction.cpp
//  runtests
//
//  Created by Vladimir Fridman on 12/1/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ThrowReferenceCountExceptionFunction}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestPrefix.hpp"
#include "IRGenerationContext.hpp"
#include "ThrowReferenceCountExceptionFunction.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct ThrowReferenceCountExceptionFunctionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mDeclareBlock;
  BasicBlock* mEntryBlock;
  Function* mFunction;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  ThrowReferenceCountExceptionFunctionTest() : mLLVMContext(mContext.getLLVMContext()) {
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
  
  ~ThrowReferenceCountExceptionFunctionTest() {
  }
};

TEST_F(ThrowReferenceCountExceptionFunctionTest, callTest) {
  Value* referenceCount = ConstantInt::get(Type::getInt64Ty(mLLVMContext), 5);
  Value* nullPointer = ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  ThrowReferenceCountExceptionFunction::call(mContext, referenceCount, nullPointer, nullPointer);
  BranchInst::Create(mEntryBlock, mDeclareBlock);
  
  *mStringStream << *mFunction;
  string expected =
  "\ndefine internal i32 @main() personality i32 (...)* @__gxx_personality_v0 {"
  "\ndeclare:"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declare"
  "\n  invoke void @__throwReferenceCountException(i64 5, i8* null, i8* null)"
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

TEST_F(ThrowReferenceCountExceptionFunctionTest, getTest) {
  mContext.getScopes().popScope(mContext, 0);
  mContext.getScopes().pushScope();
  
  Function* function = ThrowReferenceCountExceptionFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "\ndefine void @__throwReferenceCountException(i64 %referenceCount, i8* %namePointer, i8* %exception) personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %0 = bitcast i8* %exception to %wisey.lang.IException*"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint (%wisey.lang.MReferenceCountException.refCounter* getelementptr (%wisey.lang.MReferenceCountException.refCounter, %wisey.lang.MReferenceCountException.refCounter* null, i32 1) to i64))"
  "\n  %buildervar = bitcast i8* %malloccall to %wisey.lang.MReferenceCountException.refCounter*"
  "\n  %1 = bitcast %wisey.lang.MReferenceCountException.refCounter* %buildervar to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %1, i8 0, i64 ptrtoint (%wisey.lang.MReferenceCountException.refCounter* getelementptr (%wisey.lang.MReferenceCountException.refCounter, %wisey.lang.MReferenceCountException.refCounter* null, i32 1) to i64), i1 false)"
  "\n  %2 = getelementptr %wisey.lang.MReferenceCountException.refCounter, %wisey.lang.MReferenceCountException.refCounter* %buildervar, i32 0, i32 1"
  "\n  %3 = bitcast %wisey.lang.IException* %0 to i8*"
  "\n  invoke void @__checkCastToObject(i8* %3, i8 109, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @model, i32 0, i32 0))"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %invoke.continue, %entry"
  "\n  %4 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %5 = alloca { i8*, i32 }"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { i8*, i32 } %4, { i8*, i32 }* %5"
  "\n  %6 = getelementptr { i8*, i32 }, { i8*, i32 }* %5, i32 0, i32 0"
  "\n  %7 = load i8*, i8** %6"
  "\n  %8 = call i8* @__cxa_get_exception_ptr(i8* %7)"
  "\n  %9 = getelementptr i8, i8* %8, i64 8"
  "\n  resume { i8*, i32 } %4"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n  %10 = getelementptr %wisey.lang.MReferenceCountException, %wisey.lang.MReferenceCountException* %2, i32 0, i32 1"
  "\n  store i64 %referenceCount, i64* %10"
  "\n  %11 = getelementptr %wisey.lang.MReferenceCountException, %wisey.lang.MReferenceCountException* %2, i32 0, i32 2"
  "\n  store i8* %namePointer, i8** %11"
  "\n  %12 = getelementptr %wisey.lang.MReferenceCountException, %wisey.lang.MReferenceCountException* %2, i32 0, i32 3"
  "\n  store %wisey.lang.IException* %0, %wisey.lang.IException** %12"
  "\n  %13 = bitcast %wisey.lang.IException* %0 to i8*"
  "\n  call void @__adjustReferenceCounter(i8* %13, i64 1)"
  "\n  %14 = bitcast %wisey.lang.MReferenceCountException* %2 to i8*"
  "\n  %15 = getelementptr i8, i8* %14, i64 0"
  "\n  %16 = bitcast i8* %15 to i32 (...)***"
  "\n  %17 = getelementptr { [3 x i8*] }, { [3 x i8*] }* @wisey.lang.MReferenceCountException.vtable, i32 0, i32 0, i32 0"
  "\n  %18 = bitcast i8** %17 to i32 (...)**"
  "\n  store i32 (...)** %18, i32 (...)*** %16"
  "\n  %19 = bitcast { i8*, i8* }* @wisey.lang.MReferenceCountException.rtti to i8*"
  "\n  %20 = bitcast %wisey.lang.MReferenceCountException* %2 to i8*"
  "\n  %21 = getelementptr i8, i8* %20, i64 -8"
  "\n  %22 = call i8* @__cxa_allocate_exception(i64 add (i64 ptrtoint (%wisey.lang.MReferenceCountException* getelementptr (%wisey.lang.MReferenceCountException, %wisey.lang.MReferenceCountException* null, i32 1) to i64), i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64)))"
  "\n  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %22, i8* %21, i64 add (i64 ptrtoint (%wisey.lang.MReferenceCountException* getelementptr (%wisey.lang.MReferenceCountException, %wisey.lang.MReferenceCountException* null, i32 1) to i64), i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64)), i1 false)"
  "\n  tail call void @free(i8* %21)"
  "\n  invoke void @__cxa_throw(i8* %22, i8* %19, i8* null)"
  "\n          to label %invoke.continue1 unwind label %cleanup"
  "\n"
  "\ninvoke.continue1:                                 ; preds = %invoke.continue"
  "\n  unreachable"
  "\n}\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
