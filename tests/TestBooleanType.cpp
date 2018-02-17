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

#include "wisey/BooleanType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct BooleanTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  BooleanType mBoleanType;
  
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

TEST_F(BooleanTypeTest, allocateVariableTest) {
  mBoleanType.allocateVariable(mContext, "temp");
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
