//
//  TestAdjustReferenceCountFunction.cpp
//  runtests
//
//  Created by Vladimir Fridman on 12/1/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link AdjustReferenceCountFunction}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestPrefix.hpp"
#include "AdjustReferenceCountFunction.hpp"
#include "IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct AdjustReferenceCountFunctionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  Function* mFunction;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  AdjustReferenceCountFunctionTest() :
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
  
  ~AdjustReferenceCountFunctionTest() {
  }
};

TEST_F(AdjustReferenceCountFunctionTest, callTest) {
  Value* nullPointerValue = ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  AdjustReferenceCountFunction::call(mContext, nullPointerValue, 1);
  
  *mStringStream << *mBasicBlock;
  string expected =
  ""
  "\nentry:                                            ; No predecessors!"
  "\n  call void @__adjustReferenceCounter(ptr null, i64 1)"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(AdjustReferenceCountFunctionTest, getTest) {
  mContext.getScopes().popScope(mContext, 0);
  mContext.getScopes().pushScope();
  
  Function* function = AdjustReferenceCountFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "define void @__adjustReferenceCounter(ptr %object, i64 %adjustment) {"
  "\nentry:"
  "\n  %0 = icmp eq ptr %object, null"
  "\n  br i1 %0, label %if.null, label %if.notnull"
  "\n"
  "\nif.null:                                          ; preds = %entry"
  "\n  ret void"
  "\n"
  "\nif.notnull:                                       ; preds = %entry"
  "\n  %1 = call ptr @__getOriginalObject(ptr %object)"
  "\n  %2 = bitcast ptr %1 to ptr"
  "\n  %3 = getelementptr i64, ptr %2, i64 -1"
  "\n  %4 = call i1 @__isObject(ptr %object, i8 109)"
  "\n  br i1 %4, label %safe.adjust, label %if.not.model"
  "\n"
  "\nif.not.model:                                     ; preds = %if.notnull"
  "\n  %5 = call i1 @__isObject(ptr %object, i8 116)"
  "\n  br i1 %5, label %safe.adjust, label %unsafe.adjust"
  "\n"
  "\nsafe.adjust:                                      ; preds = %if.not.model, %if.notnull"
  "\n  %6 = atomicrmw add ptr %3, i64 %adjustment monotonic, align 8"
  "\n  ret void"
  "\n"
  "\nunsafe.adjust:                                    ; preds = %if.not.model"
  "\n  %count = load i64, ptr %3, align 4"
  "\n  %7 = add i64 %count, %adjustment"
  "\n  store i64 %7, ptr %3, align 4"
  "\n  ret void"
  "\n}"
  "\n";

  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
