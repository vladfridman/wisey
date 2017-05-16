//
//  TestBooleanNotExpression.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link BooleanNotExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileSampleRunner.hpp"
#include "yazyk/BooleanNotExpression.hpp"
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

struct BooleanNotExpressionTest : Test {
  IRGenerationContext mContext;
  Model* mModel;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  NiceMock<MockExpression> mExpression;
  
  BooleanNotExpressionTest() {
    LLVMContext& llvmContext = mContext.getLLVMContext();
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(llvmContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(llvmContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~BooleanNotExpressionTest() {
    delete mStringStream;
  }
};

TEST_F(BooleanNotExpressionTest, negateIntExpressionTest) {
  Value* one = ConstantInt::get(Type::getInt1Ty(mContext.getLLVMContext()), 1);
  ON_CALL(mExpression, generateIR(_)).WillByDefault(Return(one));
  ON_CALL(mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::BOOLEAN_TYPE));
  BooleanNotExpression booleanNotExpression(mExpression);
  
  Value* result = booleanNotExpression.generateIR(mContext);
  
  *mStringStream << *result;
  EXPECT_STREQ("  %lnot = xor i1 true, true", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(BooleanNotExpressionTest, negateIncompatibleTypeDeathTest) {
  ON_CALL(mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::VOID_TYPE));
  BooleanNotExpression booleanNotExpression(mExpression);
  Mock::AllowLeak(&mExpression);
  
  EXPECT_EXIT(booleanNotExpression.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Boolean NOT operator '!' can only be applied to boolean types");
}

TEST_F(BooleanNotExpressionTest, releaseOwnershipDeathTest) {
  BooleanNotExpression booleanNotExpression(mExpression);
  Mock::AllowLeak(&mExpression);
  
  EXPECT_EXIT(booleanNotExpression.releaseOwnership(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Can not release ownership of a boolean NOT expression, "
              "it is not a heap pointer");
}

TEST_F(TestFileSampleRunner, booleanNotRunTest) {
  runFile("tests/samples/test_boolean_not.yz", "1");
}
