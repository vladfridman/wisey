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
#include "ArrayType.hpp"
#include "CheckArrayNotReferencedFunction.hpp"
#include "IRGenerationContext.hpp"
#include "PrimitiveTypes.hpp"

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
    BasicBlock* declareBlock = BasicBlock::Create(mLLVMContext, "declare", mFunction);
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setDeclarationsBlock(declareBlock);
    mContext.setBasicBlock(mBasicBlock);
    
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
  Value* nullPointer = ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  CheckArrayNotReferencedFunction::call(mContext,
                                        nullPointerValue,
                                        two,
                                        nullPointer,
                                        nullPointer,
                                        0);
  
  *mStringStream << *mBasicBlock;
  string expected =
  ""
  "\nentry:                                            ; No predecessors!"
  "\n  call void @__checkArrayNotReferenced(ptr null, i64 2, ptr null, ptr null)"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(CheckArrayNotReferencedFunctionTest, getTest) {
  mContext.getScopes().popScope(mContext, 0);
  mContext.getScopes().pushScope();
  
  Function* function = CheckArrayNotReferencedFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "define void @__checkArrayNotReferenced(ptr %arrayPointer, i64 %noOfDimensions, ptr %arrayName, ptr %exception) personality ptr @__gxx_personality_v0 {"
  "\ndeclarations:"
  "\n  %indexStore = alloca i64, align 8"
  "\n  %offsetStore = alloca i64, align 8"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declarations"
  "\n  %isNull = icmp eq ptr %arrayPointer, null"
  "\n  br i1 %isNull, label %return.void, label %if.not.null"
  "\n"
  "\nreturn.void:                                      ; preds = %for.cond, %ref.count.zero, %entry"
  "\n  ret void"
  "\n"
  "\nif.not.null:                                      ; preds = %entry"
  "\n  %refCount = load i64, ptr %arrayPointer, align 4"
  "\n  %isZero = icmp eq i64 %refCount, 0"
  "\n  br i1 %isZero, label %ref.count.zero, label %ref.count.notzero"
  "\n"
  "\nref.count.zero:                                   ; preds = %if.not.null"
  "\n  %cmp = icmp sgt i64 %noOfDimensions, 1"
  "\n  br i1 %cmp, label %multi.dimensional, label %return.void"
  "\n"
  "\nref.count.notzero:                                ; preds = %if.not.null"
  "\n  invoke void @__throwReferenceCountException(i64 %refCount, ptr %arrayName, ptr %exception)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\nfor.cond:                                         ; preds = %for.body, %multi.dimensional"
  "\n  %offset = load i64, ptr %offsetStore, align 4"
  "\n  %index = load i64, ptr %indexStore, align 4"
  "\n  %cmp1 = icmp slt i64 %index, %size"
  "\n  br i1 %cmp1, label %for.body, label %return.void"
  "\n"
  "\nfor.body:                                         ; preds = %for.cond"
  "\n  %0 = getelementptr i8, ptr %5, i64 %offset"
  "\n  %newIndex = add i64 %index, 1"
  "\n  store i64 %newIndex, ptr %indexStore, align 4"
  "\n  %offsetIncrement = add i64 %offset, %elementSize"
  "\n  store i64 %offsetIncrement, ptr %offsetStore, align 4"
  "\n  %1 = bitcast ptr %0 to ptr"
  "\n  call void @__checkArrayNotReferenced(ptr %1, i64 %dimensionsMinusOne, ptr %arrayName, ptr %exception)"
  "\n  br label %for.cond"
  "\n"
  "\nmulti.dimensional:                                ; preds = %ref.count.zero"
  "\n  %2 = getelementptr i64, ptr %arrayPointer, i64 1"
  "\n  %size = load i64, ptr %2, align 4"
  "\n  %3 = getelementptr i64, ptr %arrayPointer, i64 2"
  "\n  %elementSize = load i64, ptr %3, align 4"
  "\n  %dimensionsMinusOne = sub i64 %noOfDimensions, 1"
  "\n  %4 = getelementptr i64, ptr %arrayPointer, i64 3"
  "\n  %5 = bitcast ptr %4 to ptr"
  "\n  store i64 0, ptr %indexStore, align 4"
  "\n  store i64 0, ptr %offsetStore, align 4"
  "\n  br label %for.cond"
  "\n"
  "\ncleanup:                                          ; preds = %ref.count.notzero"
  "\n  %6 = landingpad { ptr, i32 }"
  "\n          cleanup"
  "\n  %7 = alloca { ptr, i32 }, align 8"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { ptr, i32 } %6, ptr %7, align 8"
  "\n  %8 = getelementptr { ptr, i32 }, ptr %7, i32 0, i32 0"
  "\n  %9 = load ptr, ptr %8, align 8"
  "\n  %10 = call ptr @__cxa_get_exception_ptr(ptr %9)"
  "\n  %11 = getelementptr i8, ptr %10, i64 8"
  "\n  resume { ptr, i32 } %6"
  "\n"
  "\ninvoke.continue:                                  ; preds = %ref.count.notzero"
  "\n  unreachable"
  "\n}"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
