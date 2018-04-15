//
//  TestIntType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IntType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "TestPrefix.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/IntType.hpp"
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

struct IntTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  IntType mIntType;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;

public:
  
  IntTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
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
    
    IField* field = new FixedField(&mIntType, "mField", 0);
    ON_CALL(mConcreteObjectType, findField(_)).WillByDefault(Return(field));

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~IntTypeTest() {
    delete mStringStream;
  }
};

TEST_F(IntTypeTest, intTypeTest) {
  EXPECT_EQ(Type::getInt32Ty(mLLVMContext), mIntType.getLLVMType(mContext));
  EXPECT_STREQ("int", mIntType.getTypeName().c_str());
  EXPECT_EQ("%d", mIntType.getFormat());
}

TEST_F(IntTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mIntType.canAutoCastTo(mContext, PrimitiveTypes::VOID_TYPE));
  EXPECT_FALSE(mIntType.canAutoCastTo(mContext, PrimitiveTypes::STRING_TYPE));
  EXPECT_FALSE(mIntType.canAutoCastTo(mContext, PrimitiveTypes::BOOLEAN_TYPE));
  EXPECT_FALSE(mIntType.canAutoCastTo(mContext, PrimitiveTypes::CHAR_TYPE));
  EXPECT_TRUE(mIntType.canAutoCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(mIntType.canAutoCastTo(mContext, PrimitiveTypes::LONG_TYPE));
  EXPECT_FALSE(mIntType.canAutoCastTo(mContext, PrimitiveTypes::FLOAT_TYPE));
  EXPECT_TRUE(mIntType.canAutoCastTo(mContext, PrimitiveTypes::DOUBLE_TYPE));
}

TEST_F(IntTypeTest, canCastTest) {
  EXPECT_FALSE(mIntType.canCastTo(mContext, PrimitiveTypes::VOID_TYPE));
  EXPECT_FALSE(mIntType.canCastTo(mContext, PrimitiveTypes::STRING_TYPE));
  EXPECT_TRUE(mIntType.canCastTo(mContext, PrimitiveTypes::BOOLEAN_TYPE));
  EXPECT_TRUE(mIntType.canCastTo(mContext, PrimitiveTypes::CHAR_TYPE));
  EXPECT_TRUE(mIntType.canCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_TRUE(mIntType.canCastTo(mContext, PrimitiveTypes::LONG_TYPE));
  EXPECT_TRUE(mIntType.canCastTo(mContext, PrimitiveTypes::FLOAT_TYPE));
  EXPECT_TRUE(mIntType.canCastTo(mContext, PrimitiveTypes::DOUBLE_TYPE));
}

TEST_F(IntTypeTest, castToTest) {
  Mock::AllowLeak(&mConcreteObjectType);

  Value* result;
  Value* expressionValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
  
  EXPECT_EXIT(mIntType.castTo(mContext, expressionValue, PrimitiveTypes::VOID_TYPE, 5),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(5\\): Error: Incompatible types: "
              "can not cast from type 'int' to 'void'");
  
  result = mIntType.castTo(mContext, expressionValue, PrimitiveTypes::BOOLEAN_TYPE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv = trunc i32 5 to i1", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mIntType.castTo(mContext, expressionValue, PrimitiveTypes::CHAR_TYPE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv1 = trunc i32 5 to i16", mStringStream->str().c_str());
  mStringBuffer.clear();

  result = mIntType.castTo(mContext, expressionValue, PrimitiveTypes::INT_TYPE, 0);
  EXPECT_EQ(result, expressionValue);
  
  result = mIntType.castTo(mContext, expressionValue, PrimitiveTypes::LONG_TYPE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv2 = zext i32 5 to i64", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mIntType.castTo(mContext, expressionValue, PrimitiveTypes::FLOAT_TYPE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv3 = sitofp i32 5 to float", mStringStream->str().c_str());
  mStringBuffer.clear();
  
  result = mIntType.castTo(mContext, expressionValue, PrimitiveTypes::DOUBLE_TYPE, 0);
  *mStringStream << *result;
  EXPECT_STREQ("  %conv4 = sitofp i32 5 to double", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(IntTypeTest, isTypeKindTest) {
  EXPECT_TRUE(mIntType.isPrimitive());
  EXPECT_FALSE(mIntType.isOwner());
  EXPECT_FALSE(mIntType.isReference());
  EXPECT_FALSE(mIntType.isArray());
  EXPECT_FALSE(mIntType.isFunction());
  EXPECT_FALSE(mIntType.isPackage());
  EXPECT_FALSE(mIntType.isNative());
  EXPECT_FALSE(mIntType.isPointer());
}

TEST_F(IntTypeTest, isObjectTest) {
  EXPECT_FALSE(mIntType.isController());
  EXPECT_FALSE(mIntType.isInterface());
  EXPECT_FALSE(mIntType.isModel());
  EXPECT_FALSE(mIntType.isNode());
  EXPECT_FALSE(mIntType.isThread());
}

TEST_F(IntTypeTest, createLocalVariableTest) {
  mIntType.createLocalVariable(mContext, "temp");
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = alloca i32"
  "\n  store i32 0, i32* %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(IntTypeTest, createFieldVariableTest) {
  mIntType.createFieldVariable(mContext, "mField", &mConcreteObjectType);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(IntTypeTest, createParameterVariableTest) {
  mIntType.createParameterVariable(mContext, "var", NULL);
  IVariable* variable = mContext.getScopes().getVariable("var");
  
  EXPECT_NE(variable, nullptr);
}
