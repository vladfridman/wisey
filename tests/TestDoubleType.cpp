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
#include "wisey/DoubleType.hpp"
#include "wisey/Field.hpp"
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

struct DoubleTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  DoubleType mDoubleType;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;
  
public:
  
  DoubleTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
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
    Field* field = new Field(FIXED_FIELD, &mDoubleType, NULL, "mField", injectionArgumentList);
    ON_CALL(mConcreteObjectType, findField(_)).WillByDefault(Return(field));
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~DoubleTypeTest() {
    delete mStringStream;
  }
};

TEST_F(DoubleTypeTest, doubleTypeTest) {
  EXPECT_EQ(mDoubleType.getLLVMType(mContext), Type::getDoubleTy(mLLVMContext));
  EXPECT_STREQ(mDoubleType.getTypeName().c_str(), "double");
  EXPECT_EQ(mDoubleType.getTypeKind(), PRIMITIVE_TYPE);
  EXPECT_EQ(mDoubleType.getFormat(), "%e");
}

TEST_F(DoubleTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mDoubleType.canAutoCastTo(mContext, PrimitiveTypes::VOID_TYPE));
  EXPECT_FALSE(mDoubleType.canAutoCastTo(mContext, PrimitiveTypes::STRING_TYPE));
  EXPECT_FALSE(mDoubleType.canAutoCastTo(mContext, PrimitiveTypes::BOOLEAN_TYPE));
  EXPECT_FALSE(mDoubleType.canAutoCastTo(mContext, PrimitiveTypes::CHAR_TYPE));
  EXPECT_FALSE(mDoubleType.canAutoCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mDoubleType.canAutoCastTo(mContext, PrimitiveTypes::LONG_TYPE));
  EXPECT_FALSE(mDoubleType.canAutoCastTo(mContext, PrimitiveTypes::FLOAT_TYPE));
  EXPECT_TRUE(mDoubleType.canAutoCastTo(mContext, PrimitiveTypes::DOUBLE_TYPE));
}

TEST_F(DoubleTypeTest, canCastTest) {
  EXPECT_FALSE(mDoubleType.canCastTo(mContext, PrimitiveTypes::VOID_TYPE));
  EXPECT_FALSE(mDoubleType.canCastTo(mContext, PrimitiveTypes::STRING_TYPE));
  EXPECT_TRUE(mDoubleType.canCastTo(mContext, PrimitiveTypes::BOOLEAN_TYPE));
  EXPECT_TRUE(mDoubleType.canCastTo(mContext, PrimitiveTypes::CHAR_TYPE));
  EXPECT_TRUE(mDoubleType.canCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(mDoubleType.canCastTo(mContext, PrimitiveTypes::LONG_TYPE));
  EXPECT_TRUE(mDoubleType.canCastTo(mContext, PrimitiveTypes::FLOAT_TYPE));
  EXPECT_TRUE(mDoubleType.canCastTo(mContext, PrimitiveTypes::DOUBLE_TYPE));
}

TEST_F(DoubleTypeTest, castToTest) {
  Mock::AllowLeak(&mConcreteObjectType);

  Value* result;
  Value* expressionValue = ConstantFP::get(Type::getDoubleTy(mLLVMContext), 2.5);
  
  EXPECT_EXIT(mDoubleType.castTo(mContext, expressionValue, PrimitiveTypes::VOID_TYPE, 0),
              ::testing::ExitedWithCode(1),
              "Error: Incompatible types: can not cast from type 'double' to 'void'");
  
  result = mDoubleType.castTo(mContext, expressionValue, PrimitiveTypes::BOOLEAN_TYPE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = fptosi double 2.500000e+00 to i1", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mDoubleType.castTo(mContext, expressionValue, PrimitiveTypes::CHAR_TYPE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv1 = fptosi double 2.500000e+00 to i16", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mDoubleType.castTo(mContext, expressionValue, PrimitiveTypes::INT_TYPE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv2 = fptosi double 2.500000e+00 to i32", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mDoubleType.castTo(mContext, expressionValue, PrimitiveTypes::LONG_TYPE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv3 = fptosi double 2.500000e+00 to i64", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mDoubleType.castTo(mContext, expressionValue, PrimitiveTypes::FLOAT_TYPE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv4 = fptrunc double 2.500000e+00 to float", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mDoubleType.castTo(mContext, expressionValue, PrimitiveTypes::DOUBLE_TYPE, 0);
  EXPECT_EQ(result, expressionValue);
}

TEST_F(DoubleTypeTest, isOwnerTest) {
  EXPECT_FALSE(mDoubleType.isOwner());
}

TEST_F(DoubleTypeTest, isReferenceTest) {
  EXPECT_FALSE(mDoubleType.isReference());
}

TEST_F(DoubleTypeTest, createLocalVariableTest) {
  mDoubleType.createLocalVariable(mContext, "temp");
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = alloca double"
  "\n  store double 0.000000e+00, double* %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(DoubleTypeTest, createFieldVariableTest) {
  mDoubleType.createFieldVariable(mContext, "mField", &mConcreteObjectType);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}
