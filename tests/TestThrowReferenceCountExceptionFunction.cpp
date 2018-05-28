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
#include "wisey/IRGenerationContext.hpp"
#include "wisey/ThrowReferenceCountExceptionFunction.hpp"

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
    mContext.getScopes().pushScope();
    
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
  Function* function = ThrowReferenceCountExceptionFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "\ndefine void @__throwReferenceCountException(i64 %referenceCount, i8* %namePointer, i8* %exception) personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %0 = bitcast i8* %exception to %wisey.lang.IException*"
  "\n  %1 = bitcast %wisey.lang.IException* %0 to i8*"
  "\n  invoke void @__checkCastToObject(i8* %1, i8 109, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @model, i32 0, i32 0))"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %invoke.continue, %entry"
  "\n  %2 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %3 = alloca { i8*, i32 }"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { i8*, i32 } %2, { i8*, i32 }* %3"
  "\n  %4 = getelementptr { i8*, i32 }, { i8*, i32 }* %3, i32 0, i32 0"
  "\n  %5 = load i8*, i8** %4"
  "\n  %6 = call i8* @__cxa_get_exception_ptr(i8* %5)"
  "\n  %7 = getelementptr i8, i8* %6, i64 8"
  "\n  resume { i8*, i32 } %2"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n  %8 = call %wisey.lang.MReferenceCountException* @wisey.lang.MReferenceCountException.build(i64 %referenceCount, i8* %namePointer, %wisey.lang.IException* %0)"
  "\n  %9 = bitcast { i8*, i8* }* @wisey.lang.MReferenceCountException.rtti to i8*"
  "\n  %10 = bitcast %wisey.lang.MReferenceCountException* %8 to i8*"
  "\n  %11 = getelementptr i8, i8* %10, i64 -8"
  "\n  %12 = call i8* @__cxa_allocate_exception(i64 add (i64 ptrtoint (%wisey.lang.MReferenceCountException* getelementptr (%wisey.lang.MReferenceCountException, %wisey.lang.MReferenceCountException* null, i32 1) to i64), i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64)))"
  "\n  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %12, i8* %11, i64 add (i64 ptrtoint (%wisey.lang.MReferenceCountException* getelementptr (%wisey.lang.MReferenceCountException, %wisey.lang.MReferenceCountException* null, i32 1) to i64), i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64)), i32 4, i1 false)"
  "\n  tail call void @free(i8* %11)"
  "\n  invoke void @__cxa_throw(i8* %12, i8* %9, i8* null)"
  "\n          to label %invoke.continue1 unwind label %cleanup"
  "\n"
  "\ninvoke.continue1:                                 ; preds = %invoke.continue"
  "\n  unreachable"
  "\n}\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
