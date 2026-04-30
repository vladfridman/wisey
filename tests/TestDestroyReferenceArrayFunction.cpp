//
//  TestDestroyReferenceArrayFunction.cpp
//  runtests
//
//  Created by Vladimir Fridman on 1/15/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link DestroyReferenceArrayFunction}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestPrefix.hpp"
#include "DestroyReferenceArrayFunction.hpp"
#include "IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct DestroyReferenceArrayFunctionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  Function* mFunction;
  Function* mDestructor;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  DestroyReferenceArrayFunctionTest() :
  mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    vector<Type*> destructorArgumentTypes;
    destructorArgumentTypes.push_back(Type::getInt8Ty(mLLVMContext)->getPointerTo());
    Type* detructorLlvmReturnType = Type::getVoidTy(mLLVMContext);
    FunctionType* destructorFunctionType = FunctionType::get(detructorLlvmReturnType,
                                                             destructorArgumentTypes,
                                                             false);
    mDestructor = Function::Create(destructorFunctionType,
                                   GlobalValue::ExternalLinkage,
                                   "destructor",
                                   mContext.getModule());
    
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
  
  ~DestroyReferenceArrayFunctionTest() {
  }
};

TEST_F(DestroyReferenceArrayFunctionTest, callTest) {
  llvm::PointerType* i64PointerType = llvm::Type::getInt64Ty(mLLVMContext)->getPointerTo();
  llvm::PointerType* i8PointerType = llvm::Type::getInt8Ty(mLLVMContext)->getPointerTo();
  Value* arrayPointer = ConstantPointerNull::get(i64PointerType);
  Value* nullPointer = ConstantPointerNull::get(i8PointerType);
  DestroyReferenceArrayFunction::call(mContext, arrayPointer, 2u, nullPointer, nullPointer, 0);
  
  *mStringStream << *mBasicBlock;
  string expected =
  ""
  "\nentry:                                            ; No predecessors!"
  "\n  call void @__destroyReferenceArrayFunction(ptr null, i64 2, ptr null, i1 true, ptr null)"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(DestroyReferenceArrayFunctionTest, getTest) {
  mContext.getScopes().popScope(mContext, 0);
  mContext.getScopes().pushScope();
  
  Function* function = DestroyReferenceArrayFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "define void @__destroyReferenceArrayFunction(ptr %arrayPointer, i64 %noOfDimensions, ptr %arrayName, i1 %shouldFree, ptr %exception) personality ptr @__gxx_personality_v0 {"
  "\ndeclarations:"
  "\n  %indexStore = alloca i64, align 8"
  "\n  %offsetStore = alloca i64, align 8"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declarations"
  "\n  %isNull = icmp eq ptr %arrayPointer, null"
  "\n  br i1 %isNull, label %return.void, label %if.not.null"
  "\n"
  "\nreturn.void:                                      ; preds = %maybe.free.array, %entry"
  "\n  ret void"
  "\n"
  "\nif.not.null:                                      ; preds = %entry"
  "\n  %0 = getelementptr i64, ptr %arrayPointer, i64 1"
  "\n  %size = load i64, ptr %0, align 4"
  "\n  %1 = getelementptr i64, ptr %arrayPointer, i64 2"
  "\n  %elementSize = load i64, ptr %1, align 4"
  "\n  %dimensionsMinusOne = sub i64 %noOfDimensions, 1"
  "\n  %refCount = load i64, ptr %arrayPointer, align 4"
  "\n  %isZero = icmp eq i64 %refCount, 0"
  "\n  br i1 %isZero, label %ref.count.zero, label %ref.count.notzero"
  "\n"
  "\nref.count.zero:                                   ; preds = %if.not.null"
  "\n  %2 = getelementptr i64, ptr %arrayPointer, i64 3"
  "\n  %3 = bitcast ptr %2 to ptr"
  "\n  store i64 0, ptr %indexStore, align 4"
  "\n  store i64 0, ptr %offsetStore, align 4"
  "\n  br label %for.cond"
  "\n"
  "\nref.count.notzero:                                ; preds = %if.not.null"
  "\n  invoke void @__throwReferenceCountException(i64 %refCount, ptr %arrayName, ptr %exception)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\nfor.cond:                                         ; preds = %if.end, %multi.dimensional, %ref.count.zero"
  "\n  %offset = load i64, ptr %offsetStore, align 4"
  "\n  %index = load i64, ptr %indexStore, align 4"
  "\n  %cmp = icmp slt i64 %index, %size"
  "\n  br i1 %cmp, label %for.body, label %maybe.free.array"
  "\n"
  "\nfor.body:                                         ; preds = %for.cond"
  "\n  %4 = getelementptr i8, ptr %3, i64 %offset"
  "\n  %newIndex = add i64 %index, 1"
  "\n  store i64 %newIndex, ptr %indexStore, align 4"
  "\n  %offsetIncrement = add i64 %offset, %elementSize"
  "\n  store i64 %offsetIncrement, ptr %offsetStore, align 4"
  "\n  %cmp1 = icmp sgt i64 %noOfDimensions, 1"
  "\n  br i1 %cmp1, label %multi.dimensional, label %one.dimensional"
  "\n"
  "\nmulti.dimensional:                                ; preds = %for.body"
  "\n  %5 = bitcast ptr %4 to ptr"
  "\n  call void @__destroyReferenceArrayFunction(ptr %5, i64 %dimensionsMinusOne, ptr %arrayName, i1 false, ptr %exception)"
  "\n  br label %for.cond"
  "\n"
  "\none.dimensional:                                  ; preds = %for.body"
  "\n  %6 = bitcast ptr %4 to ptr"
  "\n  %7 = load ptr, ptr %6, align 8"
  "\n  %8 = icmp eq ptr %7, null"
  "\n  br i1 %8, label %if.end, label %if.notnull"
  "\n"
  "\nmaybe.free.array:                                 ; preds = %for.cond"
  "\n  br i1 %shouldFree, label %free.array, label %return.void"
  "\n"
  "\nfree.array:                                       ; preds = %maybe.free.array"
  "\n  tail call void @free(ptr %arrayPointer)"
  "\n  ret void"
  "\n"
  "\ncleanup:                                          ; preds = %ref.count.notzero"
  "\n  %9 = landingpad { ptr, i32 }"
  "\n          cleanup"
  "\n  %10 = alloca { ptr, i32 }, align 8"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { ptr, i32 } %9, ptr %10, align 8"
  "\n  %11 = getelementptr { ptr, i32 }, ptr %10, i32 0, i32 0"
  "\n  %12 = load ptr, ptr %11, align 8"
  "\n  %13 = call ptr @__cxa_get_exception_ptr(ptr %12)"
  "\n  %14 = getelementptr i8, ptr %13, i64 8"
  "\n  resume { ptr, i32 } %9"
  "\n"
  "\ninvoke.continue:                                  ; preds = %ref.count.notzero"
  "\n  unreachable"
  "\n"
  "\nif.end:                                           ; preds = %if.notnull, %one.dimensional"
  "\n  br label %for.cond"
  "\n"
  "\nif.notnull:                                       ; preds = %one.dimensional"
  "\n  %15 = bitcast ptr %7 to ptr"
  "\n  %16 = getelementptr i64, ptr %15, i64 -1"
  "\n  %17 = atomicrmw add ptr %16, i64 -1 monotonic, align 8"
  "\n  br label %if.end"
  "\n}"
  "\n";

  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
