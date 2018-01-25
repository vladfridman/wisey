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
#include "wisey/DestroyReferenceArrayFunction.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/ProgramPrefix.hpp"

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
  
  ~DestroyReferenceArrayFunctionTest() {
  }
};

TEST_F(DestroyReferenceArrayFunctionTest, callTest) {
  llvm::PointerType* genericPointer = llvm::Type::getInt64Ty(mLLVMContext)->getPointerTo();
  Value* nullPointerValue = ConstantPointerNull::get(genericPointer);
  DestroyReferenceArrayFunction::call(mContext, nullPointerValue);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  call void @__destroyReferenceArrayFunction(i64* null)\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(DestroyReferenceArrayFunctionTest, getTest) {
  Function* function = DestroyReferenceArrayFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "\ndefine internal void @__destroyReferenceArrayFunction(i64* %arrayPointer) personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %0 = icmp eq i64* %arrayPointer, null"
  "\n  br i1 %0, label %if.null, label %if.not.null"
  "\n"
  "\nif.null:                                          ; preds = %entry"
  "\n  ret void"
  "\n"
  "\nif.not.null:                                      ; preds = %entry"
  "\n  %1 = getelementptr i64, i64* %arrayPointer, i64 1"
  "\n  %dimensions = load i64, i64* %1"
  "\n  %offset = add i64 %dimensions, 2"
  "\n  %2 = getelementptr i64, i64* %arrayPointer, i64 2"
  "\n  %size = load i64, i64* %2"
  "\n  %3 = load i64, i64* %arrayPointer"
  "\n  %4 = icmp eq i64 %3, 0"
  "\n  br i1 %4, label %ref.count.zero, label %ref.count.notzero"
  "\n"
  "\nref.count.zero:                                   ; preds = %if.not.null"
  "\n  %5 = getelementptr i64, i64* %1, i64 %offset"
  "\n  %6 = bitcast i64* %5 to [0 x i8*]*"
  "\n  %7 = alloca i64"
  "\n  store i64 0, i64* %7"
  "\n  br label %for.cond"
  "\n"
  "\nref.count.notzero:                                ; preds = %if.not.null"
  "\n  invoke void @__throwReferenceCountException(i64 %3)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\nfor.cond:                                         ; preds = %for.body, %ref.count.zero"
  "\n  %8 = load i64, i64* %7"
  "\n  %cmp = icmp slt i64 %8, %size"
  "\n  br i1 %cmp, label %for.body, label %free.array"
  "\n"
  "\nfor.body:                                         ; preds = %for.cond"
  "\n  %9 = getelementptr [0 x i8*], [0 x i8*]* %6, i64 0, i64 %8"
  "\n  %10 = load i8*, i8** %9"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* %10, i64 -1)"
  "\n  %11 = add i64 %8, 1"
  "\n  store i64 %11, i64* %7"
  "\n  br label %for.cond"
  "\n"
  "\nfree.array:                                       ; preds = %for.cond"
  "\n  %12 = bitcast i64* %arrayPointer to i8*"
  "\n  tail call void @free(i8* %12)"
  "\n  ret void"
  "\n"
  "\ncleanup:                                          ; preds = %ref.count.notzero"
  "\n  %13 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  resume { i8*, i32 } %13"
  "\n"
  "\ninvoke.continue:                                  ; preds = %ref.count.notzero"
  "\n  unreachable"
  "\n}\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
