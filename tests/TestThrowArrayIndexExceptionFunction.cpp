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
#include "ThrowArrayIndexExceptionFunction.hpp"
#include "IRGenerationContext.hpp"

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
  "define internal i32 @main() personality ptr @__gxx_personality_v0 {"
  "\ndeclare:"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declare"
  "\n  invoke void @__throwArrayIndexException(ptr null, i32 11, i64 -1, i64 5)"
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

TEST_F(ThrowArrayIndexExceptionFunctionTest, getTest) {
  mContext.getScopes().popScope(mContext, 0);
  mContext.getScopes().pushScope();
  
  Function* function = ThrowArrayIndexExceptionFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "define void @__throwArrayIndexException(ptr %callstack, i32 %lineNumber, i64 %index, i64 %size) personality ptr @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %0 = bitcast ptr %callstack to ptr"
  "\n  %1 = getelementptr %CCallStack, ptr %0, i32 0, i32 3"
  "\n  %2 = load i32, ptr %1, align 4"
  "\n  %3 = sub i32 %2, 1"
  "\n  %4 = getelementptr %CCallStack, ptr %0, i32 0, i32 2"
  "\n  %5 = load ptr, ptr %4, align 8"
  "\n  %6 = getelementptr { i64, i64, i64, [0 x i32] }, ptr %5, i32 0, i32 3"
  "\n  %7 = getelementptr [0 x i32], ptr %6, i32 0, i32 %3"
  "\n  store i32 %lineNumber, ptr %7, align 4"
  "\n  %buildervar = tail call ptr @malloc(i64 ptrtoint (ptr getelementptr (%wisey.lang.MArrayIndexOutOfBoundsException.refCounter, ptr null, i32 1) to i64))"
  "\n  %8 = bitcast ptr %buildervar to ptr"
  "\n  call void @llvm.memset.p0.i64(ptr %8, i8 0, i64 ptrtoint (ptr getelementptr (%wisey.lang.MArrayIndexOutOfBoundsException.refCounter, ptr null, i32 1) to i64), i1 false)"
  "\n  %9 = getelementptr %wisey.lang.MArrayIndexOutOfBoundsException.refCounter, ptr %buildervar, i32 0, i32 1"
  "\n  %10 = getelementptr %wisey.lang.MArrayIndexOutOfBoundsException, ptr %9, i32 0, i32 1"
  "\n  store i64 %size, ptr %10, align 4"
  "\n  %11 = getelementptr %wisey.lang.MArrayIndexOutOfBoundsException, ptr %9, i32 0, i32 2"
  "\n  store i64 %index, ptr %11, align 4"
  "\n  %12 = bitcast ptr %9 to ptr"
  "\n  %13 = getelementptr i8, ptr %12, i64 0"
  "\n  %14 = bitcast ptr %13 to ptr"
  "\n  %15 = getelementptr { [3 x ptr] }, ptr @wisey.lang.MArrayIndexOutOfBoundsException.vtable, i32 0, i32 0, i32 0"
  "\n  %16 = bitcast ptr %15 to ptr"
  "\n  store ptr %16, ptr %14, align 8"
  "\n  %17 = bitcast ptr @wisey.lang.MArrayIndexOutOfBoundsException.rtti to ptr"
  "\n  %18 = bitcast ptr %9 to ptr"
  "\n  %19 = getelementptr i8, ptr %18, i64 -8"
  "\n  %20 = call ptr @__cxa_allocate_exception(i64 add (i64 ptrtoint (ptr getelementptr (%wisey.lang.MArrayIndexOutOfBoundsException, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)))"
  "\n  call void @llvm.memcpy.p0.p0.i64(ptr %20, ptr %19, i64 add (i64 ptrtoint (ptr getelementptr (%wisey.lang.MArrayIndexOutOfBoundsException, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)), i1 false)"
  "\n  tail call void @free(ptr %19)"
  "\n  invoke void @__cxa_throw(ptr %20, ptr %17, ptr null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %entry"
  "\n  %21 = landingpad { ptr, i32 }"
  "\n          cleanup"
  "\n  %22 = alloca { ptr, i32 }, align 8"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { ptr, i32 } %21, ptr %22, align 8"
  "\n  %23 = getelementptr { ptr, i32 }, ptr %22, i32 0, i32 0"
  "\n  %24 = load ptr, ptr %23, align 8"
  "\n  %25 = call ptr @__cxa_get_exception_ptr(ptr %24)"
  "\n  %26 = getelementptr i8, ptr %25, i64 8"
  "\n  resume { ptr, i32 } %21"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n  unreachable"
  "\n}"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

