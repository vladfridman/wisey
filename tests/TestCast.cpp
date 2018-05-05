//
//  TestCast.cpp
//  Wisey
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

#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/Cast.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct CastTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  CastTest() : mLLVMContext(mContext.getLLVMContext()) {
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
  
  ~CastTest() {
    delete mStringStream;
  }
};

TEST_F(CastTest, exitIncompatobleTypesTest) {
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(Cast::exitIncompatibleTypes(mContext,
                                               PrimitiveTypes::CHAR,
                                               PrimitiveTypes::INT,
                                               11));
  EXPECT_STREQ("/tmp/source.yz(11): Error: Incompatible types: can not cast from type 'char' to 'int'\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(CastTest, widenIntCastTest) {
  Value* expressionValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
  
  Value* result = Cast::widenIntCast(mContext, expressionValue, PrimitiveTypes::LONG);
  
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = zext i32 5 to i64", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(CastTest, truncIntCastTest) {
  Value* expressionValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
  
  Value* result = Cast::truncIntCast(mContext, expressionValue, PrimitiveTypes::BOOLEAN);
  
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = trunc i32 5 to i1", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(CastTest, truncIntToFloatCastTest) {
  Value* expressionValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
  
  Value* result = Cast::intToFloatCast(mContext, expressionValue, PrimitiveTypes::DOUBLE);
  
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = sitofp i32 5 to double", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(CastTest, floatToIntCastTest) {
  Value* expressionValue = ConstantFP::get(Type::getDoubleTy(mLLVMContext), 2.5);
  
  Value* result = Cast::floatToIntCast(mContext, expressionValue, PrimitiveTypes::CHAR);
  
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = fptosi double 2.500000e+00 to i16", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(CastTest, truncFloatCastTest) {
  Value* expressionValue = ConstantFP::get(Type::getDoubleTy(mLLVMContext), 2.5);
  
  Value* result = Cast::truncFloatCast(mContext, expressionValue, PrimitiveTypes::FLOAT);
  
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = fptrunc double 2.500000e+00 to float", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(CastTest, widenFloatCastTest) {
  Value* expressionValue = ConstantFP::get(Type::getFloatTy(mLLVMContext), 2.5);
  
  Value* result = Cast::widenFloatCast(mContext, expressionValue, PrimitiveTypes::DOUBLE);
  
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = fpext float 2.500000e+00 to double", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(TestFileRunner, castFloatToIntRunTest) {
  runFile("tests/samples/test_cast_float_to_int.yz", "3");
}
