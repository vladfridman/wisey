//
//  TestInstanceOfFunction.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link InstanceOfFunction}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockObjectType.hpp"
#include "InstanceOfFunction.hpp"
#include "IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct InstanceOfFunctionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  NiceMock<MockObjectType> mObjectType;
  
  InstanceOfFunctionTest() : mLLVMContext(mContext.getLLVMContext()) {    
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
    
    string typeName = "systems.vos.wisey.compiler.tests.IInterface";
    string nameGlobal = "systems.vos.wisey.compiler.tests.IInterface.typename";

    ON_CALL(mObjectType, getObjectNameGlobalVariableName()).WillByDefault(Return(nameGlobal));

    new GlobalVariable(*mContext.getModule(),
                       llvm::ArrayType::get(Type::getInt8Ty(mLLVMContext), typeName.length() + 1),
                       true,
                       GlobalValue::LinkageTypes::ExternalLinkage,
                       nullptr,
                       nameGlobal);
  }
  
  ~InstanceOfFunctionTest() {
  }
};

TEST_F(InstanceOfFunctionTest, callTest) {
  llvm::Constant* nullPointerValue =
  ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  InstanceOfFunction::call(mContext, nullPointerValue, nullPointerValue);
  
  *mStringStream << *mBasicBlock;
  string expected =
  ""
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = call i32 @__instanceOf(ptr null, ptr null)"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(InstanceOfFunctionTest, getTest) {
  Function* function = InstanceOfFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "define i32 @__instanceOf(ptr %haystack, ptr %needle) {"
  "\ndeclarations:"
  "\n  %iterator = alloca i32, align 4"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declarations"
  "\n  store i32 1, ptr %iterator, align 4"
  "\n  %0 = icmp eq ptr %haystack, null"
  "\n  br i1 %0, label %return.notfound, label %if.notnull"
  "\n"
  "\nwhile.cond:                                       ; preds = %while.body, %if.notnull"
  "\n  %1 = load i32, ptr %iterator, align 4"
  "\n  %2 = getelementptr ptr, ptr %8, i32 %1"
  "\n  %stringPointer = load ptr, ptr %2, align 8"
  "\n  %cmpnull = icmp eq ptr %stringPointer, null"
  "\n  br i1 %cmpnull, label %return.notfound, label %while.body"
  "\n"
  "\nwhile.body:                                       ; preds = %while.cond"
  "\n  %3 = load i32, ptr %iterator, align 4"
  "\n  %inc = add i32 %3, 1"
  "\n  store i32 %inc, ptr %iterator, align 4"
  "\n  %cmp = icmp eq ptr %stringPointer, %needle"
  "\n  br i1 %cmp, label %return.found, label %while.cond"
  "\n"
  "\nreturn.notfound:                                  ; preds = %while.cond, %entry"
  "\n  ret i32 -1"
  "\n"
  "\nreturn.found:                                     ; preds = %while.body"
  "\n  %4 = load i32, ptr %iterator, align 4"
  "\n  %dec = sub i32 %4, 2"
  "\n  ret i32 %dec"
  "\n"
  "\nif.notnull:                                       ; preds = %entry"
  "\n  %5 = call ptr @__getOriginalObject(ptr %haystack)"
  "\n  %6 = bitcast ptr %5 to ptr"
  "\n  %vtable = load ptr, ptr %6, align 8"
  "\n  %7 = getelementptr ptr, ptr %vtable, i64 1"
  "\n  %typeArrayI8 = load ptr, ptr %7, align 8"
  "\n  %8 = bitcast ptr %typeArrayI8 to ptr"
  "\n  br label %while.cond"
  "\n}"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
