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
  DestroyOwnerArrayFunction::call(mContext, nullPointerValue, mDestructor);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  call void @__destroyOwnerArrayFunction(i64* null, void (i8*)* @destructor)\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(DestroyOwnerArrayFunctionTest, getTest) {
  Function* function = DestroyOwnerArrayFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "\ndefine internal void @__destroyOwnerArrayFunction(i64* %arrayPointer, void (i8*)* %destructor) personality i32 (...)* @__gxx_personality_v0 {"
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
  "\n  call void %destructor(i8* %10)"
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

