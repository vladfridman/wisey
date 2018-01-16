//
//  TestDestroyOwnerArrayFunction.cpp
//  runtests
//
//  Created by Vladimir Fridman on 12/19/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link DestroyOwnerArrayFunction}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestPrefix.hpp"
#include "wisey/DestroyOwnerArrayFunction.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/ProgramPrefix.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct DestroyOwnerArrayFunctionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  Function* mFunction;
  Function* mDestructor;
  Type* mInt8Pointer;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  DestroyOwnerArrayFunctionTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mInt8Pointer(Type::getInt8Ty(mLLVMContext)->getPointerTo()) {
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    TestPrefix::generateIR(mContext);
    
    vector<Type*> destructorArgumentTypes;
    destructorArgumentTypes.push_back(mInt8Pointer);
    ArrayRef<Type*> destructorArgTypesArray = ArrayRef<Type*>(destructorArgumentTypes);
    Type* detructorLlvmReturnType = Type::getVoidTy(mLLVMContext);
    FunctionType* destructorFunctionType = FunctionType::get(detructorLlvmReturnType,
                                                             destructorArgTypesArray,
                                                             false);
    mDestructor = Function::Create(destructorFunctionType,
                                   GlobalValue::ExternalLinkage,
                                   "destructor",
                                   mContext.getModule());

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
  }
  
  ~DestroyOwnerArrayFunctionTest() {
  }
};

TEST_F(DestroyOwnerArrayFunctionTest, callTest) {
  llvm::ArrayType* arrayType = llvm::ArrayType::get(mInt8Pointer, 0);
  Value* nullPointerValue = ConstantPointerNull::get(arrayType->getPointerTo());
  DestroyOwnerArrayFunction::call(mContext, nullPointerValue, 3, mDestructor);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  call void @__destroyOwnerArrayFunction([0 x i8*]* null, i64 3, void (i8*)* @destructor)\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(DestroyOwnerArrayFunctionTest, getTest) {
  Function* function = DestroyOwnerArrayFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "\ndefine internal void @__destroyOwnerArrayFunction([0 x i8*]* %arrayPointer, i64 %size, void (i8*)* %destructor) {"
  "\nentry:"
  "\n  %0 = icmp eq [0 x i8*]* %arrayPointer, null"
  "\n  br i1 %0, label %if.null, label %if.not.null"
  "\n"
  "\nif.null:                                          ; preds = %entry"
  "\n  ret void"
  "\n"
  "\nif.not.null:                                      ; preds = %entry"
  "\n  %1 = icmp eq void (i8*)* %destructor, null"
  "\n  br i1 %1, label %free.array, label %free.elements"
  "\n"
  "\nfree.elements:                                    ; preds = %if.not.null"
  "\n  %2 = alloca i64"
  "\n  store i64 0, i64* %2"
  "\n  br label %for.cond"
  "\n"
  "\nfor.cond:                                         ; preds = %for.body, %free.elements"
  "\n  %3 = load i64, i64* %2"
  "\n  %cmp = icmp slt i64 %3, %size"
  "\n  br i1 %cmp, label %for.body, label %free.array"
  "\n"
  "\nfor.body:                                         ; preds = %for.cond"
  "\n  %4 = getelementptr [0 x i8*], [0 x i8*]* %arrayPointer, i64 0, i64 %3"
  "\n  %5 = load i8*, i8** %4"
  "\n  call void %destructor(i8* %5)"
  "\n  %6 = add i64 %3, 1"
  "\n  store i64 %6, i64* %2"
  "\n  br label %for.cond"
  "\n"
  "\nfree.array:                                       ; preds = %for.cond, %if.not.null"
  "\n  %7 = bitcast [0 x i8*]* %arrayPointer to i8*"
  "\n  tail call void @free(i8* %7)"
  "\n  ret void"
  "\n}\n";

  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

