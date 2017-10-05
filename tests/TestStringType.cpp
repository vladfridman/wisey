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

#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/StringType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct StringTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  StringType mStringType;
  
public:
  
  StringTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
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
  
  ~StringTypeTest() {
    delete mStringStream;
  }
};

TEST_F(StringTypeTest, stringTypeTest) {
  EXPECT_EQ(mStringType.getLLVMType(mLLVMContext), Type::getInt8Ty(mLLVMContext)->getPointerTo());
  EXPECT_STREQ(mStringType.getName().c_str(), "string");
  EXPECT_EQ(mStringType.getTypeKind(), PRIMITIVE_TYPE);
  EXPECT_EQ(mStringType.getFormat(), "%s");
}

TEST_F(StringTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mStringType.canAutoCastTo(PrimitiveTypes::BOOLEAN_TYPE));
  EXPECT_FALSE(mStringType.canAutoCastTo(PrimitiveTypes::CHAR_TYPE));
  EXPECT_FALSE(mStringType.canAutoCastTo(PrimitiveTypes::DOUBLE_TYPE));
  EXPECT_FALSE(mStringType.canAutoCastTo(PrimitiveTypes::FLOAT_TYPE));
  EXPECT_FALSE(mStringType.canAutoCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mStringType.canAutoCastTo(PrimitiveTypes::LONG_TYPE));
  EXPECT_TRUE(mStringType.canAutoCastTo(PrimitiveTypes::STRING_TYPE));
  EXPECT_FALSE(mStringType.canAutoCastTo(PrimitiveTypes::VOID_TYPE));
}

TEST_F(StringTypeTest, canCastTest) {
  EXPECT_FALSE(mStringType.canCastTo(PrimitiveTypes::BOOLEAN_TYPE));
  EXPECT_FALSE(mStringType.canCastTo(PrimitiveTypes::CHAR_TYPE));
  EXPECT_FALSE(mStringType.canCastTo(PrimitiveTypes::DOUBLE_TYPE));
  EXPECT_FALSE(mStringType.canCastTo(PrimitiveTypes::FLOAT_TYPE));
  EXPECT_FALSE(mStringType.canCastTo(PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mStringType.canCastTo(PrimitiveTypes::LONG_TYPE));
  EXPECT_TRUE(mStringType.canCastTo(PrimitiveTypes::STRING_TYPE));
  EXPECT_FALSE(mStringType.canCastTo(PrimitiveTypes::VOID_TYPE));
}

TEST_F(StringTypeTest, castToTest) {
  Value* expressionValue = ConstantPointerNull::get(Type::getInt16Ty(mLLVMContext)->getPointerTo());
  
  EXPECT_EXIT(mStringType.castTo(mContext, expressionValue, PrimitiveTypes::VOID_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incompatible types: can not cast from type 'string' to 'void'");

  EXPECT_EXIT(mStringType.castTo(mContext, expressionValue, PrimitiveTypes::BOOLEAN_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incompatible types: can not cast from type 'string' to 'boolean'");

  EXPECT_EXIT(mStringType.castTo(mContext, expressionValue, PrimitiveTypes::CHAR_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incompatible types: can not cast from type 'string' to 'char'");

  EXPECT_EXIT(mStringType.castTo(mContext, expressionValue, PrimitiveTypes::INT_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incompatible types: can not cast from type 'string' to 'int'");

  EXPECT_EXIT(mStringType.castTo(mContext, expressionValue, PrimitiveTypes::LONG_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incompatible types: can not cast from type 'string' to 'long'");

  EXPECT_EXIT(mStringType.castTo(mContext, expressionValue, PrimitiveTypes::FLOAT_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incompatible types: can not cast from type 'string' to 'float'");

  EXPECT_EXIT(mStringType.castTo(mContext, expressionValue, PrimitiveTypes::DOUBLE_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Incompatible types: can not cast from type 'string' to 'double'");
  
  EXPECT_EQ(mStringType.castTo(mContext, expressionValue, PrimitiveTypes::STRING_TYPE),
            expressionValue);
}
