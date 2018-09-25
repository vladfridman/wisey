//
//  TestDestroyOwnerArrayFunction.cpp
//  runtests
//
//  Created by Vladimir Fridman on 12/19/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link DestroyOwnerArrayFunction}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestPrefix.hpp"
#include "DestroyOwnerArrayFunction.hpp"
#include "IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct DestroyOwnerArrayFunctionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  Function* mFunction;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  DestroyOwnerArrayFunctionTest() :
  mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
     
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "main",
                                 mContext.getModule());
    BasicBlock* declareBlock = BasicBlock::Create(mLLVMContext, "declare", mFunction);
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setDeclarationsBlock(declareBlock);
    mContext.setBasicBlock(mBasicBlock);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~DestroyOwnerArrayFunctionTest() {
  }
};

TEST_F(DestroyOwnerArrayFunctionTest, callTest) {
  llvm::PointerType* i64PointerType = llvm::Type::getInt64Ty(mLLVMContext)->getPointerTo();
  llvm::PointerType* i8PointerType = llvm::Type::getInt8Ty(mLLVMContext)->getPointerTo();
  Value* arrayPointer = ConstantPointerNull::get(i64PointerType);
  Value* nullPointer = ConstantPointerNull::get(i8PointerType);
  DestroyOwnerArrayFunction::call(mContext, arrayPointer, 2u, nullPointer, nullPointer, 0);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:                                            ; No predecessors!"
  "\n  call void @__destroyOwnerArrayFunction(i64* null, %wisey.threads.IThread* null, %wisey.threads.CCallStack* null, i64 2, i8* null, i1 true, i8* null)\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(DestroyOwnerArrayFunctionTest, getTest) {
  mContext.getScopes().popScope(mContext, 0);
  mContext.getScopes().pushScope();
  
  Function* function = DestroyOwnerArrayFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "\ndefine void @__destroyOwnerArrayFunction(i64* %arrayPointer, %wisey.threads.IThread* %thread, %wisey.threads.CCallStack* %callstack, i64 %noOfDimensions, i8* %arrayName, i1 %shouldFree, i8* %exception) personality i32 (...)* @__gxx_personality_v0 {"
  "\ndeclarations:"
  "\n  %indexStore = alloca i64"
  "\n  %offsetStore = alloca i64"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declarations"
  "\n  %isNull = icmp eq i64* %arrayPointer, null"
  "\n  br i1 %isNull, label %return.void, label %if.not.null"
  "\n"
  "\nreturn.void:                                      ; preds = %maybe.free.array, %entry"
  "\n  ret void"
  "\n"
  "\nif.not.null:                                      ; preds = %entry"
  "\n  %0 = getelementptr i64, i64* %arrayPointer, i64 1"
  "\n  %size = load i64, i64* %0"
  "\n  %1 = getelementptr i64, i64* %arrayPointer, i64 2"
  "\n  %elementSize = load i64, i64* %1"
  "\n  %dimensionsMinusOne = sub i64 %noOfDimensions, 1"
  "\n  %refCount = load i64, i64* %arrayPointer"
  "\n  %isZero = icmp eq i64 %refCount, 0"
  "\n  br i1 %isZero, label %ref.count.zero, label %ref.count.notzero"
  "\n"
  "\nref.count.zero:                                   ; preds = %if.not.null"
  "\n  %2 = getelementptr i64, i64* %arrayPointer, i64 3"
  "\n  %3 = bitcast i64* %2 to i8*"
  "\n  store i64 0, i64* %indexStore"
  "\n  store i64 0, i64* %offsetStore"
  "\n  br label %for.cond"
  "\n"
  "\nref.count.notzero:                                ; preds = %if.not.null"
  "\n  invoke void @__throwReferenceCountException(i64 %refCount, i8* %arrayName, i8* %exception)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\nfor.cond:                                         ; preds = %one.dimensional, %multi.dimensional, %ref.count.zero"
  "\n  %offset = load i64, i64* %offsetStore"
  "\n  %index = load i64, i64* %indexStore"
  "\n  %cmp = icmp slt i64 %index, %size"
  "\n  br i1 %cmp, label %for.body, label %maybe.free.array"
  "\n"
  "\nfor.body:                                         ; preds = %for.cond"
  "\n  %4 = getelementptr i8, i8* %3, i64 %offset"
  "\n  %newIndex = add i64 %index, 1"
  "\n  store i64 %newIndex, i64* %indexStore"
  "\n  %offsetIncrement = add i64 %offset, %elementSize"
  "\n  store i64 %offsetIncrement, i64* %offsetStore"
  "\n  %cmp1 = icmp sgt i64 %noOfDimensions, 1"
  "\n  br i1 %cmp1, label %multi.dimensional, label %one.dimensional"
  "\n"
  "\nmulti.dimensional:                                ; preds = %for.body"
  "\n  %5 = bitcast i8* %4 to i64*"
  "\n  call void @__destroyOwnerArrayFunction(i64* %5, %wisey.threads.IThread* %thread, %wisey.threads.CCallStack* %callstack, i64 %dimensionsMinusOne, i8* %arrayName, i1 false, i8* %exception)"
  "\n  br label %for.cond"
  "\n"
  "\none.dimensional:                                  ; preds = %for.body"
  "\n  %6 = bitcast i8* %4 to i8**"
  "\n  %7 = load i8*, i8** %6"
  "\n  call void @__destroyObjectOwnerFunction(i8* %7, %wisey.threads.IThread* %thread, %wisey.threads.CCallStack* %callstack, i8* %exception)"
  "\n  br label %for.cond"
  "\n"
  "\nmaybe.free.array:                                 ; preds = %for.cond"
  "\n  br i1 %shouldFree, label %free.array, label %return.void"
  "\n"
  "\nfree.array:                                       ; preds = %maybe.free.array"
  "\n  %8 = bitcast i64* %arrayPointer to i8*"
  "\n  tail call void @free(i8* %8)"
  "\n  ret void"
  "\n"
  "\ncleanup:                                          ; preds = %ref.count.notzero"
  "\n  %9 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %10 = alloca { i8*, i32 }"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { i8*, i32 } %9, { i8*, i32 }* %10"
  "\n  %11 = getelementptr { i8*, i32 }, { i8*, i32 }* %10, i32 0, i32 0"
  "\n  %12 = load i8*, i8** %11"
  "\n  %13 = call i8* @__cxa_get_exception_ptr(i8* %12)"
  "\n  %14 = getelementptr i8, i8* %13, i64 8"
  "\n  resume { i8*, i32 } %9"
  "\n"
  "\ninvoke.continue:                                  ; preds = %ref.count.notzero"
  "\n  unreachable"
  "\n}\n";

  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

