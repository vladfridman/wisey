//
//  TestFunctionCall.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link FunctionCall}

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/FunctionCall.hpp"
#include "yazyk/Identifier.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/PrimitiveTypes.hpp"

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
  MOCK_CONST_METHOD1(generateIR, Value* (IRGenerationContext&));
  MOCK_CONST_METHOD1(getType, IType* (IRGenerationContext&));
  MOCK_CONST_METHOD1(releaseOwnership, void (IRGenerationContext&));
};

struct FunctionCallTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockExpression> mExpression;
  ExpressionList mArgumentList;
  Type* mIntType;
  BasicBlock* mBasicBlock = BasicBlock::Create(mContext.getLLVMContext(), "entry");
  string mStringBuffer;
  raw_string_ostream* mStringStream;

public:
  
  FunctionCallTest() :
    mLLVMContext(mContext.getLLVMContext()),
    mIntType(Type::getInt32Ty(mContext.getLLVMContext())) {
      mContext.setBasicBlock(mBasicBlock);
      mContext.getScopes().pushScope();
      mStringStream = new raw_string_ostream(mStringBuffer);
      Value* value = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 5);
      ON_CALL(mExpression, generateIR(_)).WillByDefault(Return(value));
      mArgumentList.push_back(&mExpression);
  }
  
  ~FunctionCallTest() {
    delete mBasicBlock;
    delete mStringStream;
  }
};

TEST_F(FunctionCallTest, FunctionDoesNotExistDeathTest) {
  FunctionCall functionCall("foo", mArgumentList);
  Mock::AllowLeak(&mExpression);

  EXPECT_EXIT(functionCall.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: no such function foo");
}

TEST_F(FunctionCallTest, IntFunctionCall) {
  vector<Type*> argTypes;
  argTypes.push_back(mIntType);
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argTypes);
  FunctionType* ftype = FunctionType::get(mIntType, argTypesArray, false);
  Function::Create(ftype, GlobalValue::InternalLinkage, "foo", mContext.getModule());
  mContext.addGlobalFunction(PrimitiveTypes::INT_TYPE, "foo");
  FunctionCall functionCall("foo", mArgumentList);
  
  Value* irValue = functionCall.generateIR(mContext);
  
  *mStringStream << *irValue;
  EXPECT_STREQ("  %call = call i32 @foo(i32 5)", mStringStream->str().c_str());
  EXPECT_EQ(functionCall.getType(mContext), PrimitiveTypes::INT_TYPE);
}

TEST_F(FunctionCallTest, VoidFunctionCall) {
  vector<Type*> argTypes;
  argTypes.push_back(mIntType);
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argTypes);
  FunctionType* ftype = FunctionType::get(Type::getVoidTy(mLLVMContext),
                                          argTypesArray,
                                          false);
  Function::Create(ftype, GlobalValue::InternalLinkage, "foo", mContext.getModule());
  FunctionCall functionCall("foo", mArgumentList);
  
  Value* irValue = functionCall.generateIR(mContext);
  
  *mStringStream << *irValue;
  EXPECT_STREQ("  call void @foo(i32 5)", mStringStream->str().c_str());
}

TEST_F(FunctionCallTest, PrintfFunctionCall) {
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
  FunctionCall functionCall("printf", mArgumentList);
  
  Value* irValue = functionCall.generateIR(mContext);
  
  *mStringStream << *irValue;
  string expected = string() +
    "  %call = call i32 (i8*, ...) " +
    "@printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str, i32 0, i32 0))";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FunctionCallTest, releaseOwndershipTest) {
  FunctionCall functionCall("foo", mArgumentList);
  Mock::AllowLeak(&mExpression);
  
  EXPECT_EXIT(functionCall.releaseOwnership(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Function call result ownership release is not implemented");
}

TEST_F(TestFileSampleRunner, FunctionCallRunTest) {
  runFile("tests/samples/test_function_call.yz", "5");
}

TEST_F(TestFileSampleRunner, FunctionCallInExpressionTest) {
  runFile("tests/samples/test_function_call_in_expression.yz", "4");
}
