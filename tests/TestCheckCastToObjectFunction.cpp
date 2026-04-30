//
//  TestCheckCastToObjectFunction.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/25/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link CheckCastToObjectFunction}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestPrefix.hpp"
#include "CheckCastToObjectFunction.hpp"
#include "IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct CheckCastToObjectFunctionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  Function* mFunction;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  CheckCastToObjectFunctionTest() :
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
  
  ~CheckCastToObjectFunctionTest() {
  }
};

TEST_F(CheckCastToObjectFunctionTest, callCheckCastToModelTest) {
  Value* nullPointerValue = ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  CheckCastToObjectFunction::callCheckCastToModel(mContext, nullPointerValue);
  
  *mStringStream << *mBasicBlock;
  string expected =
  ""
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = bitcast ptr null to ptr"
  "\n  invoke void @__checkCastToObject(ptr %0, i8 109, ptr @model)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(CheckCastToObjectFunctionTest, callCheckCastToNodeTest) {
  Value* nullPointerValue = ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  CheckCastToObjectFunction::callCheckCastToNode(mContext, nullPointerValue);
  
  *mStringStream << *mBasicBlock;
  string expected =
  ""
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = bitcast ptr null to ptr"
  "\n  invoke void @__checkCastToObject(ptr %0, i8 110, ptr @node)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(CheckCastToObjectFunctionTest, getTest) {
  mContext.getScopes().popScope(mContext, 0);
  mContext.getScopes().pushScope();
  
  Function* function = CheckCastToObjectFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "define void @__checkCastToObject(ptr %object, i8 %letter, ptr %toType) personality ptr @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %isNull = icmp eq ptr %object, null"
  "\n  br i1 %isNull, label %return.block, label %if.notnull"
  "\n"
  "\nreturn.block:                                     ; preds = %if.notnull, %entry"
  "\n  ret void"
  "\n"
  "\nif.notnull:                                       ; preds = %entry"
  "\n  %0 = call i1 @__isObject(ptr %object, i8 %letter)"
  "\n  br i1 %0, label %return.block, label %if.not.object"
  "\n"
  "\nif.not.object:                                    ; preds = %if.notnull"
  "\n  %1 = call ptr @__getOriginalObjectName(ptr %object)"
  "\n  %buildervar = tail call ptr @malloc(i64 ptrtoint (ptr getelementptr (%wisey.lang.MCastException.refCounter, ptr null, i32 1) to i64))"
  "\n  %2 = bitcast ptr %buildervar to ptr"
  "\n  call void @llvm.memset.p0.i64(ptr %2, i8 0, i64 ptrtoint (ptr getelementptr (%wisey.lang.MCastException.refCounter, ptr null, i32 1) to i64), i1 false)"
  "\n  %3 = getelementptr %wisey.lang.MCastException.refCounter, ptr %buildervar, i32 0, i32 1"
  "\n  %4 = getelementptr %wisey.lang.MCastException, ptr %3, i32 0, i32 1"
  "\n  store ptr %1, ptr %4, align 8"
  "\n  %5 = getelementptr %wisey.lang.MCastException, ptr %3, i32 0, i32 2"
  "\n  store ptr %toType, ptr %5, align 8"
  "\n  %6 = bitcast ptr %3 to ptr"
  "\n  %7 = getelementptr i8, ptr %6, i64 0"
  "\n  %8 = bitcast ptr %7 to ptr"
  "\n  %9 = getelementptr { [3 x ptr] }, ptr @wisey.lang.MCastException.vtable, i32 0, i32 0, i32 0"
  "\n  %10 = bitcast ptr %9 to ptr"
  "\n  store ptr %10, ptr %8, align 8"
  "\n  %11 = bitcast ptr @wisey.lang.MCastException.rtti to ptr"
  "\n  %12 = bitcast ptr %3 to ptr"
  "\n  %13 = getelementptr i8, ptr %12, i64 -8"
  "\n  %14 = call ptr @__cxa_allocate_exception(i64 add (i64 ptrtoint (ptr getelementptr (%wisey.lang.MCastException, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)))"
  "\n  call void @llvm.memcpy.p0.p0.i64(ptr %14, ptr %13, i64 add (i64 ptrtoint (ptr getelementptr (%wisey.lang.MCastException, ptr null, i32 1) to i64), i64 ptrtoint (ptr getelementptr (i64, ptr null, i32 1) to i64)), i1 false)"
  "\n  tail call void @free(ptr %13)"
  "\n  invoke void @__cxa_throw(ptr %14, ptr %11, ptr null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %if.not.object"
  "\n  %15 = landingpad { ptr, i32 }"
  "\n          cleanup"
  "\n  %16 = alloca { ptr, i32 }, align 8"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { ptr, i32 } %15, ptr %16, align 8"
  "\n  %17 = getelementptr { ptr, i32 }, ptr %16, i32 0, i32 0"
  "\n  %18 = load ptr, ptr %17, align 8"
  "\n  %19 = call ptr @__cxa_get_exception_ptr(ptr %18)"
  "\n  %20 = getelementptr i8, ptr %19, i64 8"
  "\n  resume { ptr, i32 } %15"
  "\n"
  "\ninvoke.continue:                                  ; preds = %if.not.object"
  "\n  unreachable"
  "\n}"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
