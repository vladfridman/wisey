//
//  TestCastObjectFunction.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link CastObjectFunction}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockObjectType.hpp"
#include "TestPrefix.hpp"
#include "wisey/CastObjectFunction.hpp"
#include "wisey/IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct CastObjectFunctionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mDeclareBlock;
  BasicBlock* mEntryBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  NiceMock<MockObjectType> mToObjectType;
  Function* mFunction;

  CastObjectFunctionTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "main",
                                 mContext.getModule());
    mDeclareBlock = BasicBlock::Create(mLLVMContext, "declare", mFunction);
    mEntryBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setDeclarationsBlock(mDeclareBlock);
    mContext.setBasicBlock(mEntryBlock);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    string toTypeName = "systems.vos.wisey.compiler.tests.IToInterface";
    string toNameGlobal = "systems.vos.wisey.compiler.tests.IToInterface.typename";

    ON_CALL(mToObjectType, getObjectNameGlobalVariableName())
    .WillByDefault(Return(toNameGlobal));
    ON_CALL(mToObjectType, getLLVMType(_))
    .WillByDefault(Return(Type::getInt64Ty(mLLVMContext)->getPointerTo()));

    new GlobalVariable(*mContext.getModule(),
                       llvm::ArrayType::get(Type::getInt8Ty(mLLVMContext), toTypeName.length() + 1),
                       true,
                       GlobalValue::LinkageTypes::ExternalLinkage,
                       nullptr,
                       toNameGlobal);
  }
  
  ~CastObjectFunctionTest() {
  }
};

TEST_F(CastObjectFunctionTest, callTest) {
  llvm::Constant* nullPointerValue =
  ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  CastObjectFunction::call(mContext, nullPointerValue, &mToObjectType, 5);
  BranchInst::Create(mEntryBlock, mDeclareBlock);
  
  *mStringStream << *mFunction;
  string expected =
  "\ndefine internal i32 @main() personality i32 (...)* @__gxx_personality_v0 {"
  "\ndeclare:"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declare"
  "\n  %0 = invoke i8* @__castObject(i8* null, i8* getelementptr inbounds ([46 x i8], [46 x i8]* @systems.vos.wisey.compiler.tests.IToInterface.typename, i32 0, i32 0))"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %entry"
  "\n  %1 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %2 = alloca { i8*, i32 }"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { i8*, i32 } %1, { i8*, i32 }* %2"
  "\n  %3 = getelementptr { i8*, i32 }, { i8*, i32 }* %2, i32 0, i32 0"
  "\n  %4 = load i8*, i8** %3"
  "\n  %5 = call i8* @__cxa_get_exception_ptr(i8* %4)"
  "\n  %6 = getelementptr i8, i8* %5, i64 8"
  "\n  resume { i8*, i32 } %1"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n  %7 = bitcast i8* %0 to i64*"
  "\n}\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(CastObjectFunctionTest, getTest) {
  mContext.getScopes().popScope(mContext, 0);
  mContext.getScopes().pushScope();
  
  Function* function = CastObjectFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "\ndefine i8* @__castObject(i8* %fromObjectValue, i8* %toTypeName) personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %isNull = icmp eq i8* %fromObjectValue, null"
  "\n  br i1 %isNull, label %if.null, label %if.notnull"
  "\n"
  "\nif.null:                                          ; preds = %entry"
  "\n  ret i8* null"
  "\n"
  "\nif.notnull:                                       ; preds = %entry"
  "\n  %0 = call i32 @__instanceOf(i8* %fromObjectValue, i8* %toTypeName)"
  "\n  %1 = call i8* @__getOriginalObject(i8* %fromObjectValue)"
  "\n  %cmp = icmp slt i32 %0, 0"
  "\n  br i1 %cmp, label %less.than.zero, label %not.less.than.zero"
  "\n"
  "\nless.than.zero:                                   ; preds = %if.notnull"
  "\n  %2 = call i8* @__getOriginalObjectName(i8* %fromObjectValue)"
  "\n  %3 = call %wisey.lang.MCastException* @wisey.lang.MCastException.build(i8* %2, i8* %toTypeName)"
  "\n  %4 = bitcast { i8*, i8* }* @wisey.lang.MCastException.rtti to i8*"
  "\n  %5 = bitcast %wisey.lang.MCastException* %3 to i8*"
  "\n  %6 = getelementptr i8, i8* %5, i64 -8"
  "\n  %7 = call i8* @__cxa_allocate_exception(i64 add (i64 mul nuw (i64 ptrtoint (i1** getelementptr (i1*, i1** null, i32 1) to i64), i64 3), i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64)))"
  "\n  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %7, i8* %6, i64 add (i64 mul nuw (i64 ptrtoint (i1** getelementptr (i1*, i1** null, i32 1) to i64), i64 3), i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64)), i32 4, i1 false)"
  "\n  tail call void @free(i8* %6)"
  "\n  invoke void @__cxa_throw(i8* %7, i8* %4, i8* null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\nnot.less.than.zero:                               ; preds = %if.notnull"
  "\n  %cmp1 = icmp sgt i32 %0, 0"
  "\n  br i1 %cmp1, label %more.than.zero, label %zero.exactly"
  "\n"
  "\nmore.than.zero:                                   ; preds = %not.less.than.zero"
  "\n  %8 = sub i32 %0, 1"
  "\n  %9 = bitcast i8* %1 to i8*"
  "\n  %10 = mul i32 %8, 8"
  "\n  %11 = getelementptr i8, i8* %9, i32 %10"
  "\n  ret i8* %11"
  "\n"
  "\nzero.exactly:                                     ; preds = %not.less.than.zero"
  "\n  ret i8* %1"
  "\n"
  "\ncleanup:                                          ; preds = %less.than.zero"
  "\n  %12 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %13 = alloca { i8*, i32 }"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { i8*, i32 } %12, { i8*, i32 }* %13"
  "\n  %14 = getelementptr { i8*, i32 }, { i8*, i32 }* %13, i32 0, i32 0"
  "\n  %15 = load i8*, i8** %14"
  "\n  %16 = call i8* @__cxa_get_exception_ptr(i8* %15)"
  "\n  %17 = getelementptr i8, i8* %16, i64 8"
  "\n  resume { i8*, i32 } %12"
  "\n"
  "\ninvoke.continue:                                  ; preds = %less.than.zero"
  "\n  unreachable"
  "\n}\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
