//
//  TestBooleanNotExpression.cpp
//  Wisey
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
#include "wisey/BooleanNotExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
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
  NiceMock<MockExpression>* mExpression;
  
  BooleanNotExpressionTest() : mExpression(new NiceMock<MockExpression>()) {
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
  
  static void printExpression(iostream& stream) {
    stream << "value";
  }
};

TEST_F(BooleanNotExpressionTest, getVariableTest) {
  BooleanNotExpression booleanNotExpression(mExpression);
  EXPECT_EQ(booleanNotExpression.getVariable(mContext), nullptr);
}

TEST_F(BooleanNotExpressionTest, negateIntExpressionTest) {
  Value* one = ConstantInt::get(Type::getInt1Ty(mContext.getLLVMContext()), 1);
  ON_CALL(*mExpression, generateIR(_)).WillByDefault(Return(one));
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::BOOLEAN_TYPE));
  BooleanNotExpression booleanNotExpression(mExpression);
  
  Value* result = booleanNotExpression.generateIR(mContext);
  
  *mStringStream << *result;
  EXPECT_STREQ("  %lnot = xor i1 true, true", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(BooleanNotExpressionTest, existsInOuterScopeTest) {
  BooleanNotExpression booleanNotExpression(mExpression);
  
  EXPECT_FALSE(booleanNotExpression.existsInOuterScope(mContext));
}

TEST_F(BooleanNotExpressionTest, printToStreamTest) {
  BooleanNotExpression booleanNotExpression(mExpression);
  ON_CALL(*mExpression, printToStream(_)).WillByDefault(Invoke(printExpression));
  
  stringstream stringStream;
  booleanNotExpression.printToStream(stringStream);
  
  EXPECT_STREQ("!value", stringStream.str().c_str());
}

TEST_F(BooleanNotExpressionTest, negateIncompatibleTypeDeathTest) {
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::VOID_TYPE));
  BooleanNotExpression booleanNotExpression(mExpression);
  Mock::AllowLeak(mExpression);
  
  EXPECT_EXIT(booleanNotExpression.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Boolean NOT operator '!' can only be applied to boolean types");
}

TEST_F(BooleanNotExpressionTest, releaseOwnershipDeathTest) {
  BooleanNotExpression booleanNotExpression(mExpression);
  Mock::AllowLeak(mExpression);
  
  EXPECT_EXIT(booleanNotExpression.releaseOwnership(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Can not release ownership of a boolean NOT expression, "
              "it is not a heap pointer");
}

TEST_F(BooleanNotExpressionTest, addReferenceToOwnerDeathTest) {
  BooleanNotExpression booleanNotExpression(mExpression);
  Mock::AllowLeak(mExpression);
  
  EXPECT_EXIT(booleanNotExpression.addReferenceToOwner(mContext, NULL),
              ::testing::ExitedWithCode(1),
              "Error: Can not add a reference to non owner type boolean NOT expression");
}

TEST_F(TestFileSampleRunner, booleanNotRunTest) {
  runFile("tests/samples/test_boolean_not.yz", "1");
}
