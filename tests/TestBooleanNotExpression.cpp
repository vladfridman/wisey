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
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
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
    TestPrefix::generateIR(mContext);
    
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
  
  static void printExpression(IRGenerationContext& context, iostream& stream) {
    stream << "value";
  }
};

TEST_F(BooleanNotExpressionTest, isConstantTest) {
  BooleanNotExpression booleanNotExpression(mExpression, 0);

  EXPECT_FALSE(booleanNotExpression.isConstant());
}

TEST_F(BooleanNotExpressionTest, getVariableTest) {
  BooleanNotExpression booleanNotExpression(mExpression, 0);
  vector<const IExpression*> arrayIndices;

  EXPECT_EQ(booleanNotExpression.getVariable(mContext, arrayIndices), nullptr);
}

TEST_F(BooleanNotExpressionTest, negateIntExpressionTest) {
  Value* one = ConstantInt::get(Type::getInt1Ty(mContext.getLLVMContext()), 1);
  ON_CALL(*mExpression, generateIR(_, _)).WillByDefault(Return(one));
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::BOOLEAN_TYPE));
  BooleanNotExpression booleanNotExpression(mExpression, 0);
  
  Value* result = booleanNotExpression.generateIR(mContext, PrimitiveTypes::VOID_TYPE);
  
  *mStringStream << *result;
  EXPECT_STREQ("  %lnot = xor i1 true, true", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(BooleanNotExpressionTest, printToStreamTest) {
  BooleanNotExpression booleanNotExpression(mExpression, 0);
  ON_CALL(*mExpression, printToStream(_, _)).WillByDefault(Invoke(printExpression));
  
  stringstream stringStream;
  booleanNotExpression.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("!value", stringStream.str().c_str());
}

TEST_F(BooleanNotExpressionTest, negateIncompatibleTypeDeathTest) {
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::VOID_TYPE));
  BooleanNotExpression booleanNotExpression(mExpression, 3);
  Mock::AllowLeak(mExpression);
  
  EXPECT_EXIT(booleanNotExpression.generateIR(mContext, PrimitiveTypes::VOID_TYPE),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(3\\): Error: Boolean NOT operator '!' can only be applied "
              "to boolean types");
}

TEST_F(TestFileRunner, booleanNotRunTest) {
  runFile("tests/samples/test_boolean_not.yz", "1");
}
