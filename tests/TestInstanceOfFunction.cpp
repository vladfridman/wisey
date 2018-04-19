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
#include "wisey/InstanceOfFunction.hpp"
#include "wisey/IRGenerationContext.hpp"

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
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();

    mStringStream = new raw_string_ostream(mStringBuffer);
    
    string typeName = "systems.vos.wisey.compiler.tests.IInterface";
    string nameGlobal = "systems.vos.wisey.compiler.tests.IInterface.name";

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
  Value* nullPointerValue = ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  InstanceOfFunction::call(mContext, nullPointerValue, &mObjectType);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = bitcast i8* null to i8*"
  "\n  %1 = call i32 @__instanceOf(i8* %0, i8* getelementptr inbounds ([44 x i8], [44 x i8]* @systems.vos.wisey.compiler.tests.IInterface.name, i32 0, i32 0))\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(InstanceOfFunctionTest, getTest) {
  Function* function = InstanceOfFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "\ndefine i32 @__instanceOf(i8* %haystack, i8* %needle) {"
  "\nentry:"
  "\n  %iterator = alloca i32"
  "\n  store i32 1, i32* %iterator"
  "\n  %0 = bitcast i8* %haystack to i8*"
  "\n  %1 = call i8* @__getOriginalObject(i8* %0)"
  "\n  %2 = bitcast i8* %1 to i8***"
  "\n  %vtable = load i8**, i8*** %2"
  "\n  %3 = getelementptr i8*, i8** %vtable, i64 1"
  "\n  %typeArrayI8 = load i8*, i8** %3"
  "\n  %4 = bitcast i8* %typeArrayI8 to i8**"
  "\n  br label %while.cond"
  "\n"
  "\nwhile.cond:                                       ; preds = %while.body, %entry"
  "\n  %5 = load i32, i32* %iterator"
  "\n  %6 = getelementptr i8*, i8** %4, i32 %5"
  "\n  %stringPointer = load i8*, i8** %6"
  "\n  %cmpnull = icmp eq i8* %stringPointer, null"
  "\n  br i1 %cmpnull, label %return.notfound, label %while.body"
  "\n"
  "\nwhile.body:                                       ; preds = %while.cond"
  "\n  %7 = load i32, i32* %iterator"
  "\n  %inc = add i32 %7, 1"
  "\n  store i32 %inc, i32* %iterator"
  "\n  %cmp = icmp eq i8* %stringPointer, %needle"
  "\n  br i1 %cmp, label %return.found, label %while.cond"
  "\n"
  "\nreturn.notfound:                                  ; preds = %while.cond"
  "\n  ret i32 -1"
  "\n"
  "\nreturn.found:                                     ; preds = %while.body"
  "\n  %8 = load i32, i32* %iterator"
  "\n  %dec = sub i32 %8, 2"
  "\n  ret i32 %dec"
  "\n}\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
