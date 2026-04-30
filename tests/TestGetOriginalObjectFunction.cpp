//
//  TestGetOriginalObjectFunction.cpp
//  runtests
//
//  Created by Vladimir Fridman on 12/1/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link GetOriginalObjectFunction}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "GetOriginalObjectFunction.hpp"
#include "IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct GetOriginalObjectFunctionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  GetOriginalObjectFunctionTest() : mLLVMContext(mContext.getLLVMContext()) {
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    BasicBlock* declareBlock = BasicBlock::Create(mLLVMContext, "declare", function);
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setDeclarationsBlock(declareBlock);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~GetOriginalObjectFunctionTest() {
  }
};

TEST_F(GetOriginalObjectFunctionTest, callTest) {
  Value* nullPointerValue = ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  GetOriginalObjectFunction::call(mContext, nullPointerValue);
  
  *mStringStream << *mBasicBlock;
  string expected =
  ""
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = call ptr @__getOriginalObject(ptr null)"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(GetOriginalObjectFunctionTest, getTest) {
  Function* function = GetOriginalObjectFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "define ptr @__getOriginalObject(ptr %pointer) {"
  "\nentry:"
  "\n  %0 = bitcast ptr %pointer to ptr"
  "\n  %vtable = load ptr, ptr %0, align 8"
  "\n  %1 = getelementptr ptr, ptr %vtable, i64 0"
  "\n  %unthunkbypointer = load ptr, ptr %1, align 8"
  "\n  %unthunkby = ptrtoint ptr %unthunkbypointer to i64"
  "\n  %2 = getelementptr i8, ptr %pointer, i64 %unthunkby"
  "\n  ret ptr %2"
  "\n}"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
