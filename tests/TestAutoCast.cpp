//
//  TestAutoCast.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/14/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link AutoCast}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "yazyk/AutoCast.hpp"
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
};

struct AutoCastTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockExpression> mExpression;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  AutoCastTest() : mLLVMContext(mContext.getLLVMContext()) {
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~AutoCastTest() {
    delete mStringStream;
  }
};

TEST_F(AutoCastTest, CastBooleanTest) {
  Mock::AllowLeak(&mExpression);
  Value* result;
  Value* expressionValue = ConstantInt::get(Type::getInt1Ty(mLLVMContext), 1);
  ON_CALL(mExpression, generateIR(_)).WillByDefault(Return(expressionValue));
  ON_CALL(mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::BOOLEAN_TYPE));

  EXPECT_EXIT(AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::VOID_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: can not cast from type 'boolean' to 'void'");
  
  result = AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::BOOLEAN_TYPE);
  EXPECT_EQ(result, expressionValue);
  
  result = AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::CHAR_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = zext i1 true to i16", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::INT_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv1 = zext i1 true to i32", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::LONG_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv2 = zext i1 true to i64", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::FLOAT_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv3 = sitofp i1 true to float", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::DOUBLE_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv4 = sitofp i1 true to double", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(AutoCastTest, CastCharTest) {
  Mock::AllowLeak(&mExpression);
  Value* result;
  Value* expressionValue = ConstantInt::get(Type::getInt16Ty(mLLVMContext), 'a');
  ON_CALL(mExpression, generateIR(_)).WillByDefault(Return(expressionValue));
  ON_CALL(mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::CHAR_TYPE));
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::VOID_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: can not cast from type 'char' to 'void'");

  EXPECT_EXIT(AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::BOOLEAN_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: need explicit cast from type 'char' to 'boolean'");

  result = AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::CHAR_TYPE);
  EXPECT_EQ(result, expressionValue);

  result = AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::INT_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = zext i16 97 to i32", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::LONG_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv1 = zext i16 97 to i64", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::FLOAT_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv2 = sitofp i16 97 to float", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::DOUBLE_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv3 = sitofp i16 97 to double", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(AutoCastTest, CastIntTest) {
  Mock::AllowLeak(&mExpression);
  Value* result;
  Value* expressionValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
  ON_CALL(mExpression, generateIR(_)).WillByDefault(Return(expressionValue));
  ON_CALL(mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::VOID_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: can not cast from type 'int' to 'void'");
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::BOOLEAN_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: need explicit cast from type 'int' to 'boolean'");

  EXPECT_EXIT(AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::CHAR_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: need explicit cast from type 'int' to 'char'");
  
  result = AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::INT_TYPE);
  EXPECT_EQ(result, expressionValue);
  
  result = AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::LONG_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = zext i32 5 to i64", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::FLOAT_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: need explicit cast from type 'int' to 'float'");
  
  result = AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::DOUBLE_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv1 = sitofp i32 5 to double", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(AutoCastTest, CastLongTest) {
  Mock::AllowLeak(&mExpression);
  Value* result;
  Value* expressionValue = ConstantInt::get(Type::getInt64Ty(mLLVMContext), 5l);
  ON_CALL(mExpression, generateIR(_)).WillByDefault(Return(expressionValue));
  ON_CALL(mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::LONG_TYPE));
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::VOID_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: can not cast from type 'long' to 'void'");
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::BOOLEAN_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: need explicit cast from type 'long' to 'boolean'");
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::CHAR_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: need explicit cast from type 'long' to 'char'");

  EXPECT_EXIT(AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::INT_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: need explicit cast from type 'long' to 'int'");
  
  result = AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::LONG_TYPE);
  EXPECT_EQ(result, expressionValue);
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::FLOAT_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: need explicit cast from type 'long' to 'float'");
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::DOUBLE_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: need explicit cast from type 'long' to 'double'");
}

TEST_F(AutoCastTest, CastFloatTest) {
  Mock::AllowLeak(&mExpression);
  Value* result;
  Value* expressionValue = ConstantFP::get(Type::getFloatTy(mLLVMContext), 2.5);
  ON_CALL(mExpression, generateIR(_)).WillByDefault(Return(expressionValue));
  ON_CALL(mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::VOID_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: can not cast from type 'float' to 'void'");
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::BOOLEAN_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: need explicit cast from type 'float' to 'boolean'");
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::CHAR_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: need explicit cast from type 'float' to 'char'");
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::INT_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: need explicit cast from type 'float' to 'int'");
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::LONG_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: need explicit cast from type 'float' to 'long'");
  
  result = AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::FLOAT_TYPE);
  EXPECT_EQ(result, expressionValue);
  
  result = AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::DOUBLE_TYPE);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = fpext float 2.500000e+00 to double", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(AutoCastTest, CastDoubleTest) {
  Mock::AllowLeak(&mExpression);
  Value* result;
  Value* expressionValue = ConstantFP::get(Type::getDoubleTy(mLLVMContext), 2.5);
  ON_CALL(mExpression, generateIR(_)).WillByDefault(Return(expressionValue));
  ON_CALL(mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::DOUBLE_TYPE));
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::VOID_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: can not cast from type 'double' to 'void'");
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::BOOLEAN_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: need explicit cast from type 'double' to 'boolean'");
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::CHAR_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: need explicit cast from type 'double' to 'char'");
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::INT_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: need explicit cast from type 'double' to 'int'");
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::LONG_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: need explicit cast from type 'double' to 'long'");
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::FLOAT_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: need explicit cast from type 'double' to 'float'");
  
  result = AutoCast::maybeCast(mContext, mExpression, PrimitiveTypes::DOUBLE_TYPE);
  EXPECT_EQ(result, expressionValue);
}

