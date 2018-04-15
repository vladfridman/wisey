//
//  TestAutoCast.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/14/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link AutoCast}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "TestPrefix.hpp"
#include "wisey/AutoCast.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct AutoCastTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  AutoCastTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
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

TEST_F(AutoCastTest, castBooleanTest) {
  Value* result;
  Value* expressionValue = ConstantInt::get(Type::getInt1Ty(mLLVMContext), 1);

  EXPECT_EXIT(AutoCast::maybeCast(mContext,
                                  PrimitiveTypes::BOOLEAN_TYPE,
                                  expressionValue,
                                  PrimitiveTypes::VOID_TYPE,
                                  11),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(11\\): Error: Incompatible types: "
              "can not cast from type 'boolean' to 'void'");
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::BOOLEAN_TYPE,
                               expressionValue,
                               PrimitiveTypes::BOOLEAN_TYPE,
                               0);
  EXPECT_EQ(result, expressionValue);
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::BOOLEAN_TYPE,
                               expressionValue,
                               PrimitiveTypes::CHAR_TYPE,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = zext i1 true to i16", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::BOOLEAN_TYPE,
                               expressionValue,
                               PrimitiveTypes::INT_TYPE,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv1 = zext i1 true to i32", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::BOOLEAN_TYPE,
                               expressionValue,
                               PrimitiveTypes::LONG_TYPE,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv2 = zext i1 true to i64", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::BOOLEAN_TYPE,
                               expressionValue,
                               PrimitiveTypes::FLOAT_TYPE,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv3 = sitofp i1 true to float", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::BOOLEAN_TYPE,
                               expressionValue,
                               PrimitiveTypes::DOUBLE_TYPE,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv4 = sitofp i1 true to double", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(AutoCastTest, castCharTest) {
  Value* result;
  Value* expressionValue = ConstantInt::get(Type::getInt16Ty(mLLVMContext), 'a');
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext,
                                  PrimitiveTypes::CHAR_TYPE,
                                  expressionValue,
                                  PrimitiveTypes::VOID_TYPE,
                                  11),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(11\\): Error: Incompatible types: "
              "can not cast from type 'char' to 'void'");

  EXPECT_EXIT(AutoCast::maybeCast(mContext,
                                  PrimitiveTypes::CHAR_TYPE,
                                  expressionValue,
                                  PrimitiveTypes::BOOLEAN_TYPE,
                                  11),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(11\\): Error: Incompatible types: "
              "need explicit cast from type 'char' to 'boolean'");

  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::CHAR_TYPE,
                               expressionValue,
                               PrimitiveTypes::CHAR_TYPE,
                               0);
  EXPECT_EQ(result, expressionValue);

  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::CHAR_TYPE,
                               expressionValue,
                               PrimitiveTypes::INT_TYPE,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = zext i16 97 to i32", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::CHAR_TYPE,
                               expressionValue,
                               PrimitiveTypes::LONG_TYPE,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv1 = zext i16 97 to i64", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::CHAR_TYPE,
                               expressionValue,
                               PrimitiveTypes::FLOAT_TYPE,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv2 = sitofp i16 97 to float", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::CHAR_TYPE,
                               expressionValue,
                               PrimitiveTypes::DOUBLE_TYPE,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv3 = sitofp i16 97 to double", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(AutoCastTest, castIntTest) {
  Value* result;
  Value* expressionValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext,
                                  PrimitiveTypes::INT_TYPE,
                                  expressionValue,
                                  PrimitiveTypes::VOID_TYPE,
                                  11),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(11\\): Error: Incompatible types: "
              "can not cast from type 'int' to 'void'");
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext,
                                  PrimitiveTypes::INT_TYPE,
                                  expressionValue,
                                  PrimitiveTypes::BOOLEAN_TYPE,
                                  11),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(11\\): Error: Incompatible types: "
              "need explicit cast from type 'int' to 'boolean'");

  EXPECT_EXIT(AutoCast::maybeCast(mContext,
                                  PrimitiveTypes::INT_TYPE,
                                  expressionValue,
                                  PrimitiveTypes::CHAR_TYPE,
                                  11),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(11\\): Error: Incompatible types: "
              "need explicit cast from type 'int' to 'char'");
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::INT_TYPE,
                               expressionValue,
                               PrimitiveTypes::INT_TYPE,
                               0);
  EXPECT_EQ(result, expressionValue);
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::INT_TYPE,
                               expressionValue,
                               PrimitiveTypes::LONG_TYPE,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = zext i32 5 to i64", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext,
                                  PrimitiveTypes::INT_TYPE,
                                  expressionValue,
                                  PrimitiveTypes::FLOAT_TYPE,
                                  11),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(11\\): Error: Incompatible types: "
              "need explicit cast from type 'int' to 'float'");
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::INT_TYPE,
                               expressionValue,
                               PrimitiveTypes::DOUBLE_TYPE,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv1 = sitofp i32 5 to double", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(AutoCastTest, castLongTest) {
  Value* result;
  Value* expressionValue = ConstantInt::get(Type::getInt64Ty(mLLVMContext), 5l);
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext,
                                  PrimitiveTypes::LONG_TYPE,
                                  expressionValue,
                                  PrimitiveTypes::VOID_TYPE,
                                  11),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(11\\): Error: Incompatible types: "
              "can not cast from type 'long' to 'void'");
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext,
                                  PrimitiveTypes::LONG_TYPE,
                                  expressionValue,
                                  PrimitiveTypes::BOOLEAN_TYPE,
                                  11),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(11\\): Error: Incompatible types: "
              "need explicit cast from type 'long' to 'boolean'");
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext,
                                  PrimitiveTypes::LONG_TYPE,
                                  expressionValue,
                                  PrimitiveTypes::CHAR_TYPE,
                                  11),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(11\\): Error: Incompatible types: "
              "need explicit cast from type 'long' to 'char'");

  EXPECT_EXIT(AutoCast::maybeCast(mContext,
                                  PrimitiveTypes::LONG_TYPE,
                                  expressionValue,
                                  PrimitiveTypes::INT_TYPE,
                                  11),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(11\\): Error: Incompatible types: "
              "need explicit cast from type 'long' to 'int'");
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::LONG_TYPE,
                               expressionValue,
                               PrimitiveTypes::LONG_TYPE,
                               0);
  EXPECT_EQ(result, expressionValue);
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext,
                                  PrimitiveTypes::LONG_TYPE,
                                  expressionValue,
                                  PrimitiveTypes::FLOAT_TYPE,
                                  11),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(11\\): Error: Incompatible types: "
              "need explicit cast from type 'long' to 'float'");
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext,
                                  PrimitiveTypes::LONG_TYPE,
                                  expressionValue,
                                  PrimitiveTypes::DOUBLE_TYPE,
                                  11),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(11\\): Error: Incompatible types: "
              "need explicit cast from type 'long' to 'double'");
}

TEST_F(AutoCastTest, castFloatTest) {
  Value* result;
  Value* expressionValue = ConstantFP::get(Type::getFloatTy(mLLVMContext), 2.5);
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext,
                                  PrimitiveTypes::FLOAT_TYPE,
                                  expressionValue,
                                  PrimitiveTypes::VOID_TYPE,
                                  11),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(11\\): Error: Incompatible types: "
              "can not cast from type 'float' to 'void'");
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext,
                                  PrimitiveTypes::FLOAT_TYPE,
                                  expressionValue,
                                  PrimitiveTypes::BOOLEAN_TYPE,
                                  11),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(11\\): Error: Incompatible types: "
              "need explicit cast from type 'float' to 'boolean'");
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext,
                                  PrimitiveTypes::FLOAT_TYPE,
                                  expressionValue,
                                  PrimitiveTypes::CHAR_TYPE,
                                  11),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(11\\): Error: Incompatible types: "
              "need explicit cast from type 'float' to 'char'");
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext,
                                  PrimitiveTypes::FLOAT_TYPE,
                                  expressionValue,
                                  PrimitiveTypes::INT_TYPE,
                                  11),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(11\\): Error: Incompatible types: "
              "need explicit cast from type 'float' to 'int'");
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext,
                                  PrimitiveTypes::FLOAT_TYPE,
                                  expressionValue,
                                  PrimitiveTypes::LONG_TYPE,
                                  11),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(11\\): Error: Incompatible types: "
              "need explicit cast from type 'float' to 'long'");
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::FLOAT_TYPE,
                               expressionValue,
                               PrimitiveTypes::FLOAT_TYPE,
                               0);
  EXPECT_EQ(result, expressionValue);
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::FLOAT_TYPE,
                               expressionValue,
                               PrimitiveTypes::DOUBLE_TYPE,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = fpext float 2.500000e+00 to double", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(AutoCastTest, castDoubleTest) {
  Value* result;
  Value* expressionValue = ConstantFP::get(Type::getDoubleTy(mLLVMContext), 2.5);
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext,
                                  PrimitiveTypes::DOUBLE_TYPE,
                                  expressionValue,
                                  PrimitiveTypes::VOID_TYPE,
                                  11),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(11\\): Error: Incompatible types: "
              "can not cast from type 'double' to 'void'");
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext,
                                  PrimitiveTypes::DOUBLE_TYPE,
                                  expressionValue,
                                  PrimitiveTypes::BOOLEAN_TYPE,
                                  11),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(11\\): Error: Incompatible types: "
              "need explicit cast from type 'double' to 'boolean'");
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext,
                                  PrimitiveTypes::DOUBLE_TYPE,
                                  expressionValue,
                                  PrimitiveTypes::CHAR_TYPE,
                                  11),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(11\\): Error: Incompatible types: "
              "need explicit cast from type 'double' to 'char'");
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext,
                                  PrimitiveTypes::DOUBLE_TYPE,
                                  expressionValue,
                                  PrimitiveTypes::INT_TYPE,
                                  11),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(11\\): Error: Incompatible types: "
              "need explicit cast from type 'double' to 'int'");
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext,
                                  PrimitiveTypes::DOUBLE_TYPE,
                                  expressionValue,
                                  PrimitiveTypes::LONG_TYPE,
                                  11),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(11\\): Error: Incompatible types: "
              "need explicit cast from type 'double' to 'long'");
  
  EXPECT_EXIT(AutoCast::maybeCast(mContext,
                                  PrimitiveTypes::DOUBLE_TYPE,
                                  expressionValue,
                                  PrimitiveTypes::FLOAT_TYPE,
                                  11),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(11\\): Error: Incompatible types: "
              "need explicit cast from type 'double' to 'float'");
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::DOUBLE_TYPE,
                               expressionValue,
                               PrimitiveTypes::DOUBLE_TYPE,
                               0);
  EXPECT_EQ(result, expressionValue);
}
