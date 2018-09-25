//
//  TestArrayOwnerType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 1/16/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ArrayOwnerType}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Constants.h>

#include "MockConcreteObjectType.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "ArrayOwnerType.hpp"
#include "IRGenerationContext.hpp"
#include "PrimitiveTypes.hpp"
#include "ReceivedField.hpp"

using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ArrayOwnerTypeTest : public Test {
  IRGenerationContext mContext;
  llvm::LLVMContext& mLLVMContext;
  ArrayType* mArrayType;
  ArrayOwnerType* mArrayOwnerType;
  llvm::BasicBlock* mEntryBlock;
  llvm::BasicBlock* mDeclareBlock;
  string mStringBuffer;
  llvm::raw_string_ostream* mStringStream;

  ArrayOwnerTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    mArrayType = new ArrayType(PrimitiveTypes::LONG, 1u);
    mArrayOwnerType = new ArrayOwnerType(mArrayType);

    llvm::FunctionType* functionType =
    llvm::FunctionType::get(llvm::Type::getInt32Ty(mContext.getLLVMContext()), false);
    llvm::Function* function = llvm::Function::Create(functionType,
                                                      llvm::GlobalValue::InternalLinkage,
                                                      "main",
                                                      mContext.getModule());
    mDeclareBlock = llvm::BasicBlock::Create(mLLVMContext, "declare", function);
    mEntryBlock = llvm::BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setDeclarationsBlock(mDeclareBlock);
    mContext.setBasicBlock(mEntryBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new llvm::raw_string_ostream(mStringBuffer);
  }
};

TEST_F(ArrayOwnerTypeTest, getArrayTypeTest) {
  EXPECT_EQ(mArrayType, mArrayOwnerType->getArrayType(mContext, 0));
}

TEST_F(ArrayOwnerTypeTest, getNameTest) {
  EXPECT_STREQ("long[]*", mArrayOwnerType->getTypeName().c_str());
}

TEST_F(ArrayOwnerTypeTest, getLLVMTypeTest) {
  llvm::PointerType* arrayLLVMType = mArrayOwnerType->getLLVMType(mContext);
  ASSERT_TRUE(arrayLLVMType->getPointerElementType()->isStructTy());
  llvm::StructType* arrayStruct = (llvm::StructType*) arrayLLVMType->getPointerElementType();

  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(0));
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(1));
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(2));
  EXPECT_EQ(llvm::ArrayType::get(llvm::Type::getInt64Ty(mLLVMContext), 0u),
            arrayStruct->getElementType(ArrayType::ARRAY_ELEMENTS_START_INDEX));
}

TEST_F(ArrayOwnerTypeTest, canCastToTest) {
  EXPECT_TRUE(mArrayOwnerType->canCastTo(mContext, PrimitiveTypes::BOOLEAN));
  EXPECT_FALSE(mArrayOwnerType->canCastTo(mContext, PrimitiveTypes::STRING));
  EXPECT_TRUE(mArrayOwnerType->canCastTo(mContext, mArrayOwnerType));
  EXPECT_TRUE(mArrayOwnerType->canCastTo(mContext, mArrayType));
}

TEST_F(ArrayOwnerTypeTest, canAutoCastToTest) {
  EXPECT_TRUE(mArrayOwnerType->canAutoCastTo(mContext, PrimitiveTypes::BOOLEAN));
  EXPECT_FALSE(mArrayOwnerType->canAutoCastTo(mContext, PrimitiveTypes::STRING));
  EXPECT_TRUE(mArrayOwnerType->canAutoCastTo(mContext, mArrayOwnerType));
  EXPECT_TRUE(mArrayOwnerType->canAutoCastTo(mContext, mArrayType));
}

TEST_F(ArrayOwnerTypeTest, isTypeKindTest) {
  EXPECT_FALSE(mArrayOwnerType->isPrimitive());
  EXPECT_TRUE(mArrayOwnerType->isOwner());
  EXPECT_FALSE(mArrayOwnerType->isReference());
  EXPECT_TRUE(mArrayOwnerType->isArray());
  EXPECT_FALSE(mArrayOwnerType->isFunction());
  EXPECT_FALSE(mArrayOwnerType->isPackage());
  EXPECT_FALSE(mArrayOwnerType->isNative());
  EXPECT_FALSE(mArrayOwnerType->isPointer());
  EXPECT_FALSE(mArrayOwnerType->isImmutable());
}

TEST_F(ArrayOwnerTypeTest, isObjectTest) {
  EXPECT_FALSE(mArrayOwnerType->isController());
  EXPECT_FALSE(mArrayOwnerType->isInterface());
  EXPECT_FALSE(mArrayOwnerType->isModel());
  EXPECT_FALSE(mArrayOwnerType->isNode());
}

TEST_F(ArrayOwnerTypeTest, createLocalVariableTest) {
  mArrayOwnerType->createLocalVariable(mContext, "temp", 0);
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

TEST_F(ArrayOwnerTypeTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  IField* field = new ReceivedField(mArrayOwnerType, "mField", 0);
  ON_CALL(concreteObjectType, findField(_)).WillByDefault(Return(field));
  mArrayOwnerType->createFieldVariable(mContext, "mField", &concreteObjectType, 0);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(ArrayOwnerTypeTest, createParameterVariableTest) {
  llvm::Value* value = llvm::ConstantPointerNull::get(mArrayOwnerType->getLLVMType(mContext));
  mArrayOwnerType->createParameterVariable(mContext, "var", value, 0);
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

TEST_F(ArrayOwnerTypeTest, injectDeathTest) {
  InjectionArgumentList arguments;
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mArrayOwnerType->inject(mContext, arguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: type long[]* is not injectable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, arrayOwnerTypeAutocastToBoolean) {
  runFile("tests/samples/test_array_owner_type_autocast_to_boolean.yz", 5);
}
