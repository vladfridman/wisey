//
//  TestLLVMFunctionCall.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMFunctionCall}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileRunner.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMFunctionCall.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LLVMFunctionCallTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  LLVMFunctionCall* mLLVMFunctionCall;
  NiceMock<MockExpression>* mMockExpression = new NiceMock<MockExpression>();
  BasicBlock* mBasicBlock;
  Function* mFunction;
  LLVMFunctionType* mLLVMFunctionType;
  string mStringBuffer;
  raw_string_ostream* mStringStream;

  LLVMFunctionCallTest() : mLLVMContext(mContext.getLLVMContext()) {
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "test",
                                 mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);

    vector<const IType*> argumentTypes;
    vector<Type*> llvmArgumentTypes;
    argumentTypes.push_back(LLVMPrimitiveTypes::I8->getPointerType());
    llvmArgumentTypes.push_back(LLVMPrimitiveTypes::I8->getPointerType()->getLLVMType(mContext));
    const IType* returnType = LLVMPrimitiveTypes::I16->getPointerType();
    Type* llvmReturnType = returnType->getLLVMType(mContext);
    FunctionType* llvmFunctionType = FunctionType::get(llvmReturnType, llvmArgumentTypes, false);
    mLLVMFunctionType = mContext.getLLVMFunctionType(returnType, argumentTypes);
    mContext.registerLLVMInternalFunctionNamedType("myfunction", mLLVMFunctionType);
    
    Function::Create(llvmFunctionType,
                     GlobalValue::ExternalLinkage,
                     "myfunction",
                     mContext.getModule());

    ON_CALL(*mMockExpression, getType(_)).
    WillByDefault(Return(LLVMPrimitiveTypes::I8->getPointerType()));
    Value* null = ConstantPointerNull::get(LLVMPrimitiveTypes::I8->getPointerType()->
                                           getLLVMType(mContext));
    ON_CALL(*mMockExpression, generateIR(_, _)).WillByDefault(Return(null));
    ExpressionList expressionList;
    expressionList.push_back(mMockExpression);

    mLLVMFunctionCall = new LLVMFunctionCall("myfunction", expressionList, 0);
  }
  
  ~LLVMFunctionCallTest() {
    delete mMockExpression;
  }
};

TEST_F(LLVMFunctionCallTest, getTypeTest) {
  const IType* returnType = mLLVMFunctionCall->getType(mContext);
  
  EXPECT_TRUE(returnType->isNative());
  EXPECT_TRUE(returnType->isPointer());
  
  EXPECT_EQ(Type::getInt16Ty(mLLVMContext)->getPointerTo(), returnType->getLLVMType(mContext));
}

TEST_F(LLVMFunctionCallTest, generateIRTest) {
  mLLVMFunctionCall->generateIR(mContext, LLVMPrimitiveTypes::VOID);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = call i16* @myfunction(i8* null)\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(TestFileRunner, llvmFunctionCallRunTest) {
  runFile("tests/samples/test_llvm_function_call.yz", "3");
}

TEST_F(TestFileRunner, llvmFunctionCallFullPathRunTest) {
  runFile("tests/samples/test_llvm_function_call_full_path.yz", "3");
}

TEST_F(TestFileRunner, llvmFunctionCallInInterfaceRunTest) {
  runFile("tests/samples/test_llvm_function_call_in_interface.yz", "8");
}

TEST_F(TestFileRunner, llvmFunctionCallPrivateFailDeathRunTest) {
  expectFailCompile("tests/samples/test_llvm_function_call_private_fail.yz",
                    1,
                    "Error: Static method 'getValue' of object "
                    "systems.vos.wisey.compiler.tests.IMyInterface is private");
}
