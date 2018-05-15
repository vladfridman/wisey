//
//  TestCheckArrayNotReferencedFunction.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link CheckArrayNotReferencedFunction}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestPrefix.hpp"
#include "MockType.hpp"
#include "wisey/ArrayType.hpp"
#include "wisey/CheckArrayNotReferencedFunction.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct CheckArrayNotReferencedFunctionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  Function* mFunction;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  NiceMock<MockType> mWithArrayType;
  wisey::ArrayType* mArrayType;
  
  CheckArrayNotReferencedFunctionTest() :
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

    EXPECT_CALL(mWithArrayType, die());
  }
  
  ~CheckArrayNotReferencedFunctionTest() {
  }
};

TEST_F(CheckArrayNotReferencedFunctionTest, callTest) {
  llvm::PointerType* genericPointer = llvm::Type::getInt64Ty(mLLVMContext)->getPointerTo();
  Value* nullPointerValue = ConstantPointerNull::get(genericPointer);
  Value* two = ConstantInt::get(Type::getInt64Ty(mLLVMContext), 2);
  Value* namePointer = ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  CheckArrayNotReferencedFunction::call(mContext, nullPointerValue, two, namePointer);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  call void @__checkArrayNotReferenced(i64* null, i64 2, i8* null)\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(CheckArrayNotReferencedFunctionTest, getTest) {
  Function* function = CheckArrayNotReferencedFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "\ndefine void @__checkArrayNotReferenced(i64* %arrayPointer, i64 %noOfDimensions, i8* %arrayName) personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %isNull = icmp eq i64* %arrayPointer, null"
  "\n  br i1 %isNull, label %return.void, label %if.not.null"
  "\n"
  "\nreturn.void:                                      ; preds = %for.cond, %ref.count.zero, %entry"
  "\n  ret void"
  "\n"
  "\nif.not.null:                                      ; preds = %entry"
  "\n  %refCount = load i64, i64* %arrayPointer"
  "\n  %isZero = icmp eq i64 %refCount, 0"
  "\n  br i1 %isZero, label %ref.count.zero, label %ref.count.notzero"
  "\n"
  "\nref.count.zero:                                   ; preds = %if.not.null"
  "\n  %cmp = icmp sgt i64 %noOfDimensions, 1"
  "\n  br i1 %cmp, label %multi.dimensional, label %return.void"
  "\n"
  "\nref.count.notzero:                                ; preds = %if.not.null"
  "\n  invoke void @__throwReferenceCountException(i64 %refCount, i8* %arrayName)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\nfor.cond:                                         ; preds = %for.body, %multi.dimensional"
  "\n  %offset = load i64, i64* %offsetStore"
  "\n  %index = load i64, i64* %indexStore"
  "\n  %cmp1 = icmp slt i64 %index, %size"
  "\n  br i1 %cmp1, label %for.body, label %return.void"
  "\n"
  "\nfor.body:                                         ; preds = %for.cond"
  "\n  %0 = getelementptr i8, i8* %5, i64 %offset"
  "\n  %newIndex = add i64 %index, 1"
  "\n  store i64 %newIndex, i64* %indexStore"
  "\n  %offsetIncrement = add i64 %offset, %elementSize"
  "\n  store i64 %offsetIncrement, i64* %offsetStore"
  "\n  %1 = bitcast i8* %0 to i64*"
  "\n  call void @__checkArrayNotReferenced(i64* %1, i64 %dimensionsMinusOne, i8* %arrayName)"
  "\n  br label %for.cond"
  "\n"
  "\nmulti.dimensional:                                ; preds = %ref.count.zero"
  "\n  %2 = getelementptr i64, i64* %arrayPointer, i64 1"
  "\n  %size = load i64, i64* %2"
  "\n  %3 = getelementptr i64, i64* %arrayPointer, i64 2"
  "\n  %elementSize = load i64, i64* %3"
  "\n  %dimensionsMinusOne = sub i64 %noOfDimensions, 1"
  "\n  %4 = getelementptr i64, i64* %arrayPointer, i64 3"
  "\n  %5 = bitcast i64* %4 to i8*"
  "\n  %indexStore = alloca i64"
  "\n  store i64 0, i64* %indexStore"
  "\n  %offsetStore = alloca i64"
  "\n  store i64 0, i64* %offsetStore"
  "\n  br label %for.cond"
  "\n"
  "\ncleanup:                                          ; preds = %ref.count.notzero"
  "\n  %6 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  resume { i8*, i32 } %6"
  "\n"
  "\ninvoke.continue:                                  ; preds = %ref.count.notzero"
  "\n  unreachable"
  "\n}\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
