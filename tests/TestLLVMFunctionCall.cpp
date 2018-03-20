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

    vector<Type*> argumentTypes;
    argumentTypes.push_back(Type::getInt8Ty(mLLVMContext)->getPointerTo());
    Type* llvmReturnType = Type::getInt16Ty(mLLVMContext)->getPointerTo();
    FunctionType* ftype = FunctionType::get(llvmReturnType, argumentTypes, false);
    
    Function::Create(ftype, GlobalValue::ExternalLinkage, "myfunction", mContext.getModule());

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
  const NativeType* nativeType = mLLVMFunctionCall->getType(mContext);
  
  EXPECT_TRUE(nativeType->isNative());
  EXPECT_TRUE(nativeType->isReference());
  
  EXPECT_EQ(Type::getInt16Ty(mLLVMContext)->getPointerTo(), nativeType->getLLVMType(mContext));
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
