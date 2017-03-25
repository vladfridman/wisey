//
//  TestCast.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Cast}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/Cast.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

struct CastTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  CastTest() : mLLVMContext(mContext.getLLVMContext()) {
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
  
  ~CastTest() {
    delete mStringStream;
  }
};

TEST_F(CastTest, ExitIncompatobleTypesTest) {
  EXPECT_EXIT(Cast::exitIncopatibleTypes(PrimitiveTypes::CHAR_TYPE, PrimitiveTypes::INT_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incopatible types: can not cast from type 'char' to 'int'");
}

TEST_F(CastTest, WidenIntCastTest) {
  Value* expressionValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
  
  Value* result = Cast::widenIntCast(mContext, expressionValue, PrimitiveTypes::LONG_TYPE);
  
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = zext i32 5 to i64", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(CastTest, TruncIntCastTest) {
  Value* expressionValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
  
  Value* result = Cast::truncIntCast(mContext, expressionValue, PrimitiveTypes::BOOLEAN_TYPE);
  
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = trunc i32 5 to i1", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(CastTest, TruncIntToFloatCastTest) {
  Value* expressionValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
  
  Value* result = Cast::intToFloatCast(mContext, expressionValue, PrimitiveTypes::DOUBLE_TYPE);
  
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = sitofp i32 5 to double", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(CastTest, FloatToIntCastTest) {
  Value* expressionValue = ConstantFP::get(Type::getDoubleTy(mLLVMContext), 2.5);
  
  Value* result = Cast::floatToIntCast(mContext, expressionValue, PrimitiveTypes::CHAR_TYPE);
  
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = fptosi double 2.500000e+00 to i16", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(CastTest, TruncFloatCastTest) {
  Value* expressionValue = ConstantFP::get(Type::getDoubleTy(mLLVMContext), 2.5);
  
  Value* result = Cast::truncFloatCast(mContext, expressionValue, PrimitiveTypes::FLOAT_TYPE);
  
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = fptrunc double 2.500000e+00 to float", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(CastTest, widenFloatCastTest) {
  Value* expressionValue = ConstantFP::get(Type::getFloatTy(mLLVMContext), 2.5);
  
  Value* result = Cast::widenFloatCast(mContext, expressionValue, PrimitiveTypes::DOUBLE_TYPE);
  
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = fpext float 2.500000e+00 to double", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(TestFileSampleRunner, CastFloatToIntTest) {
  runFile("tests/samples/test_cast_float_to_int.yz", "3");
}