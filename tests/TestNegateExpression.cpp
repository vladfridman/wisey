//
//  TestNegateExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/23/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link NegateExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/NegateExpression.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct NegateExpressionTest : Test {
  IRGenerationContext mContext;
  Model* mModel;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  NiceMock<MockExpression> mExpression;
  
  NegateExpressionTest() {
    LLVMContext& llvmContext = mContext.getLLVMContext();
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(llvmContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~NegateExpressionTest() {
    delete mStringStream;
  }
};

TEST_F(NegateExpressionTest, negateIntExpressionTest) {
  Value* value = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 3);
  ON_CALL(mExpression, generateIR(_)).WillByDefault(Return(value));
  ON_CALL(mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  NegateExpression negateExpression(mExpression);
  
  Value* result = negateExpression.generateIR(mContext);

  *mStringStream << *result;
  EXPECT_STREQ("  %sub = sub i32 0, 3", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(NegateExpressionTest, negateFloatExpressionTest) {
  Value* value = ConstantFP::get(Type::getFloatTy(mContext.getLLVMContext()), 2.5);
  ON_CALL(mExpression, generateIR(_)).WillByDefault(Return(value));
  ON_CALL(mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));
  NegateExpression negateExpression(mExpression);
  
  Value* result = negateExpression.generateIR(mContext);
  
  *mStringStream << *result;
  EXPECT_STREQ("  %fsub = fsub float 0.000000e+00, 2.500000e+00", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(NegateExpressionTest, negateIncompatibleTypeDeathTest) {
  ON_CALL(mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::VOID_TYPE));
  NegateExpression negateExpression(mExpression);
  Mock::AllowLeak(&mExpression);

  EXPECT_EXIT(negateExpression.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Can not apply negate operation to type 'void'");
}

TEST_F(NegateExpressionTest, releaseOwnershipDeathTest) {
  NegateExpression negateExpression(mExpression);
  Mock::AllowLeak(&mExpression);
  
  EXPECT_EXIT(negateExpression.releaseOwnership(mContext),
              ::testing::ExitedWithCode(1),
              "Can not release ownership of a negate expression result, it is not a heap pointer");
}

TEST_F(TestFileSampleRunner, negateIntRunTest) {
  runFile("tests/samples/test_unary_minus.yz", "-5");
}
