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
#include "wisey/CheckCastToObjectFunction.hpp"
#include "wisey/IRGenerationContext.hpp"

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
    mContext.getScopes().pushScope();
    
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
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = bitcast i8* null to i8*"
  "\n  invoke void @__checkCastToObject(i8* %0, i8 109, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @model, i32 0, i32 0))"
  "\n          to label %invoke.continue unwind label %cleanup\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(CheckCastToObjectFunctionTest, callCheckCastToNodeTest) {
  Value* nullPointerValue = ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  CheckCastToObjectFunction::callCheckCastToNode(mContext, nullPointerValue);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = bitcast i8* null to i8*"
  "\n  invoke void @__checkCastToObject(i8* %0, i8 110, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @node, i32 0, i32 0))"
  "\n          to label %invoke.continue unwind label %cleanup\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(CheckCastToObjectFunctionTest, getTest) {
  Function* function = CheckCastToObjectFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "\ndefine void @__checkCastToObject(i8* %object, i8 %letter, i8* %toType) personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %isNull = icmp eq i8* %object, null"
  "\n  br i1 %isNull, label %return.block, label %if.notnull"
  "\n"
  "\nreturn.block:                                     ; preds = %if.notnull, %entry"
  "\n  ret void"
  "\n"
  "\nif.notnull:                                       ; preds = %entry"
  "\n  %0 = call i1 @__isObject(i8* %object, i8 %letter)"
  "\n  br i1 %0, label %return.block, label %if.not.object"
  "\n"
  "\nif.not.object:                                    ; preds = %if.notnull"
  "\n  %1 = call i8* @__getOriginalObjectName(i8* %object)"
  "\n  %2 = call %wisey.lang.MCastException* @wisey.lang.MCastException.build(i8* %1, i8* %toType)"
  "\n  %3 = bitcast { i8*, i8* }* @wisey.lang.MCastException.rtti to i8*"
  "\n  %4 = bitcast %wisey.lang.MCastException* %2 to i8*"
  "\n  %5 = getelementptr i8, i8* %4, i64 -8"
  "\n  %6 = call i8* @__cxa_allocate_exception(i64 add (i64 mul nuw (i64 ptrtoint (i1** getelementptr (i1*, i1** null, i32 1) to i64), i64 3), i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64)))"
  "\n  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %6, i8* %5, i64 add (i64 mul nuw (i64 ptrtoint (i1** getelementptr (i1*, i1** null, i32 1) to i64), i64 3), i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64)), i32 4, i1 false)"
  "\n  tail call void @free(i8* %5)"
  "\n  invoke void @__cxa_throw(i8* %6, i8* %3, i8* null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %if.not.object"
  "\n  %7 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %8 = alloca { i8*, i32 }"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { i8*, i32 } %7, { i8*, i32 }* %8"
  "\n  %9 = getelementptr { i8*, i32 }, { i8*, i32 }* %8, i32 0, i32 0"
  "\n  %10 = load i8*, i8** %9"
  "\n  %11 = call i8* @__cxa_get_exception_ptr(i8* %10)"
  "\n  %12 = getelementptr i8, i8* %11, i64 8"
  "\n  resume { i8*, i32 } %7"
  "\n"
  "\ninvoke.continue:                                  ; preds = %if.not.object"
  "\n  unreachable"
  "\n}\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
