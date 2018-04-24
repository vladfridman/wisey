//
//  TestStringType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link StringType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "TestPrefix.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/StringType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct StringTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  StringType mStringType;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;

public:
  
  StringTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
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
    
    IField* field = new FixedField(&mStringType, "mField", 0);
    ON_CALL(mConcreteObjectType, findField(_)).WillByDefault(Return(field));
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~StringTypeTest() {
    delete mStringStream;
  }
};

TEST_F(StringTypeTest, stringTypeTest) {
  EXPECT_EQ(Type::getInt8Ty(mLLVMContext)->getPointerTo(), mStringType.getLLVMType(mContext));
  EXPECT_STREQ("string", mStringType.getTypeName().c_str());
  EXPECT_EQ("%s", mStringType.getFormat());
}

TEST_F(StringTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mStringType.canAutoCastTo(mContext, PrimitiveTypes::BOOLEAN_TYPE));
  EXPECT_FALSE(mStringType.canAutoCastTo(mContext, PrimitiveTypes::CHAR_TYPE));
  EXPECT_FALSE(mStringType.canAutoCastTo(mContext, PrimitiveTypes::DOUBLE_TYPE));
  EXPECT_FALSE(mStringType.canAutoCastTo(mContext, PrimitiveTypes::FLOAT_TYPE));
  EXPECT_FALSE(mStringType.canAutoCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mStringType.canAutoCastTo(mContext, PrimitiveTypes::LONG_TYPE));
  EXPECT_TRUE(mStringType.canAutoCastTo(mContext, PrimitiveTypes::STRING_TYPE));
  EXPECT_FALSE(mStringType.canAutoCastTo(mContext, PrimitiveTypes::VOID_TYPE));
}

TEST_F(StringTypeTest, canCastTest) {
  EXPECT_FALSE(mStringType.canCastTo(mContext, PrimitiveTypes::BOOLEAN_TYPE));
  EXPECT_FALSE(mStringType.canCastTo(mContext, PrimitiveTypes::CHAR_TYPE));
  EXPECT_FALSE(mStringType.canCastTo(mContext, PrimitiveTypes::DOUBLE_TYPE));
  EXPECT_FALSE(mStringType.canCastTo(mContext, PrimitiveTypes::FLOAT_TYPE));
  EXPECT_FALSE(mStringType.canCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mStringType.canCastTo(mContext, PrimitiveTypes::LONG_TYPE));
  EXPECT_TRUE(mStringType.canCastTo(mContext, PrimitiveTypes::STRING_TYPE));
  EXPECT_FALSE(mStringType.canCastTo(mContext, PrimitiveTypes::VOID_TYPE));
}

TEST_F(StringTypeTest, castToTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  Value* expressionValue = ConstantPointerNull::get(Type::getInt16Ty(mLLVMContext)->getPointerTo());
  
  EXPECT_EXIT(mStringType.castTo(mContext, expressionValue, PrimitiveTypes::VOID_TYPE, 5),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(5\\): Error: Incompatible types: "
              "can not cast from type 'string' to 'void'");

  EXPECT_EXIT(mStringType.castTo(mContext, expressionValue, PrimitiveTypes::BOOLEAN_TYPE, 5),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(5\\): Error: Incompatible types: "
              "can not cast from type 'string' to 'boolean'");

  EXPECT_EXIT(mStringType.castTo(mContext, expressionValue, PrimitiveTypes::CHAR_TYPE, 5),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(5\\): Error: Incompatible types: "
              "can not cast from type 'string' to 'char'");

  EXPECT_EXIT(mStringType.castTo(mContext, expressionValue, PrimitiveTypes::INT_TYPE, 5),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(5\\): Error: Incompatible types: "
              "can not cast from type 'string' to 'int'");

  EXPECT_EXIT(mStringType.castTo(mContext, expressionValue, PrimitiveTypes::LONG_TYPE, 5),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(5\\): Error: Incompatible types: "
              "can not cast from type 'string' to 'long'");

  EXPECT_EXIT(mStringType.castTo(mContext, expressionValue, PrimitiveTypes::FLOAT_TYPE, 5),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(5\\): Error: Incompatible types: "
              "can not cast from type 'string' to 'float'");

  EXPECT_EXIT(mStringType.castTo(mContext, expressionValue, PrimitiveTypes::DOUBLE_TYPE, 5),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(5\\): Error: Incompatible types: "
              "can not cast from type 'string' to 'double'");
  
  EXPECT_EQ(mStringType.castTo(mContext, expressionValue, PrimitiveTypes::STRING_TYPE, 5),
            expressionValue);
}

TEST_F(StringTypeTest, isTypeKindTest) {
  EXPECT_TRUE(mStringType.isPrimitive());
  EXPECT_FALSE(mStringType.isOwner());
  EXPECT_FALSE(mStringType.isReference());
  EXPECT_FALSE(mStringType.isArray());
  EXPECT_FALSE(mStringType.isFunction());
  EXPECT_FALSE(mStringType.isPackage());
  EXPECT_FALSE(mStringType.isNative());
  EXPECT_FALSE(mStringType.isPointer());
}

TEST_F(StringTypeTest, isObjectTest) {
  EXPECT_FALSE(mStringType.isController());
  EXPECT_FALSE(mStringType.isInterface());
  EXPECT_FALSE(mStringType.isModel());
  EXPECT_FALSE(mStringType.isNode());
  EXPECT_FALSE(mStringType.isPointer());
}

TEST_F(StringTypeTest, createLocalVariableTest) {
  mStringType.createLocalVariable(mContext, "temp");
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = alloca i8*"
  "\n  store i8* getelementptr inbounds ([1 x i8], [1 x i8]* @__empty.str, i32 0, i32 0), i8** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(StringTypeTest, createFieldVariableTest) {
  mStringType.createFieldVariable(mContext, "mField", &mConcreteObjectType);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(StringTypeTest, createParameterVariableTest) {
  mStringType.createParameterVariable(mContext, "var", NULL);
  IVariable* variable = mContext.getScopes().getVariable("var");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(StringTypeTest, injectDeathTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  InjectionArgumentList arguments;
  EXPECT_EXIT(mStringType.inject(mContext, arguments, 3),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(3\\): Error: type string is not injectable");
}
