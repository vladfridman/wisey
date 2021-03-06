//
//  TestImmutableArrayOwnerType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ImmutableArrayOwnerType}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Constants.h>

#include "MockConcreteObjectType.hpp"
#include "TestPrefix.hpp"
#include "IRGenerationContext.hpp"
#include "ImmutableArrayOwnerType.hpp"
#include "ImmutableArrayType.hpp"
#include "PrimitiveTypes.hpp"
#include "ReceivedField.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ImmutableArrayOwnerTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  wisey::ArrayType* mArrayType;
  ImmutableArrayType* mImmutableArrayType;
  ImmutableArrayOwnerType* mImmutableArrayOwnerType;
  BasicBlock* mEntryBlock;
  BasicBlock* mDeclareBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  ImmutableArrayOwnerTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    mArrayType = new wisey::ArrayType(PrimitiveTypes::LONG, 1u);
    mImmutableArrayType = new ImmutableArrayType(mArrayType);
    mImmutableArrayOwnerType = new ImmutableArrayOwnerType(mImmutableArrayType);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mDeclareBlock = BasicBlock::Create(mLLVMContext, "declare", function);
    mEntryBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setDeclarationsBlock(mDeclareBlock);
    mContext.setBasicBlock(mEntryBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
};

TEST_F(ImmutableArrayOwnerTypeTest, getArrayTypeTest) {
  EXPECT_EQ(mArrayType, mImmutableArrayOwnerType->getArrayType(mContext, 0));
}

TEST_F(ImmutableArrayOwnerTypeTest, getNameTest) {
  EXPECT_STREQ("immutable long[]*", mImmutableArrayOwnerType->getTypeName().c_str());
}

TEST_F(ImmutableArrayOwnerTypeTest, getLLVMTypeTest) {
  PointerType* arrayLLVMType = mImmutableArrayOwnerType->getLLVMType(mContext);
  ASSERT_TRUE(arrayLLVMType->getPointerElementType()->isStructTy());
  StructType* arrayStruct = (StructType*) arrayLLVMType->getPointerElementType();
  
  EXPECT_EQ(Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(0));
  EXPECT_EQ(Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(1));
  EXPECT_EQ(Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(2));
  EXPECT_EQ(llvm::ArrayType::get(Type::getInt64Ty(mLLVMContext), 0u),
            arrayStruct->getElementType(wisey:: ArrayType::ARRAY_ELEMENTS_START_INDEX));
}

TEST_F(ImmutableArrayOwnerTypeTest, canCastToTest) {
  EXPECT_FALSE(mImmutableArrayOwnerType->canCastTo(mContext, PrimitiveTypes::STRING));
  EXPECT_TRUE(mImmutableArrayOwnerType->canCastTo(mContext, mImmutableArrayOwnerType));
  EXPECT_TRUE(mImmutableArrayOwnerType->canCastTo(mContext, mImmutableArrayType));
  EXPECT_FALSE(mImmutableArrayOwnerType->canCastTo(mContext, mArrayType));
}

TEST_F(ImmutableArrayOwnerTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mImmutableArrayOwnerType->canAutoCastTo(mContext, PrimitiveTypes::STRING));
  EXPECT_TRUE(mImmutableArrayOwnerType->canAutoCastTo(mContext, mImmutableArrayOwnerType));
  EXPECT_TRUE(mImmutableArrayOwnerType->canAutoCastTo(mContext, mImmutableArrayType));
  EXPECT_FALSE(mImmutableArrayOwnerType->canAutoCastTo(mContext, mArrayType));
}

TEST_F(ImmutableArrayOwnerTypeTest, isTypeKindTest) {
  EXPECT_FALSE(mImmutableArrayOwnerType->isPrimitive());
  EXPECT_TRUE(mImmutableArrayOwnerType->isOwner());
  EXPECT_FALSE(mImmutableArrayOwnerType->isReference());
  EXPECT_TRUE(mImmutableArrayOwnerType->isArray());
  EXPECT_FALSE(mImmutableArrayOwnerType->isFunction());
  EXPECT_FALSE(mImmutableArrayOwnerType->isPackage());
  EXPECT_FALSE(mImmutableArrayOwnerType->isNative());
  EXPECT_FALSE(mImmutableArrayOwnerType->isPointer());
  EXPECT_TRUE(mImmutableArrayOwnerType->isImmutable());
}

TEST_F(ImmutableArrayOwnerTypeTest, isObjectTest) {
  EXPECT_FALSE(mImmutableArrayOwnerType->isController());
  EXPECT_FALSE(mImmutableArrayOwnerType->isInterface());
  EXPECT_FALSE(mImmutableArrayOwnerType->isModel());
  EXPECT_FALSE(mImmutableArrayOwnerType->isNode());
}

TEST_F(ImmutableArrayOwnerTypeTest, createLocalVariableTest) {
  mImmutableArrayOwnerType->createLocalVariable(mContext, "temp", 0);
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mDeclareBlock;
  *mStringStream << *mEntryBlock;

  string expected =
  "\ndeclare:"
  "\n  %0 = alloca { i64, i64, i64, [0 x i64] }*"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  store { i64, i64, i64, [0 x i64] }* null, { i64, i64, i64, [0 x i64] }** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ImmutableArrayOwnerTypeTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  IField* field = new ReceivedField(mImmutableArrayOwnerType, "mField", 0);
  ON_CALL(concreteObjectType, findField(_)).WillByDefault(Return(field));
  mImmutableArrayOwnerType->createFieldVariable(mContext, "mField", &concreteObjectType, 0);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(nullptr, variable);
  EXPECT_EQ(mImmutableArrayOwnerType, variable->getType());
}

TEST_F(ImmutableArrayOwnerTypeTest, createParameterVariableTest) {
  Value* value = ConstantPointerNull::get(mImmutableArrayOwnerType->getLLVMType(mContext));
  mImmutableArrayOwnerType->createParameterVariable(mContext, "var", value, 0);
  IVariable* variable = mContext.getScopes().getVariable("var");
  
  EXPECT_NE(variable, nullptr);
  
  *mStringStream << *mDeclareBlock;
  *mStringStream << *mEntryBlock;

  string expected =
  "\ndeclare:"
  "\n  %var = alloca { i64, i64, i64, [0 x i64] }*"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  store { i64, i64, i64, [0 x i64] }* null, { i64, i64, i64, [0 x i64] }** %var\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ImmutableArrayOwnerTypeTest, injectDeathTest) {
  InjectionArgumentList arguments;
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mImmutableArrayOwnerType->inject(mContext, arguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: type immutable long[]* is not injectable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}
