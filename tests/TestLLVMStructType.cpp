//
//  TestLLVMStructType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/18/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMStructType}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMStructType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LLVMStructTypeTest : public Test {
  
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  StructType* mStructType;
  LLVMStructType* mLLVMStructType;
  BasicBlock* mBasicBlock;
  
  LLVMStructTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    mStructType = StructType::create(mLLVMContext, "mystruct");
    vector<Type*> structBodyTypes;
    structBodyTypes.push_back(Type::getInt8Ty(mLLVMContext));
    structBodyTypes.push_back(Type::getInt64Ty(mLLVMContext));
    mStructType->setBody(structBodyTypes);
    mLLVMStructType = new LLVMStructType(mStructType);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
  }
};

TEST_F(LLVMStructTypeTest, getTypeNameTest) {
  ASSERT_STREQ("mystruct", mLLVMStructType->getTypeName().c_str());
}

TEST_F(LLVMStructTypeTest, getLLVMTypeTest) {
  ASSERT_EQ(mStructType, mLLVMStructType->getLLVMType(mContext));
}

TEST_F(LLVMStructTypeTest, isTypeKindTest) {
  EXPECT_FALSE(mLLVMStructType->isPrimitive());
  EXPECT_FALSE(mLLVMStructType->isOwner());
  EXPECT_FALSE(mLLVMStructType->isReference());
  EXPECT_FALSE(mLLVMStructType->isArray());
  EXPECT_FALSE(mLLVMStructType->isFunction());
  EXPECT_FALSE(mLLVMStructType->isPackage());
  EXPECT_TRUE(mLLVMStructType->isNative());
}

TEST_F(LLVMStructTypeTest, isObjectTest) {
  EXPECT_FALSE(mLLVMStructType->isController());
  EXPECT_FALSE(mLLVMStructType->isInterface());
  EXPECT_FALSE(mLLVMStructType->isModel());
  EXPECT_FALSE(mLLVMStructType->isNode());
  EXPECT_FALSE(mLLVMStructType->isThread());
}

TEST_F(LLVMStructTypeTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  IField* field = new FixedField(mLLVMStructType, "mField");
  ON_CALL(concreteObjectType, findField(_)).WillByDefault(Return(field));
  mLLVMStructType->createFieldVariable(mContext, "mField", &concreteObjectType);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(LLVMStructTypeTest, canCastToTest) {
  LLVMStructType* anotherType = new LLVMStructType(mStructType);
  
  EXPECT_TRUE(mLLVMStructType->canCastTo(mContext, anotherType));
}

TEST_F(LLVMStructTypeTest, canAutoCastToTest) {
  LLVMStructType* anotherType = new LLVMStructType(mStructType);

  EXPECT_TRUE(mLLVMStructType->canAutoCastTo(mContext, anotherType));
}

TEST_F(LLVMStructTypeTest, getPointerTypeTest) {
  const IType* pointerType = mLLVMStructType->getPointerType();
  EXPECT_EQ(mStructType->getPointerTo(), pointerType->getLLVMType(mContext));
}
