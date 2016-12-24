//
//  TestMethodCall.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link MethodCall}

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/Identifier.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/MethodCall.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

class MockExpression : public IExpression {
public:
  MOCK_METHOD1(generateIR, Value* (IRGenerationContext&));
};

struct MethodCallTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockExpression> mExpression;
  Identifier mFooMethodIdentifier;
  Identifier mPrintfMethodIdentifier;
  ExpressionList mArgumentList;
  Type* mIntType;
  BasicBlock* mBasicBlock = BasicBlock::Create(mContext.getLLVMContext(), "entry");
  string mStringBuffer;
  raw_string_ostream* mStringStream;

public:
  
  MethodCallTest() :
    mLLVMContext(mContext.getLLVMContext()),
    mFooMethodIdentifier(Identifier("foo")),
    mPrintfMethodIdentifier(Identifier("printf")),
    mIntType(Type::getInt32Ty(mContext.getLLVMContext())) {
      mContext.pushBlock(mBasicBlock);
      mStringStream = new raw_string_ostream(mStringBuffer);
      Value* value = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 5);
      ON_CALL(mExpression, generateIR(_)).WillByDefault(Return(value));
      mArgumentList.push_back(&mExpression);
  }
  
  ~MethodCallTest() {
    delete mBasicBlock;
    delete mStringStream;
  }
};

TEST_F(MethodCallTest, MethodDoesNotExistDeathTest) {
  MethodCall methodCall(mFooMethodIdentifier, mArgumentList);
  Mock::AllowLeak(&mExpression);

  EXPECT_EXIT(methodCall.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: no such function foo");
}

TEST_F(MethodCallTest, IntMethodCall) {
  vector<Type*> argTypes;
  argTypes.push_back(mIntType);
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argTypes);
  FunctionType* ftype = FunctionType::get(mIntType, argTypesArray, false);
  Function::Create(ftype, GlobalValue::InternalLinkage, "foo", mContext.getModule());
  MethodCall methodCall(mFooMethodIdentifier, mArgumentList);
  
  Value* irValue = methodCall.generateIR(mContext);
  
  *mStringStream << *irValue;
  EXPECT_STREQ("  %call = call i32 @foo(i32 5)", mStringStream->str().c_str());
}

TEST_F(MethodCallTest, VoidMethodCall) {
  vector<Type*> argTypes;
  argTypes.push_back(mIntType);
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argTypes);
  FunctionType* ftype = FunctionType::get(Type::getVoidTy(mLLVMContext),
                                          argTypesArray,
                                          false);
  Function::Create(ftype, GlobalValue::InternalLinkage, "foo", mContext.getModule());
  MethodCall methodCall(mFooMethodIdentifier, mArgumentList);
  
  Value* irValue = methodCall.generateIR(mContext);
  
  *mStringStream << *irValue;
  EXPECT_STREQ("  call void @foo(i32 5)", mStringStream->str().c_str());
}

TEST_F(MethodCallTest, PrintfMethodCall) {
  Constant* strConstant = ConstantDataArray::getString(mLLVMContext, "test");
  GlobalVariable* globalVariableString =
  new GlobalVariable(*mContext.getModule(),
                     strConstant->getType(),
                     true,
                     GlobalValue::InternalLinkage,
                     strConstant,
                     ".str");
  Constant* zero = Constant::getNullValue(IntegerType::getInt32Ty(mLLVMContext));
  Constant* indices[] = {zero, zero};
  Constant* strVal = ConstantExpr::getGetElementPtr(NULL,
                                                    globalVariableString,
                                                    indices,
                                                    true);
  ON_CALL(mExpression, generateIR(_)).WillByDefault(Return(strVal));
  MethodCall methodCall(mPrintfMethodIdentifier, mArgumentList);
  
  Value* irValue = methodCall.generateIR(mContext);
  
  *mStringStream << *irValue;
  string expected = string() +
    "  %call = call i32 (i8*, ...) " +
    "@printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str, i32 0, i32 0))";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(TestFileSampleRunner, MethodCallRunTest) {
  runFile("tests/samples/method_call_run_test.yz", "5");
}
