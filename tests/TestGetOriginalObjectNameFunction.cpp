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

#include "wisey/GetOriginalObjectNameFunction.hpp"
#include "wisey/IRGenerationContext.hpp"

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
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
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
  "\nentry:"
  "\n  %0 = call i8* @__getOriginalObjectName(i8* null)\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(GetOriginalObjectNameFunctionTest, getTest) {
  Function* function = GetOriginalObjectNameFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "\ndefine i8* @__getOriginalObjectName(i8* %object) {"
  "\nentry:"
  "\n  %0 = call i8* @__getOriginalObject(i8* %object)"
  "\n  %1 = bitcast i8* %0 to i8***"
  "\n  %vtable = load i8**, i8*** %1"
  "\n  %2 = getelementptr i8*, i8** %vtable, i64 1"
  "\n  %typeArrayI8 = load i8*, i8** %2"
  "\n  %3 = bitcast i8* %typeArrayI8 to i8**"
  "\n  %4 = getelementptr i8*, i8** %3, i64 1"
  "\n  %name = load i8*, i8** %4"
  "\n  ret i8* %name"
  "\n}\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
