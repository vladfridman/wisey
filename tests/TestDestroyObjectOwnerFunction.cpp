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
#include "DestroyObjectOwnerFunction.hpp"
#include "IRGenerationContext.hpp"

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
  ""
  "\nentry:                                            ; No predecessors!"
  "\n  call void @__destroyObjectOwnerFunction(ptr null, ptr null, ptr null, ptr null)"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(DestroyObjectOwnerFunctionTest, getTest) {
  mContext.getScopes().popScope(mContext, 0);
  mContext.getScopes().pushScope();
  
  Function* function = DestroyObjectOwnerFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "define void @__destroyObjectOwnerFunction(ptr %thisGeneric, ptr %thread, ptr %callstack, ptr %exception) personality ptr @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %0 = icmp eq ptr %thisGeneric, null"
  "\n  br i1 %0, label %if.null, label %if.notnull"
  "\n"
  "\nif.null:                                          ; preds = %entry"
  "\n  ret void"
  "\n"
  "\nif.notnull:                                       ; preds = %entry"
  "\n  %1 = call ptr @__getOriginalObject(ptr %thisGeneric)"
  "\n  %2 = bitcast ptr %1 to ptr"
  "\n  %vtable = load ptr, ptr %2, align 8"
  "\n  %3 = getelementptr ptr, ptr %vtable, i64 2"
  "\n  %4 = load ptr, ptr %3, align 8"
  "\n  invoke void %4(ptr %1, ptr %thread, ptr %callstack, ptr %exception)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %if.notnull"
  "\n  %5 = landingpad { ptr, i32 }"
  "\n          cleanup"
  "\n  %6 = alloca { ptr, i32 }, align 8"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { ptr, i32 } %5, ptr %6, align 8"
  "\n  %7 = getelementptr { ptr, i32 }, ptr %6, i32 0, i32 0"
  "\n  %8 = load ptr, ptr %7, align 8"
  "\n  %9 = call ptr @__cxa_get_exception_ptr(ptr %8)"
  "\n  %10 = getelementptr i8, ptr %9, i64 8"
  "\n  resume { ptr, i32 } %5"
  "\n"
  "\ninvoke.continue:                                  ; preds = %if.notnull"
  "\n  ret void"
  "\n}"
  "\n";

  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
