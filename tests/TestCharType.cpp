//
//  TestCharType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link CharType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "wisey/CharType.hpp"
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

struct CharTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  CharType mCharType;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;

public:
  
  CharTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
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
    Field* field = new Field(FIXED_FIELD, &mCharType, NULL, "mField", injectionArgumentList);
    ON_CALL(mConcreteObjectType, findField(_)).WillByDefault(Return(field));

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~CharTypeTest() {
    delete mStringStream;
  }
};

TEST_F(CharTypeTest, charTypeTest) {
  EXPECT_EQ(mCharType.getLLVMType(mContext), (llvm::Type*) Type::getInt16Ty(mLLVMContext));
  EXPECT_STREQ(mCharType.getTypeName().c_str(), "char");
  EXPECT_EQ(mCharType.getFormat(), "%c");
}

TEST_F(CharTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mCharType.canAutoCastTo(mContext, PrimitiveTypes::VOID_TYPE));
  EXPECT_FALSE(mCharType.canAutoCastTo(mContext, PrimitiveTypes::STRING_TYPE));
  EXPECT_FALSE(mCharType.canAutoCastTo(mContext, PrimitiveTypes::BOOLEAN_TYPE));
  EXPECT_TRUE(mCharType.canAutoCastTo(mContext, PrimitiveTypes::CHAR_TYPE));
  EXPECT_TRUE(mCharType.canAutoCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(mCharType.canAutoCastTo(mContext, PrimitiveTypes::LONG_TYPE));
  EXPECT_TRUE(mCharType.canAutoCastTo(mContext, PrimitiveTypes::FLOAT_TYPE));
  EXPECT_TRUE(mCharType.canAutoCastTo(mContext, PrimitiveTypes::DOUBLE_TYPE));
}

TEST_F(CharTypeTest, canCastTest) {
  EXPECT_FALSE(mCharType.canCastTo(mContext, PrimitiveTypes::VOID_TYPE));
  EXPECT_FALSE(mCharType.canCastTo(mContext, PrimitiveTypes::STRING_TYPE));
  EXPECT_TRUE(mCharType.canCastTo(mContext, PrimitiveTypes::BOOLEAN_TYPE));
  EXPECT_TRUE(mCharType.canCastTo(mContext, PrimitiveTypes::CHAR_TYPE));
  EXPECT_TRUE(mCharType.canCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(mCharType.canCastTo(mContext, PrimitiveTypes::LONG_TYPE));
  EXPECT_TRUE(mCharType.canCastTo(mContext, PrimitiveTypes::FLOAT_TYPE));
  EXPECT_TRUE(mCharType.canCastTo(mContext, PrimitiveTypes::DOUBLE_TYPE));
}

TEST_F(CharTypeTest, castToTest) {
  Mock::AllowLeak(&mConcreteObjectType);

  Value* result;
  Value* expressionValue = ConstantInt::get(Type::getInt16Ty(mLLVMContext), 'a');
  
  EXPECT_EXIT(mCharType.castTo(mContext, expressionValue, PrimitiveTypes::VOID_TYPE, 0),
              ::testing::ExitedWithCode(1),
              "Error: Incompatible types: can not cast from type 'char' to 'void'");
  
  result = mCharType.castTo(mContext, expressionValue, PrimitiveTypes::BOOLEAN_TYPE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = trunc i16 97 to i1", mStringStream->str().c_str());
  mStringBuffer.clear();

  result = mCharType.castTo(mContext, expressionValue, PrimitiveTypes::CHAR_TYPE, 0);
  EXPECT_EQ(result, expressionValue);
  
  result = mCharType.castTo(mContext, expressionValue, PrimitiveTypes::INT_TYPE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv1 = zext i16 97 to i32", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mCharType.castTo(mContext, expressionValue, PrimitiveTypes::LONG_TYPE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv2 = zext i16 97 to i64", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mCharType.castTo(mContext, expressionValue, PrimitiveTypes::FLOAT_TYPE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv3 = sitofp i16 97 to float", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mCharType.castTo(mContext, expressionValue, PrimitiveTypes::DOUBLE_TYPE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv4 = sitofp i16 97 to double", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(CharTypeTest, isPrimitiveTest) {
  EXPECT_TRUE(mCharType.isPrimitive());
}

TEST_F(CharTypeTest, isOwnerTest) {
  EXPECT_FALSE(mCharType.isOwner());
}

TEST_F(CharTypeTest, isReferenceTest) {
  EXPECT_FALSE(mCharType.isReference());
}

TEST_F(CharTypeTest, isArrayTest) {
  EXPECT_FALSE(mCharType.isArray());
}

TEST_F(CharTypeTest, isFunctionTest) {
  EXPECT_FALSE(mCharType.isFunction());
}

TEST_F(CharTypeTest, isPackageTest) {
  EXPECT_FALSE(mCharType.isPackage());
}

TEST_F(CharTypeTest, isObjectTest) {
  EXPECT_FALSE(mCharType.isController());
  EXPECT_FALSE(mCharType.isInterface());
  EXPECT_FALSE(mCharType.isModel());
  EXPECT_FALSE(mCharType.isNode());
  EXPECT_FALSE(mCharType.isThread());
  EXPECT_FALSE(mCharType.isNative());
}

TEST_F(CharTypeTest, createLocalVariableTest) {
  mCharType.createLocalVariable(mContext, "temp");
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = alloca i16"
  "\n  store i16 0, i16* %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(CharTypeTest, createFieldVariableTest) {
  mCharType.createFieldVariable(mContext, "mField", &mConcreteObjectType);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(CharTypeTest, createParameterVariableTest) {
  mCharType.createParameterVariable(mContext, "var", NULL);
  IVariable* variable = mContext.getScopes().getVariable("var");
  
  EXPECT_NE(variable, nullptr);
}
