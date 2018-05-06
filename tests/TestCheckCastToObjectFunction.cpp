//
//  TestCheckCastToObjectFunction.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/25/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link CheckCastToObjectFunction}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestPrefix.hpp"
#include "wisey/CheckCastToObjectFunction.hpp"
#include "wisey/IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct CheckCastToObjectFunctionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  Function* mFunction;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  CheckCastToObjectFunctionTest() :
  mLLVMContext(mContext.getLLVMContext()) {
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
  
  ~CheckCastToObjectFunctionTest() {
  }
};

TEST_F(CheckCastToObjectFunctionTest, callCheckCastToModelTest) {
  Value* nullPointerValue = ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  CheckCastToObjectFunction::callCheckCastToModel(mContext, nullPointerValue);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast i8* null to i8*"
  "\n  invoke void @__checkCastToObject(i8* %0, i8 77, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @__model, i32 0, i32 0))"
  "\n          to label %invoke.continue unwind label %cleanup\n";

  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(CheckCastToObjectFunctionTest, getTest) {
  Function* function = CheckCastToObjectFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "\ndefine void @__checkCastToObject(i8* %object, i8 %letter, i8* %toType) personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %isNull = icmp eq i8* %object, null"
  "\n  br i1 %isNull, label %return.block, label %if.notnull"
  "\n"
  "\nreturn.block:                                     ; preds = %if.notnull, %entry"
  "\n  ret void"
  "\n"
  "\nif.notnull:                                       ; preds = %entry"
  "\n  %0 = call i1 @__isObject(i8* %object, i8 %letter)"
  "\n  br i1 %0, label %return.block, label %if.not.object"
  "\n"
  "\nif.not.object:                                    ; preds = %if.notnull"
  "\n  %1 = call i8* @__getOriginalObjectName(i8* %object)"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint (%wisey.lang.MCastException.refCounter* getelementptr (%wisey.lang.MCastException.refCounter, %wisey.lang.MCastException.refCounter* null, i32 1) to i64))"
  "\n  %buildervar = bitcast i8* %malloccall to %wisey.lang.MCastException.refCounter*"
  "\n  %2 = bitcast %wisey.lang.MCastException.refCounter* %buildervar to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %2, i8 0, i64 ptrtoint (%wisey.lang.MCastException.refCounter* getelementptr (%wisey.lang.MCastException.refCounter, %wisey.lang.MCastException.refCounter* null, i32 1) to i64), i32 4, i1 false)"
  "\n  %3 = getelementptr %wisey.lang.MCastException.refCounter, %wisey.lang.MCastException.refCounter* %buildervar, i32 0, i32 1"
  "\n  %4 = getelementptr %wisey.lang.MCastException, %wisey.lang.MCastException* %3, i32 0, i32 1"
  "\n  store i8* %1, i8** %4"
  "\n  %5 = getelementptr %wisey.lang.MCastException, %wisey.lang.MCastException* %3, i32 0, i32 2"
  "\n  store i8* %toType, i8** %5"
  "\n  %6 = bitcast %wisey.lang.MCastException* %3 to i8*"
  "\n  %7 = getelementptr i8, i8* %6, i64 0"
  "\n  %8 = bitcast i8* %7 to i32 (...)***"
  "\n  %9 = getelementptr { [3 x i8*] }, { [3 x i8*] }* @wisey.lang.MCastException.vtable, i32 0, i32 0, i32 0"
  "\n  %10 = bitcast i8** %9 to i32 (...)**"
  "\n  store i32 (...)** %10, i32 (...)*** %8"
  "\n  %11 = alloca %wisey.lang.MCastException*"
  "\n  store %wisey.lang.MCastException* %3, %wisey.lang.MCastException** %11"
  "\n  %12 = bitcast { i8*, i8* }* @wisey.lang.MCastException.rtti to i8*"
  "\n  %13 = bitcast %wisey.lang.MCastException* %3 to i8*"
  "\n  %14 = getelementptr i8, i8* %13, i64 -8"
  "\n  %15 = call i8* @__cxa_allocate_exception(i64 add (i64 mul nuw (i64 ptrtoint (i1** getelementptr (i1*, i1** null, i32 1) to i64), i64 3), i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64)))"
  "\n  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %15, i8* %14, i64 add (i64 mul nuw (i64 ptrtoint (i1** getelementptr (i1*, i1** null, i32 1) to i64), i64 3), i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64)), i32 4, i1 false)"
  "\n  invoke void @__cxa_throw(i8* %15, i8* %12, i8* null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %if.not.object"
  "\n  %16 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %17 = load %wisey.lang.MCastException*, %wisey.lang.MCastException** %11"
  "\n  %18 = bitcast %wisey.lang.MCastException* %17 to i8*"
  "\n  call void @__destroyObjectOwnerFunction(i8* %18)"
  "\n  resume { i8*, i32 } %16"
  "\n"
  "\ninvoke.continue:                                  ; preds = %if.not.object"
  "\n  unreachable"
  "\n}\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
