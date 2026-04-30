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
  "define internal i32 @main() personality ptr @__gxx_personality_v0 {"
  "\ndeclare:"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declare"
  "\n  invoke void @__throwReferenceCountException(i64 5, ptr null, ptr null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %entry"
  "\n  %0 = landingpad { ptr, i32 }"
  "\n          cleanup"
  "\n  %1 = alloca { ptr, i32 }, align 8"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { ptr, i32 } %0, ptr %1, align 8"
  "\n  %2 = getelementptr { ptr, i32 }, ptr %1, i32 0, i32 0"
  "\n  %3 = load ptr, ptr %2, align 8"
  "\n  %4 = call ptr @__cxa_get_exception_ptr(ptr %3)"
  "\n  %5 = getelementptr i8, ptr %4, i64 8"
  "\n  resume { ptr, i32 } %0"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n}"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ThrowReferenceCountExceptionFunctionTest, getTest) {
  mContext.getScopes().popScope(mContext, 0);
  mContext.getScopes().pushScope();
  
  Function* function = ThrowReferenceCountExceptionFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "define void @__throwReferenceCountException(i64 %referenceCount, ptr %namePointer, ptr %exception) personality ptr @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %0 = bitcast ptr %exception to ptr"
  "\n  %buildervar = tail call ptr @malloc(i64 ptrtoint (ptr getelementptr (%wisey.lang.MReferenceCountException.refCounter, ptr null, i32 1) to i64))"
  "\n  %1 = bitcast ptr %buildervar to ptr"
  "\n  call void @llvm.memset.p0.i64(ptr %1, i8 0, i64 ptrtoint (ptr getelementptr (%wisey.lang.MReferenceCountException.refCounter, ptr null, i32 1) to i64), i1 false)"
  "\n  %2 = getelementptr %wisey.lang.MReferenceCountException.refCounter, ptr %buildervar, i32 0, i32 1"
  "\n  %3 = bitcast ptr %0 to ptr"
  "\n  invoke void @__checkCastToObject(ptr %3, i8 109, ptr @model)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %invoke.continue, %entry"
  "\n  %4 = landingpad { ptr, i32 }"
  "\n          cleanup"
  "\n  %5 = alloca { ptr, i32 }, align 8"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { ptr, i32 } %4, ptr %5, align 8"
  "\n  %6 = getelementptr { ptr, i32 }, ptr %5, i32 0, i32 0"
  "\n  %7 = load ptr, ptr %6, align 8"
  "\n  %8 = call ptr @__cxa_get_exception_ptr(ptr %7)"
  "\n  %9 = getelementptr i8, ptr %8, i64 8"
  "\n  resume { ptr, i32 } %4"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n  %10 = getelementptr %wisey.lang.MReferenceCountException, ptr %2, i32 0, i32 1"
  "\n  store i64 %referenceCount, ptr %10, align 4"
  "\n  %11 = getelementptr %wisey.lang.MReferenceCountException, ptr %2, i32 0, i32 2"
  "\n  store ptr %namePointer, ptr %11, align 8"
  "\n  %12 = getelementptr %wisey.lang.MReferenceCountException, ptr %2, i32 0, i32 3"
  "\n  store ptr %0, ptr %12, align 8"
  "\n  call void @__adjustReferenceCounter(ptr %0, i64 1)"
  "\n  %13 = bitcast ptr %2 to ptr"
  "\n  %14 = getelementptr i8, ptr %13, i64 0"
  "\n  %15 = bitcast ptr %14 to ptr"
  "\n  %16 = getelementptr { [3 x ptr] }, ptr @wisey.lang.MReferenceCountException.vtable, i32 0, i32 0, i32 0"
  "\n  %17 = bitcast ptr %16 to ptr"
  "\n  store ptr %17, ptr %15, align 8"
  "\n  %18 = bitcast ptr @wisey.lang.MReferenceCountException.rtti to ptr"
  "\n  %19 = bitcast ptr %2 to ptr"
  "\n  %20 = getelementptr i8, ptr %19, i64 -8"
  "\n  %21 = call ptr @__cxa_allocate_exception(i64 add (i64 ptrtoint (ptr getelementptr (%wisey.lang.MReferenceCountException, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)))"
  "\n  call void @llvm.memcpy.p0.p0.i64(ptr %21, ptr %20, i64 add (i64 ptrtoint (ptr getelementptr (%wisey.lang.MReferenceCountException, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)), i1 false)"
  "\n  tail call void @free(ptr %20)"
  "\n  invoke void @__cxa_throw(ptr %21, ptr %18, ptr null)"
  "\n          to label %invoke.continue1 unwind label %cleanup"
  "\n"
  "\ninvoke.continue1:                                 ; preds = %invoke.continue"
  "\n  unreachable"
  "\n}"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
