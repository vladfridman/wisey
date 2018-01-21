//
//  TestDestroyReferenceArrayFunction.cpp
//  runtests
//
//  Created by Vladimir Fridman on 1/15/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link DestroyReferenceArrayFunction}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestPrefix.hpp"
#include "wisey/DestroyReferenceArrayFunction.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/ProgramPrefix.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct DestroyReferenceArrayFunctionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  Function* mFunction;
  Function* mDestructor;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  DestroyReferenceArrayFunctionTest() :
  mLLVMContext(mContext.getLLVMContext()) {
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    TestPrefix::generateIR(mContext);
    
    vector<Type*> destructorArgumentTypes;
    destructorArgumentTypes.push_back(Type::getInt8Ty(mLLVMContext)->getPointerTo());
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
  
  ~DestroyReferenceArrayFunctionTest() {
  }
};

TEST_F(DestroyReferenceArrayFunctionTest, callTest) {
  llvm::PointerType* genericPointer = llvm::Type::getInt64Ty(mLLVMContext)->getPointerTo();
  Value* nullPointerValue = ConstantPointerNull::get(genericPointer);
  DestroyReferenceArrayFunction::call(mContext, nullPointerValue);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  call void @__destroyReferenceArrayFunction(i64* null)\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(DestroyReferenceArrayFunctionTest, getTest) {
  Function* function = DestroyReferenceArrayFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "\ndefine internal void @__destroyReferenceArrayFunction(i64* %arrayPointer) {"
  "\nentry:"
  "\n  %0 = icmp eq i64* %arrayPointer, null"
  "\n  br i1 %0, label %if.null, label %if.not.null"
  "\n"
  "\nif.null:                                          ; preds = %entry"
  "\n  ret void"
  "\n"
  "\nif.not.null:                                      ; preds = %entry"
  "\n  %1 = getelementptr i64, i64* %arrayPointer, i64 1"
  "\n  %dimensions = load i64, i64* %1"
  "\n  %offset = add i64 %dimensions, 2"
  "\n  %2 = getelementptr i64, i64* %arrayPointer, i64 2"
  "\n  %size = load i64, i64* %2"
  "\n  %3 = getelementptr i64, i64* %1, i64 %offset"
  "\n  %4 = bitcast i64* %3 to [0 x i8*]*"
  "\n  %5 = alloca i64"
  "\n  store i64 0, i64* %5"
  "\n  br label %for.cond"
  "\n"
  "\nfor.cond:                                         ; preds = %for.body, %if.not.null"
  "\n  %6 = load i64, i64* %5"
  "\n  %cmp = icmp slt i64 %6, %size"
  "\n  br i1 %cmp, label %for.body, label %free.array"
  "\n"
  "\nfor.body:                                         ; preds = %for.cond"
  "\n  %7 = getelementptr [0 x i8*], [0 x i8*]* %4, i64 0, i64 %6"
  "\n  %8 = load i8*, i8** %7"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* %8, i64 -1)"
  "\n  %9 = add i64 %6, 1"
  "\n  store i64 %9, i64* %5"
  "\n  br label %for.cond"
  "\n"
  "\nfree.array:                                       ; preds = %for.cond"
  "\n  %10 = bitcast i64* %arrayPointer to i8*"
  "\n  tail call void @free(i8* %10)"
  "\n  ret void"
  "\n}\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}