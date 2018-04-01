//
//  TestLocalPointerOwnerVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/28/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LocalPointerOwnerVariable}
//

#include <gtest/gtest.h>

#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/LocalPointerOwnerVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct LocalPointerOwnerVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  LLVMPointerOwnerType* mPointerOwnerType;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  LocalPointerOwnerVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    mStringStream = new raw_string_ostream(mStringBuffer);
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mPointerOwnerType = new LLVMPointerOwnerType(LLVMPrimitiveTypes::I64->getPointerType());
  }
  
  ~LocalPointerOwnerVariableTest() {
    delete mStringStream;
  }
};

TEST_F(LocalPointerOwnerVariableTest, basicFieldsTest) {
  Type* llvmType = mPointerOwnerType->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  IVariable* variable = new LocalPointerOwnerVariable("foo", mPointerOwnerType, fooValue);
  
  EXPECT_STREQ("foo", variable->getName().c_str());
  EXPECT_EQ(mPointerOwnerType, variable->getType());
  EXPECT_FALSE(variable->isField());
  EXPECT_FALSE(variable->isSystem());
}

TEST_F(LocalPointerOwnerVariableTest, generateIdentifierIRTest) {
  Type* llvmType = mPointerOwnerType->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  IVariable* variable = new LocalPointerOwnerVariable("foo", mPointerOwnerType, fooValue);
  variable->generateIdentifierIR(mContext);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = alloca i64*"
  "\n  %1 = load i64*, i64** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LocalPointerOwnerVariableTest, generateIdentifierReferenceIRTest) {
  Type* llvmType = mPointerOwnerType->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  IVariable* variable = new LocalPointerOwnerVariable("foo", mPointerOwnerType, fooValue);
  
  EXPECT_EQ(fooValue, variable->generateIdentifierReferenceIR(mContext));
}

TEST_F(LocalPointerOwnerVariableTest, freeTest) {
  Type* llvmType = mPointerOwnerType->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  IOwnerVariable* variable = new LocalPointerOwnerVariable("foo", mPointerOwnerType, fooValue);

  variable->free(mContext);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = alloca i64*"
  "\n  %1 = load i64*, i64** %0"
  "\n  %2 = bitcast i64* %1 to i8*"
  "\n  call void @__destroyOwnerObjectFunction(i8* %2)\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}
