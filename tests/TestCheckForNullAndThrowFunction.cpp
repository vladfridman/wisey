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
#include "wisey/ProgramPrefix.hpp"

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
  
  ~CheckForNullAndThrowFunctionTest() {
  }
};

TEST_F(CheckForNullAndThrowFunctionTest, callTest) {
  Value* nullPointerValue = ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  CheckForNullAndThrowFunction::call(mContext, nullPointerValue, 0);
  
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
  "\ndefine internal void @__checkForNullAndThrow(i8* %pointer) "
  "personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %cmp = icmp eq i8* %pointer, null"
  "\n  br i1 %cmp, label %if.then, label %if.end"
  "\n"
  "\nif.then:                                          ; preds = %entry"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint "
  "(i64* getelementptr (i64, i64* null, i32 1) to i64))"
  "\n  %buildervar = bitcast i8* %malloccall to %wisey.lang.MNullPointerException*"
  "\n  %0 = getelementptr %wisey.lang.MNullPointerException, "
  "%wisey.lang.MNullPointerException* %buildervar, i32 0, i32 0"
  "\n  store i64 0, i64* %0"
  "\n  %1 = alloca %wisey.lang.MNullPointerException*"
  "\n  store %wisey.lang.MNullPointerException* %buildervar, %wisey.lang.MNullPointerException** %1"
  "\n  %2 = bitcast %wisey.lang.MNullPointerException* %buildervar to i8*"
  "\n  %3 = bitcast { i8*, i8* }* @wisey.lang.MNullPointerException.rtti to i8*"
  "\n  %4 = getelementptr %wisey.lang.MNullPointerException, "
  "%wisey.lang.MNullPointerException* null, i32 1"
  "\n  %5 = ptrtoint %wisey.lang.MNullPointerException* %4 to i64"
  "\n  %6 = call i8* @__cxa_allocate_exception(i64 %5)"
  "\n  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %6, i8* %2, i64 %5, i32 4, i1 false)"
  "\n  invoke void @__cxa_throw(i8* %6, i8* %3, i8* null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\nif.end:                                           ; preds = %entry"
  "\n  ret void"
  "\n"
  "\ncleanup:                                          ; preds = %if.then"
  "\n  %7 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %8 = load %wisey.lang.MNullPointerException*, %wisey.lang.MNullPointerException** %1"
  "\n  call void @destructor.wisey.lang.MNullPointerException("
  "%wisey.lang.MNullPointerException* %8)"
  "\n  resume { i8*, i32 } %7"
  "\n"
  "\ninvoke.continue:                                  ; preds = %if.then"
  "\n  unreachable"
  "\n}\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

