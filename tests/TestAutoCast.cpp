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
#include "AutoCast.hpp"
#include "IRGenerationContext.hpp"
#include "PrimitiveTypes.hpp"

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
    BasicBlock* declareBlock = BasicBlock::Create(mLLVMContext, "declare", function);
    mBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setDeclarationsBlock(declareBlock);
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

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(AutoCast::maybeCast(mContext,
                                       PrimitiveTypes::BOOLEAN,
                                       expressionValue,
                                       PrimitiveTypes::VOID,
                                       11));
  EXPECT_STREQ("/tmp/source.yz(11): Error: Incompatible types: can not cast from type boolean to void\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);

  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::BOOLEAN,
                               expressionValue,
                               PrimitiveTypes::BOOLEAN,
                               0);
  EXPECT_EQ(result, expressionValue);
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::BOOLEAN,
                               expressionValue,
                               PrimitiveTypes::CHAR,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = zext i1 true to i8", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::BOOLEAN,
                               expressionValue,
                               PrimitiveTypes::BYTE,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv1 = zext i1 true to i8", mStringStream->str().c_str());
  mStringBuffer.clear();

  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::BOOLEAN,
                               expressionValue,
                               PrimitiveTypes::INT,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv2 = zext i1 true to i32", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::BOOLEAN,
                               expressionValue,
                               PrimitiveTypes::LONG,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv3 = zext i1 true to i64", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::BOOLEAN,
                               expressionValue,
                               PrimitiveTypes::FLOAT,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv4 = sitofp i1 true to float", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::BOOLEAN,
                               expressionValue,
                               PrimitiveTypes::DOUBLE,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv5 = sitofp i1 true to double", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(AutoCastTest, castCharTest) {
  Value* result;
  Value* expressionValue = ConstantInt::get(Type::getInt8Ty(mLLVMContext), 'a');
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(AutoCast::maybeCast(mContext,
                                       PrimitiveTypes::CHAR,
                                       expressionValue,
                                       PrimitiveTypes::VOID,
                                       11));
  EXPECT_STREQ("/tmp/source.yz(11): Error: Incompatible types: can not cast from type char to void\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);

  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::CHAR,
                               expressionValue,
                               PrimitiveTypes::CHAR,
                               0);
  EXPECT_EQ(result, expressionValue);

  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::CHAR,
                               expressionValue,
                               PrimitiveTypes::BYTE,
                               0);
  EXPECT_EQ(result, expressionValue);

  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::CHAR,
                               expressionValue,
                               PrimitiveTypes::BOOLEAN,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %0 = icmp ne i8 97, 0", mStringStream->str().c_str());
  mStringBuffer.clear();

  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::CHAR,
                               expressionValue,
                               PrimitiveTypes::INT,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = sext i8 97 to i32", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::CHAR,
                               expressionValue,
                               PrimitiveTypes::LONG,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv1 = sext i8 97 to i64", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::CHAR,
                               expressionValue,
                               PrimitiveTypes::FLOAT,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv2 = sitofp i8 97 to float", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::CHAR,
                               expressionValue,
                               PrimitiveTypes::DOUBLE,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv3 = sitofp i8 97 to double", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(AutoCastTest, castByteTest) {
  Value* result;
  Value* expressionValue = ConstantInt::get(Type::getInt8Ty(mLLVMContext), 5);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(AutoCast::maybeCast(mContext,
                                       PrimitiveTypes::BYTE,
                                       expressionValue,
                                       PrimitiveTypes::VOID,
                                       11));
  EXPECT_STREQ("/tmp/source.yz(11): Error: Incompatible types: can not cast from type byte to void\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::BYTE,
                               expressionValue,
                               PrimitiveTypes::CHAR,
                               0);
  EXPECT_EQ(result, expressionValue);
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::BYTE,
                               expressionValue,
                               PrimitiveTypes::BYTE,
                               0);
  EXPECT_EQ(result, expressionValue);
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::BYTE,
                               expressionValue,
                               PrimitiveTypes::BOOLEAN,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %0 = icmp ne i8 5, 0", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::BYTE,
                               expressionValue,
                               PrimitiveTypes::INT,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = sext i8 5 to i32", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::BYTE,
                               expressionValue,
                               PrimitiveTypes::LONG,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv1 = sext i8 5 to i64", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::BYTE,
                               expressionValue,
                               PrimitiveTypes::FLOAT,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv2 = sitofp i8 5 to float", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::BYTE,
                               expressionValue,
                               PrimitiveTypes::DOUBLE,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv3 = sitofp i8 5 to double", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(AutoCastTest, castIntTest) {
  Value* result;
  Value* expressionValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(AutoCast::maybeCast(mContext,
                                       PrimitiveTypes::INT,
                                       expressionValue,
                                       PrimitiveTypes::VOID,
                                       11));
  EXPECT_ANY_THROW(AutoCast::maybeCast(mContext,
                                       PrimitiveTypes::INT,
                                       expressionValue,
                                       PrimitiveTypes::CHAR,
                                       11));
  EXPECT_ANY_THROW(AutoCast::maybeCast(mContext,
                                       PrimitiveTypes::INT,
                                       expressionValue,
                                       PrimitiveTypes::BYTE,
                                       11));

  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::INT,
                               expressionValue,
                               PrimitiveTypes::INT,
                               0);
  EXPECT_EQ(result, expressionValue);
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::INT,
                               expressionValue,
                               PrimitiveTypes::LONG,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = sext i32 5 to i64", mStringStream->str().c_str());
  mStringBuffer.clear();
  buffer.clear();
  
  EXPECT_ANY_THROW(AutoCast::maybeCast(mContext,
                                       PrimitiveTypes::INT,
                                       expressionValue,
                                       PrimitiveTypes::FLOAT,
                                       11));
  EXPECT_STREQ("/tmp/source.yz(11): Error: Incompatible types: can not cast from type int to void\n"
               "/tmp/source.yz(11): Error: Incompatible types: need explicit cast from type int to char\n"
               "/tmp/source.yz(11): Error: Incompatible types: need explicit cast from type int to byte\n"
               "/tmp/source.yz(11): Error: Incompatible types: need explicit cast from type int to float\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);

  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::INT,
                               expressionValue,
                               PrimitiveTypes::BOOLEAN,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %0 = icmp ne i32 5, 0", mStringStream->str().c_str());
  mStringBuffer.clear();

  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::INT,
                               expressionValue,
                               PrimitiveTypes::DOUBLE,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv1 = sitofp i32 5 to double", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(AutoCastTest, castLongTest) {
  Value* result;
  Value* expressionValue = ConstantInt::get(Type::getInt64Ty(mLLVMContext), 5l);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(AutoCast::maybeCast(mContext,
                                       PrimitiveTypes::LONG,
                                       expressionValue,
                                       PrimitiveTypes::VOID,
                                       11));
  EXPECT_ANY_THROW(AutoCast::maybeCast(mContext,
                                       PrimitiveTypes::LONG,
                                       expressionValue,
                                       PrimitiveTypes::CHAR,
                                       11));
  EXPECT_ANY_THROW(AutoCast::maybeCast(mContext,
                                       PrimitiveTypes::LONG,
                                       expressionValue,
                                       PrimitiveTypes::BYTE,
                                       11));
  EXPECT_ANY_THROW(AutoCast::maybeCast(mContext,
                                       PrimitiveTypes::LONG,
                                       expressionValue,
                                       PrimitiveTypes::INT,
                                       11));
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::LONG,
                               expressionValue,
                               PrimitiveTypes::LONG,
                               0);
  EXPECT_EQ(result, expressionValue);
  
  EXPECT_ANY_THROW(AutoCast::maybeCast(mContext,
                                       PrimitiveTypes::LONG,
                                       expressionValue,
                                       PrimitiveTypes::FLOAT,
                                       11));
  EXPECT_ANY_THROW(AutoCast::maybeCast(mContext,
                                       PrimitiveTypes::LONG,
                                       expressionValue,
                                       PrimitiveTypes::DOUBLE,
                                       11));
  EXPECT_STREQ("/tmp/source.yz(11): Error: Incompatible types: can not cast from type long to void\n"
               "/tmp/source.yz(11): Error: Incompatible types: need explicit cast from type long to char\n"
               "/tmp/source.yz(11): Error: Incompatible types: need explicit cast from type long to byte\n"
               "/tmp/source.yz(11): Error: Incompatible types: need explicit cast from type long to int\n"
               "/tmp/source.yz(11): Error: Incompatible types: need explicit cast from type long to float\n"
               "/tmp/source.yz(11): Error: Incompatible types: need explicit cast from type long to double\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);

  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::LONG,
                               expressionValue,
                               PrimitiveTypes::BOOLEAN,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %0 = icmp ne i64 5, 0", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(AutoCastTest, castFloatTest) {
  Value* result;
  Value* expressionValue = ConstantFP::get(Type::getFloatTy(mLLVMContext), 2.5);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(AutoCast::maybeCast(mContext,
                                       PrimitiveTypes::FLOAT,
                                       expressionValue,
                                       PrimitiveTypes::VOID,
                                       11));
  EXPECT_ANY_THROW(AutoCast::maybeCast(mContext,
                                       PrimitiveTypes::FLOAT,
                                       expressionValue,
                                       PrimitiveTypes::CHAR,
                                       11));
  EXPECT_ANY_THROW(AutoCast::maybeCast(mContext,
                                       PrimitiveTypes::FLOAT,
                                       expressionValue,
                                       PrimitiveTypes::BYTE,
                                       11));
  EXPECT_ANY_THROW(AutoCast::maybeCast(mContext,
                                       PrimitiveTypes::FLOAT,
                                       expressionValue,
                                       PrimitiveTypes::INT,
                                       11));
  EXPECT_ANY_THROW(AutoCast::maybeCast(mContext,
                                       PrimitiveTypes::FLOAT,
                                       expressionValue,
                                       PrimitiveTypes::LONG,
                                       11));
  EXPECT_STREQ("/tmp/source.yz(11): Error: Incompatible types: can not cast from type float to void\n"
               "/tmp/source.yz(11): Error: Incompatible types: need explicit cast from type float to char\n"
               "/tmp/source.yz(11): Error: Incompatible types: need explicit cast from type float to byte\n"
               "/tmp/source.yz(11): Error: Incompatible types: need explicit cast from type float to int\n"
               "/tmp/source.yz(11): Error: Incompatible types: need explicit cast from type float to long\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);

  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::FLOAT,
                               expressionValue,
                               PrimitiveTypes::BOOLEAN,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %0 = fcmp one float 2.500000e+00, 0.000000e+00", mStringStream->str().c_str());
  mStringBuffer.clear();

  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::FLOAT,
                               expressionValue,
                               PrimitiveTypes::FLOAT,
                               0);
  EXPECT_EQ(result, expressionValue);
  
  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::FLOAT,
                               expressionValue,
                               PrimitiveTypes::DOUBLE,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = fpext float 2.500000e+00 to double", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(AutoCastTest, castDoubleTest) {
  Value* result;
  Value* expressionValue = ConstantFP::get(Type::getDoubleTy(mLLVMContext), 2.5);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(AutoCast::maybeCast(mContext,
                                       PrimitiveTypes::DOUBLE,
                                       expressionValue,
                                       PrimitiveTypes::VOID,
                                       11));
  EXPECT_ANY_THROW(AutoCast::maybeCast(mContext,
                                       PrimitiveTypes::DOUBLE,
                                       expressionValue,
                                       PrimitiveTypes::CHAR,
                                       11));
  EXPECT_ANY_THROW(AutoCast::maybeCast(mContext,
                                       PrimitiveTypes::DOUBLE,
                                       expressionValue,
                                       PrimitiveTypes::BYTE,
                                       11));
  EXPECT_ANY_THROW(AutoCast::maybeCast(mContext,
                                       PrimitiveTypes::DOUBLE,
                                       expressionValue,
                                       PrimitiveTypes::INT,
                                       11));
  EXPECT_ANY_THROW(AutoCast::maybeCast(mContext,
                                       PrimitiveTypes::DOUBLE,
                                       expressionValue,
                                       PrimitiveTypes::LONG,
                                       11));
  EXPECT_ANY_THROW(AutoCast::maybeCast(mContext,
                                       PrimitiveTypes::DOUBLE,
                                       expressionValue,
                                       PrimitiveTypes::FLOAT,
                                       11));
  EXPECT_STREQ("/tmp/source.yz(11): Error: Incompatible types: can not cast from type double to void\n"
               "/tmp/source.yz(11): Error: Incompatible types: need explicit cast from type double to char\n"
               "/tmp/source.yz(11): Error: Incompatible types: need explicit cast from type double to byte\n"
               "/tmp/source.yz(11): Error: Incompatible types: need explicit cast from type double to int\n"
               "/tmp/source.yz(11): Error: Incompatible types: need explicit cast from type double to long\n"
               "/tmp/source.yz(11): Error: Incompatible types: need explicit cast from type double to float\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);

  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::DOUBLE,
                               expressionValue,
                               PrimitiveTypes::BOOLEAN,
                               0);
  *mStringStream << *result;
  EXPECT_STREQ("  %0 = fcmp one double 2.500000e+00, 0.000000e+00", mStringStream->str().c_str());
  mStringBuffer.clear();

  result = AutoCast::maybeCast(mContext,
                               PrimitiveTypes::DOUBLE,
                               expressionValue,
                               PrimitiveTypes::DOUBLE,
                               0);
  EXPECT_EQ(result, expressionValue);
}
