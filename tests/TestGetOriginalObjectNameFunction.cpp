//
//  TestGetOriginalObjectNameFunction.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/24/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link GetOriginalObjectNameFunction}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "GetOriginalObjectNameFunction.hpp"
#include "IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct GetOriginalObjectNameFunctionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  GetOriginalObjectNameFunctionTest() : mLLVMContext(mContext.getLLVMContext()) {
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
  
  ~GetOriginalObjectNameFunctionTest() {
  }
};

TEST_F(GetOriginalObjectNameFunctionTest, callTest) {
  Value* nullPointerValue = ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  GetOriginalObjectNameFunction::call(mContext, nullPointerValue);
  
  *mStringStream << *mBasicBlock;
  string expected =
  ""
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = call ptr @__getOriginalObjectName(ptr null)"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(GetOriginalObjectNameFunctionTest, getTest) {
  Function* function = GetOriginalObjectNameFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "define ptr @__getOriginalObjectName(ptr %object) {"
  "\nentry:"
  "\n  %0 = call ptr @__getOriginalObject(ptr %object)"
  "\n  %1 = bitcast ptr %0 to ptr"
  "\n  %vtable = load ptr, ptr %1, align 8"
  "\n  %2 = getelementptr ptr, ptr %vtable, i64 1"
  "\n  %typeArrayI8 = load ptr, ptr %2, align 8"
  "\n  %3 = bitcast ptr %typeArrayI8 to ptr"
  "\n  %4 = getelementptr ptr, ptr %3, i64 1"
  "\n  %name = load ptr, ptr %4, align 8"
  "\n  ret ptr %name"
  "\n}"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
