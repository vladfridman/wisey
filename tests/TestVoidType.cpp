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
#include "IRGenerationContext.hpp"
#include "PrimitiveTypes.hpp"
#include "VoidType.hpp"

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
    BasicBlock* declareBlock = BasicBlock::Create(mLLVMContext, "declare", function);
    mBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setDeclarationsBlock(declareBlock);
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
  EXPECT_TRUE(mVoidType.canAutoCastTo(mContext, PrimitiveTypes::VOID));
  EXPECT_FALSE(mVoidType.canAutoCastTo(mContext, PrimitiveTypes::BOOLEAN));
  EXPECT_FALSE(mVoidType.canAutoCastTo(mContext, PrimitiveTypes::CHAR));
  EXPECT_FALSE(mVoidType.canAutoCastTo(mContext, PrimitiveTypes::BYTE));
  EXPECT_FALSE(mVoidType.canAutoCastTo(mContext, PrimitiveTypes::INT));
  EXPECT_FALSE(mVoidType.canAutoCastTo(mContext, PrimitiveTypes::LONG));
  EXPECT_FALSE(mVoidType.canAutoCastTo(mContext, PrimitiveTypes::FLOAT));
  EXPECT_FALSE(mVoidType.canAutoCastTo(mContext, PrimitiveTypes::DOUBLE));
}

TEST_F(VoidTypeTest, canCastTest) {
  EXPECT_TRUE(mVoidType.canCastTo(mContext, PrimitiveTypes::VOID));
  EXPECT_FALSE(mVoidType.canCastTo(mContext, PrimitiveTypes::BOOLEAN));
  EXPECT_FALSE(mVoidType.canCastTo(mContext, PrimitiveTypes::CHAR));
  EXPECT_FALSE(mVoidType.canCastTo(mContext, PrimitiveTypes::BYTE));
  EXPECT_FALSE(mVoidType.canCastTo(mContext, PrimitiveTypes::INT));
  EXPECT_FALSE(mVoidType.canCastTo(mContext, PrimitiveTypes::LONG));
  EXPECT_FALSE(mVoidType.canCastTo(mContext, PrimitiveTypes::FLOAT));
  EXPECT_FALSE(mVoidType.canCastTo(mContext, PrimitiveTypes::DOUBLE));
}

TEST_F(VoidTypeTest, castToTest) {
  Value* result;
  Value* expressionValue = ConstantFP::get(Type::getDoubleTy(mLLVMContext), 2.5);
  
  result = mVoidType.castTo(mContext, expressionValue, PrimitiveTypes::VOID, 0);
  EXPECT_EQ(result, expressionValue);

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mVoidType.castTo(mContext, expressionValue, PrimitiveTypes::BOOLEAN, 5));
  EXPECT_ANY_THROW(mVoidType.castTo(mContext, expressionValue, PrimitiveTypes::CHAR, 5));
  EXPECT_ANY_THROW(mVoidType.castTo(mContext, expressionValue, PrimitiveTypes::BYTE, 5));
  EXPECT_ANY_THROW(mVoidType.castTo(mContext, expressionValue, PrimitiveTypes::INT, 5));
  EXPECT_ANY_THROW(mVoidType.castTo(mContext, expressionValue, PrimitiveTypes::LONG, 5));
  EXPECT_ANY_THROW(mVoidType.castTo(mContext, expressionValue, PrimitiveTypes::FLOAT, 5));
  EXPECT_ANY_THROW(mVoidType.castTo(mContext, expressionValue, PrimitiveTypes::DOUBLE, 5));
  EXPECT_STREQ("/tmp/source.yz(5): Error: Incompatible types: can not cast from type void to boolean\n"
               "/tmp/source.yz(5): Error: Incompatible types: can not cast from type void to char\n"
               "/tmp/source.yz(5): Error: Incompatible types: can not cast from type void to byte\n"
               "/tmp/source.yz(5): Error: Incompatible types: can not cast from type void to int\n"
               "/tmp/source.yz(5): Error: Incompatible types: can not cast from type void to long\n"
               "/tmp/source.yz(5): Error: Incompatible types: can not cast from type void to float\n"
               "/tmp/source.yz(5): Error: Incompatible types: can not cast from type void to double\n",
               buffer.str().c_str());  
  std::cerr.rdbuf(oldbuffer);
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
}

TEST_F(VoidTypeTest, injectDeathTest) {
  InjectionArgumentList arguments;
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mVoidType.inject(mContext, arguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: type void is not injectable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}
