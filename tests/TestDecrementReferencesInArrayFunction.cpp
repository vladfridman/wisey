//
//  TestDecrementReferencesInArrayFunction.cpp
//  runtests
//
//  Created by Vladimir Fridman on 12/19/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link DecrementReferencesInArrayFunction}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestPrefix.hpp"
#include "wisey/DecrementReferencesInArrayFunction.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/ProgramPrefix.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct DecrementReferencesInArrayFunctionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  Function* mFunction;
  Function* mDecrementor;
  Type* mInt8Pointer;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  DecrementReferencesInArrayFunctionTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mInt8Pointer(Type::getInt8Ty(mLLVMContext)->getPointerTo()) {
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    TestPrefix::generateIR(mContext);
    
    vector<Type*> decrementorArgumentTypes;
    decrementorArgumentTypes.push_back(Type::getInt8Ty(mLLVMContext)->getPointerTo());
    decrementorArgumentTypes.push_back(Type::getInt64Ty(mLLVMContext));
    ArrayRef<Type*> decrementorArgTypesArray = ArrayRef<Type*>(decrementorArgumentTypes);
    Type* decrementorReturnType = Type::getVoidTy(mLLVMContext);
    FunctionType* decrementorFunctionType = FunctionType::get(decrementorReturnType,
                                                              decrementorArgTypesArray,
                                                              false);
    mDecrementor = Function::Create(decrementorFunctionType,
                                    GlobalValue::ExternalLinkage,
                                    "decrementor",
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
  
  ~DecrementReferencesInArrayFunctionTest() {
  }
};

TEST_F(DecrementReferencesInArrayFunctionTest, callTest) {
  llvm::ArrayType* arrayType = llvm::ArrayType::get(mInt8Pointer, 0);
  Value* nullPointerValue = ConstantPointerNull::get(arrayType->getPointerTo());
  DecrementReferencesInArrayFunction::call(mContext, nullPointerValue, 3, mDecrementor);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  call void @__decrementReferencesInArrayFunction([0 x i8*]* null, "
  "i64 3, void (i8*, i64)* @decrementor)\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(DecrementReferencesInArrayFunctionTest, getTest) {
  Function* function = DecrementReferencesInArrayFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "\ndefine internal void @__decrementReferencesInArrayFunction([0 x i8*]* %arrayBitcast, "
  "i64 %size, void (i8*, i64)* %decrementor) {"
  "\nentry:"
  "\n  %0 = alloca i64"
  "\n  store i64 0, i64* %0"
  "\n  br label %for.cond"
  "\n"
  "\nfor.cond:                                         ; preds = %for.body, %entry"
  "\n  %1 = load i64, i64* %0"
  "\n  %cmp = icmp slt i64 %1, %size"
  "\n  br i1 %cmp, label %for.body, label %for.end"
  "\n"
  "\nfor.body:                                         ; preds = %for.cond"
  "\n  %2 = getelementptr [0 x i8*], [0 x i8*]* %arrayBitcast, i64 0, i64 %1"
  "\n  %3 = load i8*, i8** %2"
  "\n  call void %decrementor(i8* %3, i64 -1)"
  "\n  %4 = add i64 %1, 1"
  "\n  store i64 %4, i64* %0"
  "\n  br label %for.cond"
  "\n"
  "\nfor.end:                                          ; preds = %for.cond"
  "\n  ret void"
  "\n}\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}


