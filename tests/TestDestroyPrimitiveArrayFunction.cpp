//
//  TestDestroyPrimitiveArrayFunction.cpp
//  runtests
//
//  Created by Vladimir Fridman on 1/15/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link DestroyPrimitiveArrayFunction}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestPrefix.hpp"
#include "wisey/DestroyPrimitiveArrayFunction.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/ProgramPrefix.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct DestroyPrimitiveArrayFunctionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  Function* mFunction;
  Function* mDestructor;
  Type* mInt8Pointer;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  DestroyPrimitiveArrayFunctionTest() :
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
  
  ~DestroyPrimitiveArrayFunctionTest() {
  }
};

TEST_F(DestroyPrimitiveArrayFunctionTest, callTest) {
  llvm::ArrayType* arrayType = llvm::ArrayType::get(mInt8Pointer, 0);
  Value* nullPointerValue = ConstantPointerNull::get(arrayType->getPointerTo());
  DestroyPrimitiveArrayFunction::call(mContext, nullPointerValue);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  call void @__destroyPrimitiveArrayFunction([0 x i8*]* null)\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(DestroyPrimitiveArrayFunctionTest, getTest) {
  Function* function = DestroyPrimitiveArrayFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "\ndefine internal void @__destroyPrimitiveArrayFunction([0 x i8*]* %arrayPointer) {"
  "\nentry:"
  "\n  %0 = icmp eq [0 x i8*]* %arrayPointer, null"
  "\n  br i1 %0, label %if.null, label %free.array"
  "\n"
  "\nif.null:                                          ; preds = %entry"
  "\n  ret void"
  "\n"
  "\nfree.array:                                       ; preds = %entry"
  "\n  %1 = bitcast [0 x i8*]* %arrayPointer to i8*"
  "\n  tail call void @free(i8* %1)"
  "\n  ret void"
  "\n}\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
