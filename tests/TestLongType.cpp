//
//  TestLongType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LongType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "wisey/Field.hpp"
#include "wisey/LongType.hpp"
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

struct LongTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  LongType mLongType;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;

public:
  
  LongTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
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
    Field* field = new Field(FIXED_FIELD, &mLongType, NULL, "mField", injectionArgumentList);
    ON_CALL(mConcreteObjectType, findField(_)).WillByDefault(Return(field));
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~LongTypeTest() {
    delete mStringStream;
  }
};

TEST_F(LongTypeTest, longTypeTest) {
  EXPECT_EQ(mLongType.getLLVMType(mContext), (llvm::Type*) Type::getInt64Ty(mLLVMContext));
  EXPECT_STREQ(mLongType.getTypeName().c_str(), "long");
  EXPECT_EQ(mLongType.getTypeKind(), PRIMITIVE_TYPE);
  EXPECT_EQ(mLongType.getFormat(), "%d");
}

TEST_F(LongTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mLongType.canAutoCastTo(mContext, PrimitiveTypes::VOID_TYPE));
  EXPECT_FALSE(mLongType.canAutoCastTo(mContext, PrimitiveTypes::STRING_TYPE));
  EXPECT_FALSE(mLongType.canAutoCastTo(mContext, PrimitiveTypes::BOOLEAN_TYPE));
  EXPECT_FALSE(mLongType.canAutoCastTo(mContext, PrimitiveTypes::CHAR_TYPE));
  EXPECT_FALSE(mLongType.canAutoCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(mLongType.canAutoCastTo(mContext, PrimitiveTypes::LONG_TYPE));
  EXPECT_FALSE(mLongType.canAutoCastTo(mContext, PrimitiveTypes::FLOAT_TYPE));
  EXPECT_FALSE(mLongType.canAutoCastTo(mContext, PrimitiveTypes::DOUBLE_TYPE));
}

TEST_F(LongTypeTest, canCastTest) {
  EXPECT_FALSE(mLongType.canCastTo(mContext, PrimitiveTypes::VOID_TYPE));
  EXPECT_FALSE(mLongType.canCastTo(mContext, PrimitiveTypes::STRING_TYPE));
  EXPECT_TRUE(mLongType.canCastTo(mContext, PrimitiveTypes::BOOLEAN_TYPE));
  EXPECT_TRUE(mLongType.canCastTo(mContext, PrimitiveTypes::CHAR_TYPE));
  EXPECT_TRUE(mLongType.canCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(mLongType.canCastTo(mContext, PrimitiveTypes::LONG_TYPE));
  EXPECT_TRUE(mLongType.canCastTo(mContext, PrimitiveTypes::FLOAT_TYPE));
  EXPECT_TRUE(mLongType.canCastTo(mContext, PrimitiveTypes::DOUBLE_TYPE));
}

TEST_F(LongTypeTest, castToTest) {
  Mock::AllowLeak(&mConcreteObjectType);

  Value* result;
  Value* expressionValue = ConstantInt::get(Type::getInt64Ty(mLLVMContext), 5l);
  
  EXPECT_EXIT(mLongType.castTo(mContext, expressionValue, PrimitiveTypes::VOID_TYPE, 0),
              ::testing::ExitedWithCode(1),
              "Error: Incompatible types: can not cast from type 'long' to 'void'");
  
  result = mLongType.castTo(mContext, expressionValue, PrimitiveTypes::BOOLEAN_TYPE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = trunc i64 5 to i1", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mLongType.castTo(mContext, expressionValue, PrimitiveTypes::CHAR_TYPE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv1 = trunc i64 5 to i16", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mLongType.castTo(mContext, expressionValue, PrimitiveTypes::INT_TYPE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv2 = trunc i64 5 to i32", mStringStream->str().c_str());
  mStringBuffer.clear();

  result = mLongType.castTo(mContext, expressionValue, PrimitiveTypes::LONG_TYPE, 0);
  EXPECT_EQ(result, expressionValue);
  
  result = mLongType.castTo(mContext, expressionValue, PrimitiveTypes::FLOAT_TYPE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv3 = sitofp i64 5 to float", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mLongType.castTo(mContext, expressionValue, PrimitiveTypes::DOUBLE_TYPE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv4 = sitofp i64 5 to double", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LongTypeTest, isOwnerTest) {
  EXPECT_FALSE(mLongType.isOwner());
}

TEST_F(LongTypeTest, isReferenceTest) {
  EXPECT_FALSE(mLongType.isReference());
}

TEST_F(LongTypeTest, allocateLocalVariableTest) {
  mLongType.allocateLocalVariable(mContext, "temp");
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = alloca i64"
  "\n  store i64 0, i64* %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LongTypeTest, createFieldVariableTest) {
  mLongType.createFieldVariable(mContext, "mField", &mConcreteObjectType);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}
