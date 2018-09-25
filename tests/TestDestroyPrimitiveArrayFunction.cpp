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
#include "DestroyPrimitiveArrayFunction.hpp"
#include "IRGenerationContext.hpp"
#include "PrimitiveTypes.hpp"

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
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  DestroyPrimitiveArrayFunctionTest() :
  mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    vector<Type*> destructorArgumentTypes;
    destructorArgumentTypes.push_back(Type::getInt8Ty(mLLVMContext)->getPointerTo());
    Type* detructorLlvmReturnType = Type::getVoidTy(mLLVMContext);
    FunctionType* destructorFunctionType = FunctionType::get(detructorLlvmReturnType,
                                                             destructorArgumentTypes,
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
    BasicBlock* declareBlock = BasicBlock::Create(mLLVMContext, "declare", mFunction);
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setDeclarationsBlock(declareBlock);
    mContext.setBasicBlock(mBasicBlock);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~DestroyPrimitiveArrayFunctionTest() {
  }
};

TEST_F(DestroyPrimitiveArrayFunctionTest, callTest) {
  llvm::PointerType* i64PointerType = llvm::Type::getInt64Ty(mLLVMContext)->getPointerTo();
  llvm::PointerType* i8PointerType = llvm::Type::getInt8Ty(mLLVMContext)->getPointerTo();
  Value* arrayPointer = ConstantPointerNull::get(i64PointerType);
  Value* nullPointer = ConstantPointerNull::get(i8PointerType);
  DestroyPrimitiveArrayFunction::call(mContext, arrayPointer, 2u, nullPointer, nullPointer, 0);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:                                            ; No predecessors!"
  "\n  call void @__destroyPrimitiveArrayFunction(i64* null, i64 2, i8* null, i8* null)\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(DestroyPrimitiveArrayFunctionTest, getTest) {
  mContext.getScopes().popScope(mContext, 0);
  mContext.getScopes().pushScope();
  
  Function* function = DestroyPrimitiveArrayFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "\ndefine void @__destroyPrimitiveArrayFunction(i64* %arrayPointer, i64 %noOfDimensions, i8* %arrayName, i8* %exception) {"
  "\nentry:"
  "\n  %isNull = icmp eq i64* %arrayPointer, null"
  "\n  br i1 %isNull, label %return.void, label %if.not.null"
  "\n"
  "\nreturn.void:                                      ; preds = %entry"
  "\n  ret void"
  "\n"
  "\nif.not.null:                                      ; preds = %entry"
  "\n  call void @__checkArrayNotReferenced(i64* %arrayPointer, i64 %noOfDimensions, i8* %arrayName, i8* %exception)"
  "\n  %0 = bitcast i64* %arrayPointer to i8*"
  "\n  tail call void @free(i8* %0)"
  "\n  ret void"
  "\n}\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
