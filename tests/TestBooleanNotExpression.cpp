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

TEST_F(BooleanNotExpressionTest, isAssignableTest) {
  BooleanNotExpression booleanNotExpression(mExpression, 0);
  
  EXPECT_FALSE(booleanNotExpression.isAssignable());
}

TEST_F(BooleanNotExpressionTest, negateIntExpressionTest) {
  Value* one = ConstantInt::get(Type::getInt1Ty(mContext.getLLVMContext()), 1);
  ON_CALL(*mExpression, generateIR(_, _)).WillByDefault(Return(one));
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::BOOLEAN));
  BooleanNotExpression booleanNotExpression(mExpression, 0);
  
  Value* result = booleanNotExpression.generateIR(mContext, PrimitiveTypes::VOID);
  
  *mStringStream << *result;
  EXPECT_STREQ("  %0 = xor i1 true, true", mStringStream->str().c_str());
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
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::VOID));
  ON_CALL(*mExpression, getLine()).WillByDefault(Return(3));
  BooleanNotExpression booleanNotExpression(mExpression, 3);
  Mock::AllowLeak(mExpression);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(booleanNotExpression.generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Incompatible types: can not cast from type 'void' to 'boolean'\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, booleanNotRunTest) {
  runFile("tests/samples/test_boolean_not.yz", "1");
}
