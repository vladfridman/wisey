//
//  TestBooleanType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link BooleanType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "wisey/BooleanType.hpp"
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

struct BooleanTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  BooleanType mBoleanType;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;

public:
  
  BooleanTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
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
    Field* field = new Field(FIXED_FIELD, &mBoleanType, NULL, "mField", injectionArgumentList);
    ON_CALL(mConcreteObjectType, findField(_)).WillByDefault(Return(field));

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~BooleanTypeTest() {
    delete mStringStream;
  }
};

TEST_F(BooleanTypeTest, booleanTypeTest) {
  EXPECT_EQ(mBoleanType.getLLVMType(mContext), (llvm::Type*) Type::getInt1Ty(mLLVMContext));
  EXPECT_STREQ(mBoleanType.getTypeName().c_str(), "boolean");
  EXPECT_EQ(mBoleanType.getTypeKind(), PRIMITIVE_TYPE);
  EXPECT_EQ(mBoleanType.getFormat(), "%d");
}

TEST_F(BooleanTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mBoleanType.canAutoCastTo(mContext, PrimitiveTypes::VOID_TYPE));
  EXPECT_FALSE(mBoleanType.canAutoCastTo(mContext, PrimitiveTypes::STRING_TYPE));
  EXPECT_TRUE(mBoleanType.canAutoCastTo(mContext, PrimitiveTypes::BOOLEAN_TYPE));
  EXPECT_TRUE(mBoleanType.canAutoCastTo(mContext, PrimitiveTypes::CHAR_TYPE));
  EXPECT_TRUE(mBoleanType.canAutoCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(mBoleanType.canAutoCastTo(mContext, PrimitiveTypes::LONG_TYPE));
  EXPECT_TRUE(mBoleanType.canAutoCastTo(mContext, PrimitiveTypes::FLOAT_TYPE));
  EXPECT_TRUE(mBoleanType.canAutoCastTo(mContext, PrimitiveTypes::DOUBLE_TYPE));
}

TEST_F(BooleanTypeTest, canCastTest) {
  EXPECT_FALSE(mBoleanType.canCastTo(mContext, PrimitiveTypes::VOID_TYPE));
  EXPECT_FALSE(mBoleanType.canCastTo(mContext, PrimitiveTypes::STRING_TYPE));
  EXPECT_TRUE(mBoleanType.canCastTo(mContext, PrimitiveTypes::BOOLEAN_TYPE));
  EXPECT_TRUE(mBoleanType.canCastTo(mContext, PrimitiveTypes::CHAR_TYPE));
  EXPECT_TRUE(mBoleanType.canCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(mBoleanType.canCastTo(mContext, PrimitiveTypes::LONG_TYPE));
  EXPECT_TRUE(mBoleanType.canCastTo(mContext, PrimitiveTypes::FLOAT_TYPE));
  EXPECT_TRUE(mBoleanType.canCastTo(mContext, PrimitiveTypes::DOUBLE_TYPE));
}

TEST_F(BooleanTypeTest, castToTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  Value* result;
  Value* expressionValue = ConstantInt::get(Type::getInt1Ty(mLLVMContext), 1);
  
  EXPECT_EXIT(mBoleanType.castTo(mContext, expressionValue, PrimitiveTypes::VOID_TYPE, 0),
              ::testing::ExitedWithCode(1),
              "Error: Incompatible types: can not cast from type 'boolean' to 'void'");
  
  result = mBoleanType.castTo(mContext, expressionValue, PrimitiveTypes::BOOLEAN_TYPE, 0);
  EXPECT_EQ(result, expressionValue);
  
  result = mBoleanType.castTo(mContext, expressionValue, PrimitiveTypes::CHAR_TYPE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = zext i1 true to i16", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mBoleanType.castTo(mContext, expressionValue, PrimitiveTypes::INT_TYPE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv1 = zext i1 true to i32", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mBoleanType.castTo(mContext, expressionValue, PrimitiveTypes::LONG_TYPE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv2 = zext i1 true to i64", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mBoleanType.castTo(mContext, expressionValue, PrimitiveTypes::FLOAT_TYPE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv3 = sitofp i1 true to float", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mBoleanType.castTo(mContext, expressionValue, PrimitiveTypes::DOUBLE_TYPE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv4 = sitofp i1 true to double", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(BooleanTypeTest, isOwnerTest) {
  EXPECT_FALSE(mBoleanType.isOwner());
}

TEST_F(BooleanTypeTest, isReferenceTest) {
  EXPECT_FALSE(mBoleanType.isReference());
}

TEST_F(BooleanTypeTest, isArrayTest) {
  EXPECT_FALSE(mBoleanType.isArray());
}

TEST_F(BooleanTypeTest, isObjectTest) {
  EXPECT_FALSE(mBoleanType.isController());
  EXPECT_FALSE(mBoleanType.isInterface());
  EXPECT_FALSE(mBoleanType.isModel());
  EXPECT_FALSE(mBoleanType.isNode());
}

TEST_F(BooleanTypeTest, createLocalVariableTest) {
  mBoleanType.createLocalVariable(mContext, "temp");
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = alloca i1"
  "\n  store i1 false, i1* %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(BooleanTypeTest, createFieldVariableTest) {
  mBoleanType.createFieldVariable(mContext, "mField", &mConcreteObjectType);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(BooleanTypeTest, createParameterVariableTest) {
  mBoleanType.createParameterVariable(mContext, "var", NULL);
  IVariable* variable = mContext.getScopes().getVariable("var");
  
  EXPECT_NE(variable, nullptr);
}
