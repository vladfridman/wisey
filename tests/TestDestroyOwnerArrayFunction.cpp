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
#include "wisey/DestroyOwnerArrayFunction.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/ProgramPrefix.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct DestroyOwnerArrayFunctionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  Function* mFunction;
  Function* mDestructor;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  DestroyOwnerArrayFunctionTest() :
  mLLVMContext(mContext.getLLVMContext()) {
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    TestPrefix::generateIR(mContext);
    
    vector<Type*> destructorArgumentTypes;
    destructorArgumentTypes.push_back(Type::getInt8Ty(mLLVMContext)->getPointerTo());
    ArrayRef<Type*> destructorArgTypesArray = ArrayRef<Type*>(destructorArgumentTypes);
    Type* detructorLlvmReturnType = Type::getVoidTy(mLLVMContext);
    FunctionType* destructorFunctionType = FunctionType::get(detructorLlvmReturnType,
                                                             destructorArgTypesArray,
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
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~DestroyOwnerArrayFunctionTest() {
  }
};

TEST_F(DestroyOwnerArrayFunctionTest, callTest) {
  llvm::PointerType* genericPointer = llvm::Type::getInt64Ty(mLLVMContext)->getPointerTo();
  Value* nullPointerValue = ConstantPointerNull::get(genericPointer);
  DestroyOwnerArrayFunction::call(mContext, nullPointerValue, 2u, mDestructor);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = call i64 @__destroyOwnerArrayFunction(i64* null, i64 2, i1 true, void (i8*)* @destructor)\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(DestroyOwnerArrayFunctionTest, getTest) {
  Function* function = DestroyOwnerArrayFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "\ndefine internal i64 @__destroyOwnerArrayFunction(i64* %arrayPointer, i64 %noOfDimensions, i1 %shouldFree, void (i8*)* %destructor) personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %isNull = icmp eq i64* %arrayPointer, null"
  "\n  br i1 %isNull, label %if.null, label %if.not.null"
  "\n"
  "\nif.null:                                          ; preds = %entry"
  "\n  ret i64 0"
  "\n"
  "\nif.not.null:                                      ; preds = %entry"
  "\n  %0 = getelementptr i64, i64* %arrayPointer, i64 1"
  "\n  %size = load i64, i64* %0"
  "\n  %dimensionsMinusOne = sub i64 %noOfDimensions, 1"
  "\n  %refCount = load i64, i64* %arrayPointer"
  "\n  %isZero = icmp eq i64 %refCount, 0"
  "\n  br i1 %isZero, label %ref.count.zero, label %ref.count.notzero"
  "\n"
  "\nref.count.zero:                                   ; preds = %if.not.null"
  "\n  %1 = getelementptr i64, i64* %arrayPointer, i64 2"
  "\n  %2 = bitcast i64* %1 to [0 x i8*]*"
  "\n  %indexStore = alloca i64"
  "\n  store i64 0, i64* %indexStore"
  "\n  %offsetStore = alloca i64"
  "\n  store i64 0, i64* %offsetStore"
  "\n  br label %for.cond"
  "\n"
  "\nref.count.notzero:                                ; preds = %if.not.null"
  "\n  invoke void @__throwReferenceCountException(i64 %refCount)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\nfor.cond:                                         ; preds = %one.dimensional, %multi.dimensional, %ref.count.zero"
  "\n  %offset = load i64, i64* %offsetStore"
  "\n  %index = load i64, i64* %indexStore"
  "\n  %cmp = icmp slt i64 %index, %size"
  "\n  br i1 %cmp, label %for.body, label %maybe.free.array"
  "\n"
  "\nfor.body:                                         ; preds = %for.cond"
  "\n  %3 = getelementptr [0 x i8*], [0 x i8*]* %2, i64 0, i64 %offset"
  "\n  %newIndex = add i64 %index, 1"
  "\n  store i64 %newIndex, i64* %indexStore"
  "\n  %cmp1 = icmp sgt i64 %noOfDimensions, 1"
  "\n  br i1 %cmp1, label %multi.dimensional, label %one.dimensional"
  "\n"
  "\nmulti.dimensional:                                ; preds = %for.body"
  "\n  %4 = bitcast i8** %3 to i64*"
  "\n  %5 = call i64 @__destroyOwnerArrayFunction(i64* %4, i64 %dimensionsMinusOne, i1 false, void (i8*)* %destructor)"
  "\n  %offsetIncrement = add i64 %offset, %5"
  "\n  store i64 %offsetIncrement, i64* %offsetStore"
  "\n  br label %for.cond"
  "\n"
  "\none.dimensional:                                  ; preds = %for.body"
  "\n  %6 = load i8*, i8** %3"
  "\n  call void %destructor(i8* %6)"
  "\n  %offsetIncrement2 = add i64 %offset, 1"
  "\n  store i64 %offsetIncrement2, i64* %offsetStore"
  "\n  br label %for.cond"
  "\n"
  "\nmaybe.free.array:                                 ; preds = %for.cond"
  "\n  %offsetPlusTwo = add i64 %offset, 2"
  "\n  br i1 %shouldFree, label %free.array, label %dont.free.array"
  "\n"
  "\nfree.array:                                       ; preds = %maybe.free.array"
  "\n  %7 = bitcast i64* %arrayPointer to i8*"
  "\n  tail call void @free(i8* %7)"
  "\n  ret i64 %offsetPlusTwo"
  "\n"
  "\ndont.free.array:                                  ; preds = %maybe.free.array"
  "\n  ret i64 %offsetPlusTwo"
  "\n"
  "\ncleanup:                                          ; preds = %ref.count.notzero"
  "\n  %8 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  resume { i8*, i32 } %8"
  "\n"
  "\ninvoke.continue:                                  ; preds = %ref.count.notzero"
  "\n  unreachable"
  "\n}\n";

  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

