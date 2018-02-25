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
#include "wisey/Field.hpp"
#include "wisey/FloatType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

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
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  FloatType mFloatType;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;

public:
  
  FloatTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();

    InjectionArgumentList injectionArgumentList;
    Field* field = new Field(FIXED_FIELD, &mFloatType, NULL, "mField", injectionArgumentList);
    ON_CALL(mConcreteObjectType, findField(_)).WillByDefault(Return(field));
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~FloatTypeTest() {
    delete mStringStream;
  }
};

TEST_F(FloatTypeTest, floatTypeTest) {
  EXPECT_EQ(mFloatType.getLLVMType(mContext), Type::getFloatTy(mLLVMContext));
  EXPECT_STREQ(mFloatType.getTypeName().c_str(), "float");
  EXPECT_EQ(mFloatType.getTypeKind(), PRIMITIVE_TYPE);
  EXPECT_EQ(mFloatType.getFormat(), "%f");
}

TEST_F(FloatTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mFloatType.canAutoCastTo(mContext, PrimitiveTypes::VOID_TYPE));
  EXPECT_FALSE(mFloatType.canAutoCastTo(mContext, PrimitiveTypes::STRING_TYPE));
  EXPECT_FALSE(mFloatType.canAutoCastTo(mContext, PrimitiveTypes::BOOLEAN_TYPE));
  EXPECT_FALSE(mFloatType.canAutoCastTo(mContext, PrimitiveTypes::CHAR_TYPE));
  EXPECT_FALSE(mFloatType.canAutoCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mFloatType.canAutoCastTo(mContext, PrimitiveTypes::LONG_TYPE));
  EXPECT_TRUE(mFloatType.canAutoCastTo(mContext, PrimitiveTypes::FLOAT_TYPE));
  EXPECT_TRUE(mFloatType.canAutoCastTo(mContext, PrimitiveTypes::DOUBLE_TYPE));
}

TEST_F(FloatTypeTest, canCastTest) {
  EXPECT_FALSE(mFloatType.canCastTo(mContext, PrimitiveTypes::VOID_TYPE));
  EXPECT_FALSE(mFloatType.canCastTo(mContext, PrimitiveTypes::STRING_TYPE));
  EXPECT_TRUE(mFloatType.canCastTo(mContext, PrimitiveTypes::BOOLEAN_TYPE));
  EXPECT_TRUE(mFloatType.canCastTo(mContext, PrimitiveTypes::CHAR_TYPE));
  EXPECT_TRUE(mFloatType.canCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(mFloatType.canCastTo(mContext, PrimitiveTypes::LONG_TYPE));
  EXPECT_TRUE(mFloatType.canCastTo(mContext, PrimitiveTypes::FLOAT_TYPE));
  EXPECT_TRUE(mFloatType.canCastTo(mContext, PrimitiveTypes::DOUBLE_TYPE));
}

TEST_F(FloatTypeTest, castToTest) {
  Mock::AllowLeak(&mConcreteObjectType);

  Value* result;
  Value* expressionValue = ConstantFP::get(Type::getFloatTy(mLLVMContext), 2.5);
  
  EXPECT_EXIT(mFloatType.castTo(mContext, expressionValue, PrimitiveTypes::VOID_TYPE, 0),
              ::testing::ExitedWithCode(1),
              "Error: Incompatible types: can not cast from type 'float' to 'void'");
  
  result = mFloatType.castTo(mContext, expressionValue, PrimitiveTypes::BOOLEAN_TYPE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = fptosi float 2.500000e+00 to i1", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mFloatType.castTo(mContext, expressionValue, PrimitiveTypes::CHAR_TYPE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv1 = fptosi float 2.500000e+00 to i16", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mFloatType.castTo(mContext, expressionValue, PrimitiveTypes::INT_TYPE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv2 = fptosi float 2.500000e+00 to i32", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mFloatType.castTo(mContext, expressionValue, PrimitiveTypes::LONG_TYPE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv3 = fptosi float 2.500000e+00 to i64", mStringStream->str().c_str());
  mStringBuffer.clear();

  result = mFloatType.castTo(mContext, expressionValue, PrimitiveTypes::FLOAT_TYPE, 0);
  EXPECT_EQ(result, expressionValue);
  
  result = mFloatType.castTo(mContext, expressionValue, PrimitiveTypes::DOUBLE_TYPE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv4 = fpext float 2.500000e+00 to double", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(FloatTypeTest, isPrimitiveTest) {
  EXPECT_TRUE(mFloatType.isPrimitive());
}

TEST_F(FloatTypeTest, isOwnerTest) {
  EXPECT_FALSE(mFloatType.isOwner());
}

TEST_F(FloatTypeTest, isReferenceTest) {
  EXPECT_FALSE(mFloatType.isReference());
}

TEST_F(FloatTypeTest, isArrayTest) {
  EXPECT_FALSE(mFloatType.isArray());
}

TEST_F(FloatTypeTest, isObjectTest) {
  EXPECT_FALSE(mFloatType.isController());
  EXPECT_FALSE(mFloatType.isInterface());
  EXPECT_FALSE(mFloatType.isModel());
  EXPECT_FALSE(mFloatType.isNode());
}

TEST_F(FloatTypeTest, createLocalVariableTest) {
  mFloatType.createLocalVariable(mContext, "temp");
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = alloca float"
  "\n  store float 0.000000e+00, float* %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(FloatTypeTest, createFieldVariableTest) {
  mFloatType.createFieldVariable(mContext, "mField", &mConcreteObjectType);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(FloatTypeTest, createParameterVariableTest) {
  mFloatType.createParameterVariable(mContext, "var", NULL);
  IVariable* variable = mContext.getScopes().getVariable("var");
  
  EXPECT_NE(variable, nullptr);
}
