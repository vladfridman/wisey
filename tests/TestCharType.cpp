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
#include "TestPrefix.hpp"
#include "wisey/CharType.hpp"
#include "wisey/FixedField.hpp"
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
    
    IField* field = new FixedField(&mCharType, "mField", 0);
    ON_CALL(mConcreteObjectType, findField(_)).WillByDefault(Return(field));

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~CharTypeTest() {
    delete mStringStream;
  }
};

TEST_F(CharTypeTest, charTypeTest) {
  EXPECT_EQ(Type::getInt16Ty(mLLVMContext), mCharType.getLLVMType(mContext));
  EXPECT_STREQ("char", mCharType.getTypeName().c_str());
  EXPECT_EQ("%c", mCharType.getFormat());
}

TEST_F(CharTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mCharType.canAutoCastTo(mContext, PrimitiveTypes::VOID));
  EXPECT_FALSE(mCharType.canAutoCastTo(mContext, PrimitiveTypes::STRING));
  EXPECT_FALSE(mCharType.canAutoCastTo(mContext, PrimitiveTypes::BOOLEAN));
  EXPECT_TRUE(mCharType.canAutoCastTo(mContext, PrimitiveTypes::CHAR));
  EXPECT_TRUE(mCharType.canAutoCastTo(mContext, PrimitiveTypes::INT));
  EXPECT_TRUE(mCharType.canAutoCastTo(mContext, PrimitiveTypes::LONG));
  EXPECT_TRUE(mCharType.canAutoCastTo(mContext, PrimitiveTypes::FLOAT));
  EXPECT_TRUE(mCharType.canAutoCastTo(mContext, PrimitiveTypes::DOUBLE));
}

TEST_F(CharTypeTest, canCastTest) {
  EXPECT_FALSE(mCharType.canCastTo(mContext, PrimitiveTypes::VOID));
  EXPECT_FALSE(mCharType.canCastTo(mContext, PrimitiveTypes::STRING));
  EXPECT_TRUE(mCharType.canCastTo(mContext, PrimitiveTypes::BOOLEAN));
  EXPECT_TRUE(mCharType.canCastTo(mContext, PrimitiveTypes::CHAR));
  EXPECT_TRUE(mCharType.canCastTo(mContext, PrimitiveTypes::INT));
  EXPECT_TRUE(mCharType.canCastTo(mContext, PrimitiveTypes::LONG));
  EXPECT_TRUE(mCharType.canCastTo(mContext, PrimitiveTypes::FLOAT));
  EXPECT_TRUE(mCharType.canCastTo(mContext, PrimitiveTypes::DOUBLE));
}

TEST_F(CharTypeTest, castToTest) {
  Mock::AllowLeak(&mConcreteObjectType);

  Value* result;
  Value* expressionValue = ConstantInt::get(Type::getInt16Ty(mLLVMContext), 'a');
  
  EXPECT_EXIT(mCharType.castTo(mContext, expressionValue, PrimitiveTypes::VOID, 5),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(5\\): Error: Incompatible types: "
              "can not cast from type 'char' to 'void'");
  
  result = mCharType.castTo(mContext, expressionValue, PrimitiveTypes::BOOLEAN, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = trunc i16 97 to i1", mStringStream->str().c_str());
  mStringBuffer.clear();

  result = mCharType.castTo(mContext, expressionValue, PrimitiveTypes::CHAR, 0);
  EXPECT_EQ(result, expressionValue);
  
  result = mCharType.castTo(mContext, expressionValue, PrimitiveTypes::INT, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv1 = zext i16 97 to i32", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mCharType.castTo(mContext, expressionValue, PrimitiveTypes::LONG, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv2 = zext i16 97 to i64", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mCharType.castTo(mContext, expressionValue, PrimitiveTypes::FLOAT, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv3 = sitofp i16 97 to float", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mCharType.castTo(mContext, expressionValue, PrimitiveTypes::DOUBLE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv4 = sitofp i16 97 to double", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(CharTypeTest, isTypeKindTest) {
  EXPECT_TRUE(mCharType.isPrimitive());
  EXPECT_FALSE(mCharType.isOwner());
  EXPECT_FALSE(mCharType.isReference());
  EXPECT_FALSE(mCharType.isArray());
  EXPECT_FALSE(mCharType.isFunction());
  EXPECT_FALSE(mCharType.isPackage());
  EXPECT_FALSE(mCharType.isNative());
  EXPECT_FALSE(mCharType.isPointer());
  EXPECT_FALSE(mCharType.isPointer());
}

TEST_F(CharTypeTest, isObjectTest) {
  EXPECT_FALSE(mCharType.isController());
  EXPECT_FALSE(mCharType.isInterface());
  EXPECT_FALSE(mCharType.isModel());
  EXPECT_FALSE(mCharType.isNode());
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

TEST_F(CharTypeTest, injectDeathTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  InjectionArgumentList arguments;
  EXPECT_EXIT(mCharType.inject(mContext, arguments, 3),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(3\\): Error: type char is not injectable");
}
