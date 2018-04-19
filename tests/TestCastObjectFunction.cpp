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
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  NiceMock<MockObjectType> mFromObjectType;
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
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    string fromTypeName = "systems.vos.wisey.compiler.tests.IFromInterface";
    string fromNameGlobal = "systems.vos.wisey.compiler.tests.IFromInterface.name";
    string toTypeName = "systems.vos.wisey.compiler.tests.IToInterface";
    string toNameGlobal = "systems.vos.wisey.compiler.tests.IToInterface.name";

    ON_CALL(mFromObjectType, getObjectNameGlobalVariableName())
    .WillByDefault(Return(fromNameGlobal));
    ON_CALL(mToObjectType, getObjectNameGlobalVariableName())
    .WillByDefault(Return(toNameGlobal));
    ON_CALL(mToObjectType, getLLVMType(_))
    .WillByDefault(Return(Type::getInt64Ty(mLLVMContext)->getPointerTo()));

    new GlobalVariable(*mContext.getModule(),
                       llvm::ArrayType::get(Type::getInt8Ty(mLLVMContext),
                                            fromTypeName.length() + 1),
                       true,
                       GlobalValue::LinkageTypes::ExternalLinkage,
                       nullptr,
                       fromNameGlobal);
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
  CastObjectFunction::call(mContext, nullPointerValue, &mFromObjectType, &mToObjectType, 5);
  
  *mStringStream << *mFunction;
  string expected =
  "\ndefine internal i32 @main() personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %0 = invoke i8* @__castObject(i8* null, i8* getelementptr inbounds ([48 x i8], [48 x i8]* @systems.vos.wisey.compiler.tests.IFromInterface.name, i32 0, i32 0), i8* getelementptr inbounds ([46 x i8], [46 x i8]* @systems.vos.wisey.compiler.tests.IToInterface.name, i32 0, i32 0))"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %entry"
  "\n  %1 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  resume { i8*, i32 } %1"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n  %2 = bitcast i8* %0 to i64*"
  "\n}\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(CastObjectFunctionTest, getTest) {
  Function* function = CastObjectFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "\ndefine i8* @__castObject(i8* %fromObjectValue, i8* %fromTypeName, i8* %toTypeName) personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %0 = call i32 @__instanceOf(i8* %fromObjectValue, i8* %toTypeName)"
  "\n  %1 = bitcast i8* %fromObjectValue to i8*"
  "\n  %2 = call i8* @__getOriginalObject(i8* %1)"
  "\n  %cmp = icmp slt i32 %0, 0"
  "\n  br i1 %cmp, label %less.than.zero, label %not.less.than.zero"
  "\n"
  "\nless.than.zero:                                   ; preds = %entry"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint (%wisey.lang.MCastException.refCounter* getelementptr (%wisey.lang.MCastException.refCounter, %wisey.lang.MCastException.refCounter* null, i32 1) to i64))"
  "\n  %buildervar = bitcast i8* %malloccall to %wisey.lang.MCastException.refCounter*"
  "\n  %3 = bitcast %wisey.lang.MCastException.refCounter* %buildervar to i8*"
  "\n  call void @llvm.memset.p0i8.i64(i8* %3, i8 0, i64 ptrtoint (%wisey.lang.MCastException.refCounter* getelementptr (%wisey.lang.MCastException.refCounter, %wisey.lang.MCastException.refCounter* null, i32 1) to i64), i32 4, i1 false)"
  "\n  %4 = getelementptr %wisey.lang.MCastException.refCounter, %wisey.lang.MCastException.refCounter* %buildervar, i32 0, i32 1"
  "\n  %5 = getelementptr %wisey.lang.MCastException, %wisey.lang.MCastException* %4, i32 0, i32 1"
  "\n  store i8* %fromTypeName, i8** %5"
  "\n  %6 = getelementptr %wisey.lang.MCastException, %wisey.lang.MCastException* %4, i32 0, i32 2"
  "\n  store i8* %toTypeName, i8** %6"
  "\n  %7 = bitcast %wisey.lang.MCastException* %4 to i8*"
  "\n  %8 = getelementptr i8, i8* %7, i64 0"
  "\n  %9 = bitcast i8* %8 to i32 (...)***"
  "\n  %10 = getelementptr { [3 x i8*] }, { [3 x i8*] }* @wisey.lang.MCastException.vtable, i32 0, i32 0, i32 0"
  "\n  %11 = bitcast i8** %10 to i32 (...)**"
  "\n  store i32 (...)** %11, i32 (...)*** %9"
  "\n  %12 = alloca %wisey.lang.MCastException*"
  "\n  store %wisey.lang.MCastException* %4, %wisey.lang.MCastException** %12"
  "\n  %13 = bitcast { i8*, i8* }* @wisey.lang.MCastException.rtti to i8*"
  "\n  %14 = bitcast %wisey.lang.MCastException* %4 to i8*"
  "\n  %15 = getelementptr i8, i8* %14, i64 -8"
  "\n  %16 = call i8* @__cxa_allocate_exception(i64 add (i64 mul nuw (i64 ptrtoint (i1** getelementptr (i1*, i1** null, i32 1) to i64), i64 3), i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64)))"
  "\n  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %16, i8* %15, i64 add (i64 mul nuw (i64 ptrtoint (i1** getelementptr (i1*, i1** null, i32 1) to i64), i64 3), i64 ptrtoint (i64* getelementptr (i64, i64* null, i32 1) to i64)), i32 4, i1 false)"
  "\n  invoke void @__cxa_throw(i8* %16, i8* %13, i8* null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\nnot.less.than.zero:                               ; preds = %entry"
  "\n  %cmp1 = icmp sgt i32 %0, 0"
  "\n  br i1 %cmp1, label %more.than.zero, label %zero.exactly"
  "\n"
  "\nmore.than.zero:                                   ; preds = %not.less.than.zero"
  "\n  %17 = sub i32 %0, 1"
  "\n  %18 = bitcast i8* %2 to i8*"
  "\n  %19 = mul i32 %17, 8"
  "\n  %20 = getelementptr i8, i8* %18, i32 %19"
  "\n  ret i8* %20"
  "\n"
  "\nzero.exactly:                                     ; preds = %not.less.than.zero"
  "\n  ret i8* %2"
  "\n"
  "\ncleanup:                                          ; preds = %less.than.zero"
  "\n  %21 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %22 = load %wisey.lang.MCastException*, %wisey.lang.MCastException** %12"
  "\n  %23 = bitcast %wisey.lang.MCastException* %22 to i8*"
  "\n  call void @__destroyOwnerObjectFunction(i8* %23)"
  "\n  resume { i8*, i32 } %21"
  "\n"
  "\ninvoke.continue:                                  ; preds = %less.than.zero"
  "\n  unreachable"
  "\n}\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}