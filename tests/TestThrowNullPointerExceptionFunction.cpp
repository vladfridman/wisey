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
  "define internal i32 @main() personality ptr @__gxx_personality_v0 {"
  "\ndeclare:"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declare"
  "\n  invoke void @__throwNPE()"
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

TEST_F(ThrowNullPointerExceptionFunctionTest, getTest) {
  mContext.getScopes().popScope(mContext, 0);
  mContext.getScopes().pushScope();
  
  Function* function = ThrowNullPointerExceptionFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "define void @__throwNPE() personality ptr @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %buildervar = tail call ptr @malloc(i64 ptrtoint (ptr getelementptr (%wisey.lang.MNullPointerException.refCounter, ptr null, i32 1) to i64))"
  "\n  %0 = bitcast ptr %buildervar to ptr"
  "\n  call void @llvm.memset.p0.i64(ptr %0, i8 0, i64 ptrtoint (ptr getelementptr (%wisey.lang.MNullPointerException.refCounter, ptr null, i32 1) to i64), i1 false)"
  "\n  %1 = getelementptr %wisey.lang.MNullPointerException.refCounter, ptr %buildervar, i32 0, i32 1"
  "\n  %2 = bitcast ptr %1 to ptr"
  "\n  %3 = getelementptr i8, ptr %2, i64 0"
  "\n  %4 = bitcast ptr %3 to ptr"
  "\n  %5 = getelementptr { [3 x ptr] }, ptr @wisey.lang.MNullPointerException.vtable, i32 0, i32 0, i32 0"
  "\n  %6 = bitcast ptr %5 to ptr"
  "\n  store ptr %6, ptr %4, align 8"
  "\n  %7 = bitcast ptr @wisey.lang.MNullPointerException.rtti to ptr"
  "\n  %8 = bitcast ptr %1 to ptr"
  "\n  %9 = getelementptr i8, ptr %8, i64 -8"
  "\n  %10 = call ptr @__cxa_allocate_exception(i64 add (i64 ptrtoint (ptr getelementptr (%wisey.lang.MNullPointerException, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)))"
  "\n  call void @llvm.memcpy.p0.p0.i64(ptr %10, ptr %9, i64 add (i64 ptrtoint (ptr getelementptr (%wisey.lang.MNullPointerException, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)), i1 false)"
  "\n  tail call void @free(ptr %9)"
  "\n  invoke void @__cxa_throw(ptr %10, ptr %7, ptr null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %entry"
  "\n  %11 = landingpad { ptr, i32 }"
  "\n          cleanup"
  "\n  %12 = alloca { ptr, i32 }, align 8"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { ptr, i32 } %11, ptr %12, align 8"
  "\n  %13 = getelementptr { ptr, i32 }, ptr %12, i32 0, i32 0"
  "\n  %14 = load ptr, ptr %13, align 8"
  "\n  %15 = call ptr @__cxa_get_exception_ptr(ptr %14)"
  "\n  %16 = getelementptr i8, ptr %15, i64 8"
  "\n  resume { ptr, i32 } %11"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n  unreachable"
  "\n}"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}


