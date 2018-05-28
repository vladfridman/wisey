//
//  TestAdjustReferenceCounterForConcreteObjectSafelyFunction.cpp
//  runtests
//
//  Created by Vladimir Fridman on 12/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link AdjustReferenceCounterForConcreteObjectSafelyFunction}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestPrefix.hpp"
#include "wisey/AdjustReferenceCounterForConcreteObjectSafelyFunction.hpp"
#include "wisey/IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct AdjustReferenceCounterForConcreteObjectSafelyFunctionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  Function* mFunction;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  AdjustReferenceCounterForConcreteObjectSafelyFunctionTest() :
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
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~AdjustReferenceCounterForConcreteObjectSafelyFunctionTest() {
  }
};

TEST_F(AdjustReferenceCounterForConcreteObjectSafelyFunctionTest, callTest) {
  Value* nullPointerValue = ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  AdjustReferenceCounterForConcreteObjectSafelyFunction::call(mContext, nullPointerValue, 1);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:                                            ; No predecessors!"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* null, i64 1)\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(AdjustReferenceCounterForConcreteObjectSafelyFunctionTest, getTest) {
  Function* function = AdjustReferenceCounterForConcreteObjectSafelyFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "\ndefine void @__adjustReferenceCounterForConcreteObjectSafely(i8* %object, i64 %adjustment) {"
  "\nentry:"
  "\n  %0 = icmp eq i8* %object, null"
  "\n  br i1 %0, label %if.null, label %if.notnull"
  "\n"
  "\nif.null:                                          ; preds = %entry"
  "\n  ret void"
  "\n"
  "\nif.notnull:                                       ; preds = %entry"
  "\n  %1 = bitcast i8* %object to i64*"
  "\n  %2 = getelementptr i64, i64* %1, i64 -1"
  "\n  %3 = atomicrmw add i64* %2, i64 %adjustment monotonic"
  "\n  ret void"
  "\n}\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
