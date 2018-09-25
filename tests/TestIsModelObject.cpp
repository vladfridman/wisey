//
//  TestIsObjectFunction.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/25/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IsObjectFunction}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestPrefix.hpp"
#include "IRGenerationContext.hpp"
#include "IsObjectFunction.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct IsObjectFunctionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  Function* mFunction;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  IsObjectFunctionTest() :
  mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
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
  
  ~IsObjectFunctionTest() {
  }
};

TEST_F(IsObjectFunctionTest, callIsModelTest) {
  Value* nullPointerValue = ConstantPointerNull::get(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  IsObjectFunction::callIsModel(mContext, nullPointerValue);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = call i1 @__isObject(i8* null, i8 109)\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(IsObjectFunctionTest, getTest) {
  mContext.getScopes().popScope(mContext, 0);
  mContext.getScopes().pushScope();
  
  Function* function = IsObjectFunction::get(mContext);
  mContext.runComposingCallbacks();
  
  *mStringStream << *function;
  string expected =
  "\ndefine i1 @__isObject(i8* %object, i8 %letter) {"
  "\nentry:"
  "\n  %0 = call i8* @__getOriginalObject(i8* %object)"
  "\n  %1 = bitcast i8* %0 to i8***"
  "\n  %vtable = load i8**, i8*** %1"
  "\n  %2 = getelementptr i8*, i8** %vtable, i64 1"
  "\n  %typeArrayI8 = load i8*, i8** %2"
  "\n  %3 = bitcast i8* %typeArrayI8 to i8**"
  "\n  %stringPointer = load i8*, i8** %3"
  "\n  %firstLetter = load i8, i8* %stringPointer"
  "\n  %4 = icmp eq i8 %firstLetter, %letter"
  "\n  ret i1 %4"
  "\n}\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
