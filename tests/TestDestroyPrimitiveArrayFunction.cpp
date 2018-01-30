//
//  TestDestroyPrimitiveArrayFunction.cpp
//  runtests
//
//  Created by Vladimir Fridman on 1/15/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link DestroyPrimitiveArrayFunction}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestPrefix.hpp"
#include "wisey/DestroyPrimitiveArrayFunction.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/ProgramPrefix.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct DestroyPrimitiveArrayFunctionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  Function* mFunction;
  Function* mDestructor;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  DestroyPrimitiveArrayFunctionTest() :
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
  
  ~DestroyPrimitiveArrayFunctionTest() {
  }
};

TEST_F(DestroyPrimitiveArrayFunctionTest, callTest) {
  llvm::PointerType* genericPointer = llvm::Type::getInt64Ty(mLLVMContext)->getPointerTo();
  Value* nullPointerValue = ConstantPointerNull::get(genericPointer);
  DestroyPrimitiveArrayFunction::call(mContext, nullPointerValue, 2u, PrimitiveTypes::LONG_TYPE);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = call i64 @__destroyPrimitiveArrayFunction(i64* null, i64 2, i64 8, i1 true)\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(DestroyPrimitiveArrayFunctionTest, getTest) {
  Function* function = DestroyPrimitiveArrayFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "\ndefine internal i64 @__destroyPrimitiveArrayFunction(i64* %arrayPointer, i64 %noOfDimensions, i64 %primitiveSize, i1 %shouldFree) personality i32 (...)* @__gxx_personality_v0 {"
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
  "\n  %1 = getelementptr i64, i64* %arrayPointer, i32 2"
  "\n  %2 = bitcast i64* %1 to i1*"
  "\n  %indexStore = alloca i64"
  "\n  store i64 0, i64* %indexStore"
  "\n  %offsetStore = alloca i64"
  "\n  store i64 0, i64* %offsetStore"
  "\n  %cmp = icmp sgt i64 %noOfDimensions, 1"
  "\n  br i1 %cmp, label %for.cond, label %one.dimensional"
  "\n"
  "\nref.count.notzero:                                ; preds = %if.not.null"
  "\n  invoke void @__throwReferenceCountException(i64 %refCount)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\nfor.cond:                                         ; preds = %for.body, %ref.count.zero"
  "\n  %index = load i64, i64* %indexStore"
  "\n  %cmp1 = icmp slt i64 %index, %size"
  "\n  br i1 %cmp1, label %for.body, label %maybe.free.array"
  "\n"
  "\nfor.body:                                         ; preds = %for.cond"
  "\n  %offset = load i64, i64* %offsetStore"
  "\n  %3 = getelementptr i1, i1* %2, i64 %offset"
  "\n  %newIndex = add i64 %index, 1"
  "\n  store i64 %newIndex, i64* %indexStore"
  "\n  %4 = bitcast i1* %3 to i64*"
  "\n  %5 = call i64 @__destroyPrimitiveArrayFunction(i64* %4, i64 %dimensionsMinusOne, i64 %primitiveSize, i1 false)"
  "\n  %offsetIncrement = add i64 %offset, %5"
  "\n  store i64 %offsetIncrement, i64* %offsetStore"
  "\n  br label %for.cond"
  "\n"
  "\none.dimensional:                                  ; preds = %ref.count.zero"
  "\n  %sizeInBytes = mul i64 %size, %primitiveSize"
  "\n  %6 = srem i64 %sizeInBytes, 8"
  "\n  %cmp2 = icmp eq i64 %6, 0"
  "\n  br i1 %cmp2, label %reminder.zero, label %reminder.non.zero"
  "\n"
  "\nmaybe.free.array:                                 ; preds = %reminder.non.zero, %reminder.zero, %for.cond"
  "\n  %offset3 = load i64, i64* %offsetStore"
  "\n  %offsetPlusTwo = add i64 %offset3, 16"
  "\n  br i1 %shouldFree, label %free.array, label %dont.free.array"
  "\n"
  "\nreminder.zero:                                    ; preds = %one.dimensional"
  "\n  store i64 %sizeInBytes, i64* %offsetStore"
  "\n  br label %maybe.free.array"
  "\n"
  "\nreminder.non.zero:                                ; preds = %one.dimensional"
  "\n  %7 = sub i64 8, %6"
  "\n  %8 = add i64 %sizeInBytes, %7"
  "\n  store i64 %8, i64* %offsetStore"
  "\n  br label %maybe.free.array"
  "\n"
  "\nfree.array:                                       ; preds = %maybe.free.array"
  "\n  %9 = bitcast i64* %arrayPointer to i8*"
  "\n  tail call void @free(i8* %9)"
  "\n  ret i64 %offsetPlusTwo"
  "\n"
  "\ndont.free.array:                                  ; preds = %maybe.free.array"
  "\n  ret i64 %offsetPlusTwo"
  "\n"
  "\ncleanup:                                          ; preds = %ref.count.notzero"
  "\n  %10 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  resume { i8*, i32 } %10"
  "\n"
  "\ninvoke.continue:                                  ; preds = %ref.count.notzero"
  "\n  unreachable"
  "\n}\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
