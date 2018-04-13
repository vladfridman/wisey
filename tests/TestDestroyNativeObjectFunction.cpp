//
//  TestDestroyNativeObjectFunction.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/13/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link DestroyNativeObjectFunction}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestPrefix.hpp"
#include "wisey/DestroyNativeObjectFunction.hpp"
#include "wisey/IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct DestroyNativeObjectFunctionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  Function* mFunction;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  DestroyNativeObjectFunctionTest() :
  mLLVMContext(mContext.getLLVMContext()) {
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
  }
  
  ~DestroyNativeObjectFunctionTest() {
  }
};

TEST_F(DestroyNativeObjectFunctionTest, callTest) {
  PointerType* genericPointer = Type::getInt8Ty(mLLVMContext)->getPointerTo();
  Value* nullPointerValue = ConstantPointerNull::get(genericPointer);
  DestroyNativeObjectFunction::call(mContext, nullPointerValue);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  call void @__destroyNativeObjectFunction(i8* null)\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(DestroyNativeObjectFunctionTest, getTest) {
  Function* function = DestroyNativeObjectFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "\ndefine void @__destroyNativeObjectFunction(i8* %thisGeneric) {"
  "\nentry:"
  "\n  %0 = icmp eq i8* %thisGeneric, null"
  "\n  br i1 %0, label %if.null, label %if.notnull"
  "\n"
  "\nif.null:                                          ; preds = %entry"
  "\n  ret void"
  "\n"
  "\nif.notnull:                                       ; preds = %entry"
  "\n  tail call void @free(i8* %thisGeneric)"
  "\n  ret void"
  "\n}\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
