//
//  TestCastObjectFunction.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link CastObjectFunction}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockObjectType.hpp"
#include "TestPrefix.hpp"
#include "CastObjectFunction.hpp"
#include "IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct CastObjectFunctionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mDeclareBlock;
  BasicBlock* mEntryBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  NiceMock<MockObjectType> mToObjectType;
  Function* mFunction;

  CastObjectFunctionTest() : mLLVMContext(mContext.getLLVMContext()) {
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
    
    string toTypeName = "systems.vos.wisey.compiler.tests.IToInterface";
    string toNameGlobal = "systems.vos.wisey.compiler.tests.IToInterface.typename";

    ON_CALL(mToObjectType, getObjectNameGlobalVariableName())
    .WillByDefault(Return(toNameGlobal));
    ON_CALL(mToObjectType, getLLVMType(_))
    .WillByDefault(Return(Type::getInt64Ty(mLLVMContext)->getPointerTo()));

    new GlobalVariable(*mContext.getModule(),
                       llvm::ArrayType::get(Type::getInt8Ty(mLLVMContext), toTypeName.length() + 1),
                       true,
                       GlobalValue::LinkageTypes::ExternalLinkage,
                       nullptr,
                       toNameGlobal);
  }
  
  ~CastObjectFunctionTest() {
  }
};

TEST_F(CastObjectFunctionTest, callTest) {
  llvm::Constant* nullPointerValue =
  ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  CastObjectFunction::call(mContext, nullPointerValue, &mToObjectType, 5);
  BranchInst::Create(mEntryBlock, mDeclareBlock);
  
  *mStringStream << *mFunction;
  string expected =
  "define internal i32 @main() personality ptr @__gxx_personality_v0 {"
  "\ndeclare:"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declare"
  "\n  %0 = invoke ptr @__castObject(ptr null, ptr @systems.vos.wisey.compiler.tests.IToInterface.typename)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %entry"
  "\n  %1 = landingpad { ptr, i32 }"
  "\n          cleanup"
  "\n  %2 = alloca { ptr, i32 }, align 8"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { ptr, i32 } %1, ptr %2, align 8"
  "\n  %3 = getelementptr { ptr, i32 }, ptr %2, i32 0, i32 0"
  "\n  %4 = load ptr, ptr %3, align 8"
  "\n  %5 = call ptr @__cxa_get_exception_ptr(ptr %4)"
  "\n  %6 = getelementptr i8, ptr %5, i64 8"
  "\n  resume { ptr, i32 } %1"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n  %7 = bitcast ptr %0 to ptr"
  "\n}"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(CastObjectFunctionTest, getTest) {
  mContext.getScopes().popScope(mContext, 0);
  mContext.getScopes().pushScope();
  
  Function* function = CastObjectFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "define ptr @__castObject(ptr %fromObjectValue, ptr %toTypeName) personality ptr @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %isNull = icmp eq ptr %fromObjectValue, null"
  "\n  br i1 %isNull, label %if.null, label %if.notnull"
  "\n"
  "\nif.null:                                          ; preds = %entry"
  "\n  ret ptr null"
  "\n"
  "\nif.notnull:                                       ; preds = %entry"
  "\n  %0 = call i32 @__instanceOf(ptr %fromObjectValue, ptr %toTypeName)"
  "\n  %1 = call ptr @__getOriginalObject(ptr %fromObjectValue)"
  "\n  %cmp = icmp slt i32 %0, 0"
  "\n  br i1 %cmp, label %less.than.zero, label %not.less.than.zero"
  "\n"
  "\nless.than.zero:                                   ; preds = %if.notnull"
  "\n  %2 = call ptr @__getOriginalObjectName(ptr %fromObjectValue)"
  "\n  %buildervar = tail call ptr @malloc(i64 ptrtoint (ptr getelementptr (%wisey.lang.MCastException.refCounter, ptr null, i32 1) to i64))"
  "\n  %3 = bitcast ptr %buildervar to ptr"
  "\n  call void @llvm.memset.p0.i64(ptr %3, i8 0, i64 ptrtoint (ptr getelementptr (%wisey.lang.MCastException.refCounter, ptr null, i32 1) to i64), i1 false)"
  "\n  %4 = getelementptr %wisey.lang.MCastException.refCounter, ptr %buildervar, i32 0, i32 1"
  "\n  %5 = getelementptr %wisey.lang.MCastException, ptr %4, i32 0, i32 1"
  "\n  store ptr %2, ptr %5, align 8"
  "\n  %6 = getelementptr %wisey.lang.MCastException, ptr %4, i32 0, i32 2"
  "\n  store ptr %toTypeName, ptr %6, align 8"
  "\n  %7 = bitcast ptr %4 to ptr"
  "\n  %8 = getelementptr i8, ptr %7, i64 0"
  "\n  %9 = bitcast ptr %8 to ptr"
  "\n  %10 = getelementptr { [3 x ptr] }, ptr @wisey.lang.MCastException.vtable, i32 0, i32 0, i32 0"
  "\n  %11 = bitcast ptr %10 to ptr"
  "\n  store ptr %11, ptr %9, align 8"
  "\n  %12 = bitcast ptr @wisey.lang.MCastException.rtti to ptr"
  "\n  %13 = bitcast ptr %4 to ptr"
  "\n  %14 = getelementptr i8, ptr %13, i64 -8"
  "\n  %15 = call ptr @__cxa_allocate_exception(i64 add (i64 ptrtoint (ptr getelementptr (%wisey.lang.MCastException, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)))"
  "\n  call void @llvm.memcpy.p0.p0.i64(ptr %15, ptr %14, i64 add (i64 ptrtoint (ptr getelementptr (%wisey.lang.MCastException, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)), i1 false)"
  "\n  tail call void @free(ptr %14)"
  "\n  invoke void @__cxa_throw(ptr %15, ptr %12, ptr null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\nnot.less.than.zero:                               ; preds = %if.notnull"
  "\n  %cmp1 = icmp sgt i32 %0, 0"
  "\n  br i1 %cmp1, label %more.than.zero, label %zero.exactly"
  "\n"
  "\nmore.than.zero:                                   ; preds = %not.less.than.zero"
  "\n  %16 = sub i32 %0, 1"
  "\n  %17 = bitcast ptr %1 to ptr"
  "\n  %18 = mul i32 %16, 8"
  "\n  %19 = getelementptr i8, ptr %17, i32 %18"
  "\n  ret ptr %19"
  "\n"
  "\nzero.exactly:                                     ; preds = %not.less.than.zero"
  "\n  ret ptr %1"
  "\n"
  "\ncleanup:                                          ; preds = %less.than.zero"
  "\n  %20 = landingpad { ptr, i32 }"
  "\n          cleanup"
  "\n  %21 = alloca { ptr, i32 }, align 8"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { ptr, i32 } %20, ptr %21, align 8"
  "\n  %22 = getelementptr { ptr, i32 }, ptr %21, i32 0, i32 0"
  "\n  %23 = load ptr, ptr %22, align 8"
  "\n  %24 = call ptr @__cxa_get_exception_ptr(ptr %23)"
  "\n  %25 = getelementptr i8, ptr %24, i64 8"
  "\n  resume { ptr, i32 } %20"
  "\n"
  "\ninvoke.continue:                                  ; preds = %less.than.zero"
  "\n  unreachable"
  "\n}"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
