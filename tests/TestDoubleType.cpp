//
//  TestDoubleType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link DoubleType}
//

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "DoubleType.hpp"
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

struct DoubleTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mEntryBlock;
  BasicBlock* mDeclareBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  DoubleType mDoubleType;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;
  
public:
  
  DoubleTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
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
    
    IField* field = new ReceivedField(&mDoubleType, "mField", 0);
    ON_CALL(mConcreteObjectType, findField(_)).WillByDefault(Return(field));
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~DoubleTypeTest() {
    delete mStringStream;
  }
};

TEST_F(DoubleTypeTest, doubleTypeTest) {
  EXPECT_EQ(Type::getDoubleTy(mLLVMContext), mDoubleType.getLLVMType(mContext));
  EXPECT_STREQ("double", mDoubleType.getTypeName().c_str());
  EXPECT_EQ("%e", mDoubleType.getFormat());
}

TEST_F(DoubleTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mDoubleType.canAutoCastTo(mContext, PrimitiveTypes::VOID));
  EXPECT_FALSE(mDoubleType.canAutoCastTo(mContext, PrimitiveTypes::STRING));
  EXPECT_TRUE(mDoubleType.canAutoCastTo(mContext, PrimitiveTypes::BOOLEAN));
  EXPECT_FALSE(mDoubleType.canAutoCastTo(mContext, PrimitiveTypes::CHAR));
  EXPECT_FALSE(mDoubleType.canAutoCastTo(mContext, PrimitiveTypes::BYTE));
  EXPECT_FALSE(mDoubleType.canAutoCastTo(mContext, PrimitiveTypes::INT));
  EXPECT_FALSE(mDoubleType.canAutoCastTo(mContext, PrimitiveTypes::LONG));
  EXPECT_FALSE(mDoubleType.canAutoCastTo(mContext, PrimitiveTypes::FLOAT));
  EXPECT_TRUE(mDoubleType.canAutoCastTo(mContext, PrimitiveTypes::DOUBLE));
}

TEST_F(DoubleTypeTest, canCastTest) {
  EXPECT_FALSE(mDoubleType.canCastTo(mContext, PrimitiveTypes::VOID));
  EXPECT_FALSE(mDoubleType.canCastTo(mContext, PrimitiveTypes::STRING));
  EXPECT_TRUE(mDoubleType.canCastTo(mContext, PrimitiveTypes::BOOLEAN));
  EXPECT_TRUE(mDoubleType.canCastTo(mContext, PrimitiveTypes::CHAR));
  EXPECT_TRUE(mDoubleType.canCastTo(mContext, PrimitiveTypes::BYTE));
  EXPECT_TRUE(mDoubleType.canCastTo(mContext, PrimitiveTypes::INT));
  EXPECT_TRUE(mDoubleType.canCastTo(mContext, PrimitiveTypes::LONG));
  EXPECT_TRUE(mDoubleType.canCastTo(mContext, PrimitiveTypes::FLOAT));
  EXPECT_TRUE(mDoubleType.canCastTo(mContext, PrimitiveTypes::DOUBLE));
}

TEST_F(DoubleTypeTest, castToTest) {
  Mock::AllowLeak(&mConcreteObjectType);

  Value* result;
  Value* expressionValue = ConstantFP::get(Type::getDoubleTy(mLLVMContext), 2.5);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mDoubleType.castTo(mContext, expressionValue, PrimitiveTypes::VOID, 5));
  EXPECT_STREQ("/tmp/source.yz(5): Error: Incompatible types: can not cast from type double to void\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);

  result = mDoubleType.castTo(mContext, expressionValue, PrimitiveTypes::BOOLEAN, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %0 = fcmp one double 2.500000e+00, 0.000000e+00", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mDoubleType.castTo(mContext, expressionValue, PrimitiveTypes::CHAR, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = fptosi double 2.500000e+00 to i8", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mDoubleType.castTo(mContext, expressionValue, PrimitiveTypes::BYTE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv1 = fptosi double 2.500000e+00 to i8", mStringStream->str().c_str());
  mStringBuffer.clear();

  result = mDoubleType.castTo(mContext, expressionValue, PrimitiveTypes::INT, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv2 = fptosi double 2.500000e+00 to i32", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mDoubleType.castTo(mContext, expressionValue, PrimitiveTypes::LONG, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv3 = fptosi double 2.500000e+00 to i64", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mDoubleType.castTo(mContext, expressionValue, PrimitiveTypes::FLOAT, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv4 = fptrunc double 2.500000e+00 to float", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mDoubleType.castTo(mContext, expressionValue, PrimitiveTypes::DOUBLE, 0);
  EXPECT_EQ(result, expressionValue);
}

TEST_F(DoubleTypeTest, isTypeKindTest) {
  EXPECT_TRUE(mDoubleType.isPrimitive());
  EXPECT_FALSE(mDoubleType.isOwner());
  EXPECT_FALSE(mDoubleType.isReference());
  EXPECT_FALSE(mDoubleType.isArray());
  EXPECT_FALSE(mDoubleType.isFunction());
  EXPECT_FALSE(mDoubleType.isPackage());
  EXPECT_FALSE(mDoubleType.isNative());
  EXPECT_FALSE(mDoubleType.isPointer());
  EXPECT_FALSE(mDoubleType.isPointer());
}

TEST_F(DoubleTypeTest, isObjectTest) {
  EXPECT_FALSE(mDoubleType.isController());
  EXPECT_FALSE(mDoubleType.isInterface());
  EXPECT_FALSE(mDoubleType.isModel());
  EXPECT_FALSE(mDoubleType.isNode());
}

TEST_F(DoubleTypeTest, createLocalVariableTest) {
  mDoubleType.createLocalVariable(mContext, "temp", 0);
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mDeclareBlock;
  *mStringStream << *mEntryBlock;

  string expected =
  "\ndeclare:"
  "\n  %0 = alloca double"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  store double 0.000000e+00, double* %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(DoubleTypeTest, createFieldVariableTest) {
  mDoubleType.createFieldVariable(mContext, "mField", &mConcreteObjectType, 0);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(DoubleTypeTest, createParameterVariableTest) {
  mDoubleType.createParameterVariable(mContext, "var", NULL, 0);
  IVariable* variable = mContext.getScopes().getVariable("var");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(DoubleTypeTest, injectDeathTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  InjectionArgumentList arguments;
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mDoubleType.inject(mContext, arguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: type double is not injectable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, doubleCastToBooleanRunTest) {
  runFile("tests/samples/test_double_cast_to_boolean.yz", 9);
}

TEST_F(TestFileRunner, signedCastDoubleToIntRunTest) {
  runFile("tests/samples/test_signed_cast_double_to_int.yz", -7);
}
