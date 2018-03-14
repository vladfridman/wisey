//
//  TestLocalNativeVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/13/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LocalNativeVariable}
//

#include <gtest/gtest.h>

#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalNativeVariable.hpp"
#include "wisey/NativeType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct LocalNativeVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  NativeType* mNativeType;
  string mStringBuffer;
  raw_string_ostream* mStringStream;

public:
  
  LocalNativeVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    mStringStream = new raw_string_ostream(mStringBuffer);
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mNativeType = new NativeType(Type::getInt64Ty(mLLVMContext)->getPointerTo());
  }
  
  ~LocalNativeVariableTest() {
    delete mStringStream;
  }
};

TEST_F(LocalNativeVariableTest, basicFieldsTest) {
  Type* llvmType = mNativeType->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  IVariable* variable = new LocalNativeVariable("foo", mNativeType, fooValue);
  
  EXPECT_STREQ("foo", variable->getName().c_str());
  EXPECT_EQ(mNativeType, variable->getType());
  EXPECT_FALSE(variable->isField());
  EXPECT_FALSE(variable->isSystem());
}

TEST_F(LocalNativeVariableTest, generateIdentifierIRTest) {
  Type* llvmType = mNativeType->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  IVariable* variable = new LocalNativeVariable("foo", mNativeType, fooValue);
  variable->generateIdentifierIR(mContext);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = alloca i64*"
  "\n  %1 = load i64*, i64** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LocalNativeVariableTest, generateIdentifierReferenceIRTest) {
  Type* llvmType = mNativeType->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  IVariable* variable = new LocalNativeVariable("foo", mNativeType, fooValue);
  
  EXPECT_EQ(fooValue, variable->generateIdentifierReferenceIR(mContext));
}
