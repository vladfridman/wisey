//
//  TestDestroyObjectOwnerFunction.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/30/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link DestroyObjectOwnerFunction}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestPrefix.hpp"
#include "wisey/DestroyObjectOwnerFunction.hpp"
#include "wisey/IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct DestroyObjectOwnerFunctionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  Function* mFunction;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  DestroyObjectOwnerFunctionTest() :
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
  
  ~DestroyObjectOwnerFunctionTest() {
  }
};

TEST_F(DestroyObjectOwnerFunctionTest, callTest) {
  PointerType* genericPointer = Type::getInt8Ty(mLLVMContext)->getPointerTo();
  Value* nullPointer = ConstantPointerNull::get(genericPointer);
  DestroyObjectOwnerFunction::call(mContext, nullPointer, nullPointer, 0);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:                                            ; No predecessors!"
  "\n  call void @__destroyObjectOwnerFunction(i8* null, %wisey.threads.IThread* null, %wisey.threads.CCallStack* null, i8* null)\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(DestroyObjectOwnerFunctionTest, getTest) {
  mContext.getScopes().popScope(mContext, 0);
  mContext.getScopes().pushScope();
  
  Function* function = DestroyObjectOwnerFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "\ndefine void @__destroyObjectOwnerFunction(i8* %thisGeneric, %wisey.threads.IThread* %thread, %wisey.threads.CCallStack* %callstack, i8* %exception) personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %0 = icmp eq i8* %thisGeneric, null"
  "\n  br i1 %0, label %if.null, label %if.notnull"
  "\n"
  "\nif.null:                                          ; preds = %entry"
  "\n  ret void"
  "\n"
  "\nif.notnull:                                       ; preds = %entry"
  "\n  %1 = call i8* @__getOriginalObject(i8* %thisGeneric)"
  "\n  %2 = bitcast i8* %1 to void (i8*, %wisey.threads.IThread*, %wisey.threads.CCallStack*, i8*)***"
  "\n  %vtable = load void (i8*, %wisey.threads.IThread*, %wisey.threads.CCallStack*, i8*)**, void (i8*, %wisey.threads.IThread*, %wisey.threads.CCallStack*, i8*)*** %2"
  "\n  %3 = getelementptr void (i8*, %wisey.threads.IThread*, %wisey.threads.CCallStack*, i8*)*, void (i8*, %wisey.threads.IThread*, %wisey.threads.CCallStack*, i8*)** %vtable, i64 2"
  "\n  %4 = load void (i8*, %wisey.threads.IThread*, %wisey.threads.CCallStack*, i8*)*, void (i8*, %wisey.threads.IThread*, %wisey.threads.CCallStack*, i8*)** %3"
  "\n  invoke void %4(i8* %1, %wisey.threads.IThread* %thread, %wisey.threads.CCallStack* %callstack, i8* %exception)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %if.notnull"
  "\n  %5 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %6 = alloca { i8*, i32 }"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { i8*, i32 } %5, { i8*, i32 }* %6"
  "\n  %7 = getelementptr { i8*, i32 }, { i8*, i32 }* %6, i32 0, i32 0"
  "\n  %8 = load i8*, i8** %7"
  "\n  %9 = call i8* @__cxa_get_exception_ptr(i8* %8)"
  "\n  %10 = getelementptr i8, i8* %9, i64 8"
  "\n  resume { i8*, i32 } %5"
  "\n"
  "\ninvoke.continue:                                  ; preds = %if.notnull"
  "\n  ret void"
  "\n}\n";

  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
