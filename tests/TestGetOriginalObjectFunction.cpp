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

#include "wisey/GetOriginalObjectFunction.hpp"
#include "wisey/IRGenerationContext.hpp"

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
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~GetOriginalObjectFunctionTest() {
  }
};

TEST_F(GetOriginalObjectFunctionTest, callGetObjectTest) {
  Value* nullPointerValue = ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  GetOriginalObjectFunction::callGetObject(mContext, nullPointerValue);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast i8* null to i8*"
  "\n  %1 = call i8* @__getOriginalObject(i8* %0, i64 -8)\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(GetOriginalObjectFunctionTest, callGetVTableTest) {
  Value* nullPointerValue = ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  GetOriginalObjectFunction::callGetVTable(mContext, nullPointerValue);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast i8* null to i8*"
  "\n  %1 = call i8* @__getOriginalObject(i8* %0, i64 0)\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(GetOriginalObjectFunctionTest, getTest) {
  Function* function = GetOriginalObjectFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "\ndefine internal i8* @__getOriginalObject(i8* %pointer, i64 %adjustment) {"
  "\nentry:"
  "\n  %0 = bitcast i8* %pointer to i8***"
  "\n  %vtable = load i8**, i8*** %0"
  "\n  %1 = getelementptr i8*, i8** %vtable, i64 0"
  "\n  %unthunkbypointer = load i8*, i8** %1"
  "\n  %unthunkby = ptrtoint i8* %unthunkbypointer to i64"
  "\n  %2 = add i64 %unthunkby, %adjustment"
  "\n  %3 = getelementptr i8, i8* %pointer, i64 %2"
  "\n  ret i8* %3"
  "\n}\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
