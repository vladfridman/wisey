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
#include "wisey/AdjustReferenceCountFunction.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/ProgramPrefix.hpp"

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
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
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
  }
  
  ~AdjustReferenceCountFunctionTest() {
  }
};

TEST_F(AdjustReferenceCountFunctionTest, callTest) {
  Value* nullPointerValue = ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  AdjustReferenceCountFunction::call(mContext, nullPointerValue, 1);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast i8* null to i8*"
  "\n  call void @__adjustReferenceCounter(i8* %0, i64 1)\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(AdjustReferenceCountFunctionTest, getTest) {
  Function* function = AdjustReferenceCountFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "\ndefine internal void @__adjustReferenceCounter(i8* %object, i64 %adjustment) {"
  "\nentry:"
  "\n  %0 = icmp eq i8* %object, null"
  "\n  br i1 %0, label %if.null, label %if.notnull"
  "\n"
  "\nif.null:                                          ; preds = %entry"
  "\n  ret void"
  "\n"
  "\nif.notnull:                                       ; preds = %entry"
  "\n  %1 = bitcast i8* %object to i8*"
  "\n  %2 = call i8* @__getOriginalObject(i8* %1)"
  "\n  %3 = bitcast i8* %2 to i64*"
  "\n  %4 = getelementptr i64, i64* %3, i64 -1"
  "\n  %5 = bitcast i8* %2 to i8***"
  "\n  %vtable = load i8**, i8*** %5"
  "\n  %6 = getelementptr i8*, i8** %vtable, i64 1"
  "\n  %typeArrayI8 = load i8*, i8** %6"
  "\n  %7 = bitcast i8* %typeArrayI8 to i8**"
  "\n  %stringPointer = load i8*, i8** %7"
  "\n  %firstLetter = load i8, i8* %stringPointer"
  "\n  %8 = icmp eq i8 %firstLetter, 77"
  "\n  br i1 %8, label %if.model, label %if.not.model"
  "\n"
  "\nif.model:                                         ; preds = %if.notnull"
  "\n  %9 = atomicrmw add i64* %4, i64 %adjustment monotonic"
  "\n  ret void"
  "\n"
  "\nif.not.model:                                     ; preds = %if.notnull"
  "\n  %count = load i64, i64* %4"
  "\n  %10 = add i64 %count, %adjustment"
  "\n  store i64 %10, i64* %4"
  "\n  ret void"
  "\n}\n";

  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}


