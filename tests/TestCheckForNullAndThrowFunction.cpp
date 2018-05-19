//
//  TestCheckForNullAndThrowFunction.cpp
//  runtests
//
//  Created by Vladimir Fridman on 12/1/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link CheckForNullAndThrowFunction}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestPrefix.hpp"
#include "wisey/CheckForNullAndThrowFunction.hpp"
#include "wisey/IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct CheckForNullAndThrowFunctionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  Function* mFunction;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  CheckForNullAndThrowFunctionTest() : mLLVMContext(mContext.getLLVMContext()) {
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
  
  ~CheckForNullAndThrowFunctionTest() {
  }
};

TEST_F(CheckForNullAndThrowFunctionTest, callTest) {
  Value* nullPointerValue = ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  CheckForNullAndThrowFunction::call(mContext, nullPointerValue);
  
  *mStringStream << *mFunction;
  string expected =
  "\ndefine internal i32 @main() personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %0 = bitcast i8* null to i8*"
  "\n  invoke void @__checkForNullAndThrow(i8* %0)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %entry"
  "\n  %1 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %2 = alloca { i8*, i32 }"
  "\n  store { i8*, i32 } %1, { i8*, i32 }* %2"
  "\n  %3 = getelementptr { i8*, i32 }, { i8*, i32 }* %2, i32 0, i32 0"
  "\n  %4 = load i8*, i8** %3"
  "\n  %5 = call i8* @__cxa_get_exception_ptr(i8* %4)"
  "\n  %6 = getelementptr i8, i8* %5, i64 8"
  "\n  resume { i8*, i32 } %1"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n}\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(CheckForNullAndThrowFunctionTest, getTest) {
  Function* function = CheckForNullAndThrowFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "\ndefine void @__checkForNullAndThrow(i8* %pointer) personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %cmp = icmp eq i8* %pointer, null"
  "\n  br i1 %cmp, label %if.then, label %if.end"
  "\n"
  "\nif.then:                                          ; preds = %entry"
  "\n  %0 = call %wisey.lang.MNullPointerException* @wisey.lang.MNullPointerException.build()"
  "\n  %1 = alloca %wisey.lang.MNullPointerException*"
  "\n  store %wisey.lang.MNullPointerException* %0, %wisey.lang.MNullPointerException** %1"
  "\n  %2 = bitcast { i8*, i8* }* @wisey.lang.MNullPointerException.rtti to i8*"
  "\n  %3 = bitcast %wisey.lang.MNullPointerException* %0 to i8*"
  "\n  %4 = getelementptr i8, i8* %3, i64 -8"
  "\n  %5 = call i8* @__cxa_allocate_exception(i64 add (i64 ptrtoint (i1** getelementptr (i1*, i1** null, i32 1) to i64), i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64)))"
  "\n  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %5, i8* %4, i64 add (i64 ptrtoint (i1** getelementptr (i1*, i1** null, i32 1) to i64), i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64)), i32 4, i1 false)"
  "\n  invoke void @__cxa_throw(i8* %5, i8* %2, i8* null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\nif.end:                                           ; preds = %entry"
  "\n  ret void"
  "\n"
  "\ncleanup:                                          ; preds = %if.then"
  "\n  %6 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %7 = alloca { i8*, i32 }"
  "\n  store { i8*, i32 } %6, { i8*, i32 }* %7"
  "\n  %8 = getelementptr { i8*, i32 }, { i8*, i32 }* %7, i32 0, i32 0"
  "\n  %9 = load i8*, i8** %8"
  "\n  %10 = call i8* @__cxa_get_exception_ptr(i8* %9)"
  "\n  %11 = getelementptr i8, i8* %10, i64 8"
  "\n  %12 = load %wisey.lang.MNullPointerException*, %wisey.lang.MNullPointerException** %1"
  "\n  %13 = bitcast %wisey.lang.MNullPointerException* %12 to i8*"
  "\n  call void @__destroyObjectOwnerFunction(i8* %13, i8* %11)"
  "\n  resume { i8*, i32 } %6"
  "\n"
  "\ninvoke.continue:                                  ; preds = %if.then"
  "\n  unreachable"
  "\n}\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

