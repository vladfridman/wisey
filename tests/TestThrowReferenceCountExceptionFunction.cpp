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
#include "wisey/ProgramPrefix.hpp"
#include "wisey/ThrowReferenceCountExceptionFunction.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct ThrowReferenceCountExceptionFunctionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  Function* mFunction;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  ThrowReferenceCountExceptionFunctionTest() : mLLVMContext(mContext.getLLVMContext()) {
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    TestPrefix::generateIR(mContext);
    
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
  
  ~ThrowReferenceCountExceptionFunctionTest() {
  }
};

TEST_F(ThrowReferenceCountExceptionFunctionTest, callTest) {
  Value* referenceCount = ConstantInt::get(Type::getInt64Ty(mLLVMContext), 5);
  ThrowReferenceCountExceptionFunction::call(mContext, referenceCount);
  
  *mStringStream << *mFunction;
  string expected =
  "\ndefine internal i32 @main() personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  invoke void @__throwReferenceCountException(i64 5)"
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

TEST_F(ThrowReferenceCountExceptionFunctionTest, getTest) {
  Function* function = ThrowReferenceCountExceptionFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "\ndefine internal void @__throwReferenceCountException(i64 %referenceCount) "
  "personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint (%wisey.lang.MReferenceCountException* getelementptr (%wisey.lang.MReferenceCountException, %wisey.lang.MReferenceCountException* null, i32 1) to i64))"
  "\n  %buildervar = bitcast i8* %malloccall to %wisey.lang.MReferenceCountException*"
  "\n  %0 = bitcast %wisey.lang.MReferenceCountException* %buildervar to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %0, i8 0, i64 ptrtoint (%wisey.lang.MReferenceCountException* getelementptr (%wisey.lang.MReferenceCountException, %wisey.lang.MReferenceCountException* null, i32 1) to i64), i32 4, i1 false)"
  "\n  %1 = getelementptr %wisey.lang.MReferenceCountException, %wisey.lang.MReferenceCountException* %buildervar, i32 0, i32 2"
  "\n  store i64 %referenceCount, i64* %1"
  "\n  %2 = bitcast %wisey.lang.MReferenceCountException* %buildervar to i8*"
  "\n  %3 = getelementptr i8, i8* %2, i64 8"
  "\n  %4 = bitcast i8* %3 to i32 (...)***"
  "\n  %5 = getelementptr { [3 x i8*] }, { [3 x i8*] }* @wisey.lang.MReferenceCountException.vtable, i32 0, i32 0, i32 0"
  "\n  %6 = bitcast i8** %5 to i32 (...)**"
  "\n  store i32 (...)** %6, i32 (...)*** %4"
  "\n  %7 = alloca %wisey.lang.MReferenceCountException*"
  "\n  store %wisey.lang.MReferenceCountException* %buildervar, %wisey.lang.MReferenceCountException** %7"
  "\n  %8 = bitcast %wisey.lang.MReferenceCountException* %buildervar to i8*"
  "\n  %9 = bitcast { i8*, i8* }* @wisey.lang.MReferenceCountException.rtti to i8*"
  "\n  %10 = call i8* @__cxa_allocate_exception(i64 ptrtoint (%wisey.lang.MReferenceCountException* getelementptr (%wisey.lang.MReferenceCountException, %wisey.lang.MReferenceCountException* null, i32 1) to i64))"
  "\n  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %10, i8* %8, i64 ptrtoint (%wisey.lang.MReferenceCountException* getelementptr (%wisey.lang.MReferenceCountException, %wisey.lang.MReferenceCountException* null, i32 1) to i64), i32 4, i1 false)"
  "\n  invoke void @__cxa_throw(i8* %10, i8* %9, i8* null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %entry"
  "\n  %11 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %12 = load %wisey.lang.MReferenceCountException*, %wisey.lang.MReferenceCountException** %7"
  "\n  %13 = bitcast %wisey.lang.MReferenceCountException* %12 to i8*"
  "\n  call void @destructor.wisey.lang.MReferenceCountException(i8* %13)"
  "\n  resume { i8*, i32 } %11"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n  unreachable"
  "\n}\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
