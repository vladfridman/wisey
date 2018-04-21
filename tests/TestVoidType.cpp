//
//  TestVoidType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link VoidType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "TestPrefix.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/VoidType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct VoidTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  VoidType mVoidType;
  
public:
  
  VoidTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
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
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~VoidTypeTest() {
    delete mStringStream;
  }
};

TEST_F(VoidTypeTest, voidTypeTest) {
  EXPECT_EQ(Type::getVoidTy(mLLVMContext), mVoidType.getLLVMType(mContext));
  EXPECT_STREQ("void", mVoidType.getTypeName().c_str());
  EXPECT_EQ("", mVoidType.getFormat());
}

TEST_F(VoidTypeTest, canAutoCastToTest) {
  EXPECT_TRUE(mVoidType.canAutoCastTo(mContext, PrimitiveTypes::VOID_TYPE));
  EXPECT_FALSE(mVoidType.canAutoCastTo(mContext, PrimitiveTypes::BOOLEAN_TYPE));
  EXPECT_FALSE(mVoidType.canAutoCastTo(mContext, PrimitiveTypes::CHAR_TYPE));
  EXPECT_FALSE(mVoidType.canAutoCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mVoidType.canAutoCastTo(mContext, PrimitiveTypes::LONG_TYPE));
  EXPECT_FALSE(mVoidType.canAutoCastTo(mContext, PrimitiveTypes::FLOAT_TYPE));
  EXPECT_FALSE(mVoidType.canAutoCastTo(mContext, PrimitiveTypes::DOUBLE_TYPE));
}

TEST_F(VoidTypeTest, canCastTest) {
  EXPECT_TRUE(mVoidType.canCastTo(mContext, PrimitiveTypes::VOID_TYPE));
  EXPECT_FALSE(mVoidType.canCastTo(mContext, PrimitiveTypes::BOOLEAN_TYPE));
  EXPECT_FALSE(mVoidType.canCastTo(mContext, PrimitiveTypes::CHAR_TYPE));
  EXPECT_FALSE(mVoidType.canCastTo(mContext, PrimitiveTypes::INT_TYPE));
  EXPECT_FALSE(mVoidType.canCastTo(mContext, PrimitiveTypes::LONG_TYPE));
  EXPECT_FALSE(mVoidType.canCastTo(mContext, PrimitiveTypes::FLOAT_TYPE));
  EXPECT_FALSE(mVoidType.canCastTo(mContext, PrimitiveTypes::DOUBLE_TYPE));
}

TEST_F(VoidTypeTest, castToTest) {
  Value* result;
  Value* expressionValue = ConstantFP::get(Type::getDoubleTy(mLLVMContext), 2.5);
  
  result = mVoidType.castTo(mContext, expressionValue, PrimitiveTypes::VOID_TYPE, 0);
  EXPECT_EQ(result, expressionValue);

  EXPECT_EXIT(mVoidType.castTo(mContext, expressionValue, PrimitiveTypes::BOOLEAN_TYPE, 5),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(5\\): Error: Incompatible types: "
              "can not cast from type 'void' to 'boolean'");
  
  EXPECT_EXIT(mVoidType.castTo(mContext, expressionValue, PrimitiveTypes::CHAR_TYPE, 5),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(5\\): Error: Incompatible types: "
              "can not cast from type 'void' to 'char'");
  
  EXPECT_EXIT(mVoidType.castTo(mContext, expressionValue, PrimitiveTypes::INT_TYPE, 5),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(5\\): Error: Incompatible types: "
              "can not cast from type 'void' to 'int'");
  
  EXPECT_EXIT(mVoidType.castTo(mContext, expressionValue, PrimitiveTypes::LONG_TYPE, 5),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(5\\): Error: Incompatible types: "
              "can not cast from type 'void' to 'long'");
  
  EXPECT_EXIT(mVoidType.castTo(mContext, expressionValue, PrimitiveTypes::FLOAT_TYPE, 5),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(5\\): Error: Incompatible types: "
              "can not cast from type 'void' to 'float'");
  
  EXPECT_EXIT(mVoidType.castTo(mContext, expressionValue, PrimitiveTypes::DOUBLE_TYPE, 5),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(5\\): Error: Incompatible types: "
              "can not cast from type 'void' to 'double'");
  
}

TEST_F(VoidTypeTest, isTypeKindTest) {
  EXPECT_TRUE(mVoidType.isPrimitive());
  EXPECT_FALSE(mVoidType.isOwner());
  EXPECT_FALSE(mVoidType.isReference());
  EXPECT_FALSE(mVoidType.isArray());
  EXPECT_FALSE(mVoidType.isFunction());
  EXPECT_FALSE(mVoidType.isPackage());
  EXPECT_FALSE(mVoidType.isNative());
  EXPECT_FALSE(mVoidType.isPointer());
  EXPECT_FALSE(mVoidType.isPointer());
}

TEST_F(VoidTypeTest, isObjectTest) {
  EXPECT_FALSE(mVoidType.isController());
  EXPECT_FALSE(mVoidType.isInterface());
  EXPECT_FALSE(mVoidType.isModel());
  EXPECT_FALSE(mVoidType.isNode());
  EXPECT_FALSE(mVoidType.isThread());
}

TEST_F(VoidTypeTest, injectDeathTest) {
  InjectionArgumentList arguments;
  EXPECT_EXIT(mVoidType.inject(mContext, arguments, 3),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(3\\): Error: type void is not injectable");
}
