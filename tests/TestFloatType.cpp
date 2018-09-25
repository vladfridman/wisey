//
//  TestFloatType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link FloatType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "FloatType.hpp"
#include "IRGenerationContext.hpp"
#include "PrimitiveTypes.hpp"
#include "ReceivedField.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct FloatTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mEntryBlock;
  BasicBlock* mDeclareBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  FloatType mFloatType;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;

public:
  
  FloatTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mDeclareBlock = BasicBlock::Create(mLLVMContext, "declare", function);
    mEntryBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setDeclarationsBlock(mDeclareBlock);
    mContext.setBasicBlock(mEntryBlock);
    mContext.getScopes().pushScope();

    IField* field = new ReceivedField(&mFloatType, "mField", 0);
    ON_CALL(mConcreteObjectType, findField(_)).WillByDefault(Return(field));
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~FloatTypeTest() {
    delete mStringStream;
  }
};

TEST_F(FloatTypeTest, floatTypeTest) {
  EXPECT_EQ(Type::getFloatTy(mLLVMContext), mFloatType.getLLVMType(mContext));
  EXPECT_STREQ("float", mFloatType.getTypeName().c_str());
  EXPECT_EQ("%f", mFloatType.getFormat());
}

TEST_F(FloatTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mFloatType.canAutoCastTo(mContext, PrimitiveTypes::VOID));
  EXPECT_FALSE(mFloatType.canAutoCastTo(mContext, PrimitiveTypes::STRING));
  EXPECT_TRUE(mFloatType.canAutoCastTo(mContext, PrimitiveTypes::BOOLEAN));
  EXPECT_FALSE(mFloatType.canAutoCastTo(mContext, PrimitiveTypes::CHAR));
  EXPECT_FALSE(mFloatType.canAutoCastTo(mContext, PrimitiveTypes::BYTE));
  EXPECT_FALSE(mFloatType.canAutoCastTo(mContext, PrimitiveTypes::INT));
  EXPECT_FALSE(mFloatType.canAutoCastTo(mContext, PrimitiveTypes::LONG));
  EXPECT_TRUE(mFloatType.canAutoCastTo(mContext, PrimitiveTypes::FLOAT));
  EXPECT_TRUE(mFloatType.canAutoCastTo(mContext, PrimitiveTypes::DOUBLE));
}

TEST_F(FloatTypeTest, canCastTest) {
  EXPECT_FALSE(mFloatType.canCastTo(mContext, PrimitiveTypes::VOID));
  EXPECT_FALSE(mFloatType.canCastTo(mContext, PrimitiveTypes::STRING));
  EXPECT_TRUE(mFloatType.canCastTo(mContext, PrimitiveTypes::BOOLEAN));
  EXPECT_TRUE(mFloatType.canCastTo(mContext, PrimitiveTypes::CHAR));
  EXPECT_TRUE(mFloatType.canCastTo(mContext, PrimitiveTypes::BYTE));
  EXPECT_TRUE(mFloatType.canCastTo(mContext, PrimitiveTypes::INT));
  EXPECT_TRUE(mFloatType.canCastTo(mContext, PrimitiveTypes::LONG));
  EXPECT_TRUE(mFloatType.canCastTo(mContext, PrimitiveTypes::FLOAT));
  EXPECT_TRUE(mFloatType.canCastTo(mContext, PrimitiveTypes::DOUBLE));
}

TEST_F(FloatTypeTest, castToTest) {
  Mock::AllowLeak(&mConcreteObjectType);

  Value* result;
  Value* expressionValue = ConstantFP::get(Type::getFloatTy(mLLVMContext), 2.5);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mFloatType.castTo(mContext, expressionValue, PrimitiveTypes::VOID, 5));
  EXPECT_STREQ("/tmp/source.yz(5): Error: Incompatible types: can not cast from type float to void\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);

  result = mFloatType.castTo(mContext, expressionValue, PrimitiveTypes::BOOLEAN, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %0 = fcmp one float 2.500000e+00, 0.000000e+00", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mFloatType.castTo(mContext, expressionValue, PrimitiveTypes::CHAR, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = fptosi float 2.500000e+00 to i8", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mFloatType.castTo(mContext, expressionValue, PrimitiveTypes::BYTE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv1 = fptosi float 2.500000e+00 to i8", mStringStream->str().c_str());
  mStringBuffer.clear();

  result = mFloatType.castTo(mContext, expressionValue, PrimitiveTypes::INT, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv2 = fptosi float 2.500000e+00 to i32", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mFloatType.castTo(mContext, expressionValue, PrimitiveTypes::LONG, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv3 = fptosi float 2.500000e+00 to i64", mStringStream->str().c_str());
  mStringBuffer.clear();

  result = mFloatType.castTo(mContext, expressionValue, PrimitiveTypes::FLOAT, 0);
  EXPECT_EQ(result, expressionValue);
  
  result = mFloatType.castTo(mContext, expressionValue, PrimitiveTypes::DOUBLE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv4 = fpext float 2.500000e+00 to double", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(FloatTypeTest, isTypeKindTest) {
  EXPECT_TRUE(mFloatType.isPrimitive());
  EXPECT_FALSE(mFloatType.isOwner());
  EXPECT_FALSE(mFloatType.isReference());
  EXPECT_FALSE(mFloatType.isArray());
  EXPECT_FALSE(mFloatType.isFunction());
  EXPECT_FALSE(mFloatType.isPackage());
  EXPECT_FALSE(mFloatType.isNative());
  EXPECT_FALSE(mFloatType.isPointer());
  EXPECT_FALSE(mFloatType.isPointer());
}

TEST_F(FloatTypeTest, isObjectTest) {
  EXPECT_FALSE(mFloatType.isController());
  EXPECT_FALSE(mFloatType.isInterface());
  EXPECT_FALSE(mFloatType.isModel());
  EXPECT_FALSE(mFloatType.isNode());
}

TEST_F(FloatTypeTest, createLocalVariableTest) {
  mFloatType.createLocalVariable(mContext, "temp", 0);
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mDeclareBlock;
  *mStringStream << *mEntryBlock;

  string expected =
  "\ndeclare:"
  "\n  %0 = alloca float"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  store float 0.000000e+00, float* %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(FloatTypeTest, createFieldVariableTest) {
  mFloatType.createFieldVariable(mContext, "mField", &mConcreteObjectType, 0);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(FloatTypeTest, createParameterVariableTest) {
  mFloatType.createParameterVariable(mContext, "var", NULL, 0);
  IVariable* variable = mContext.getScopes().getVariable("var");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(FloatTypeTest, injectDeathTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  InjectionArgumentList arguments;
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mFloatType.inject(mContext, arguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: type float is not injectable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, floatCastToBooleanRunTest) {
  runFile("tests/samples/test_float_cast_to_boolean.yz", 7);
}

TEST_F(TestFileRunner, signedCastFloatToIntRunTest) {
  runFile("tests/samples/test_signed_cast_float_to_int.yz", -5);
}
