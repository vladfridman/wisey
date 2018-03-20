//
//  TestLLVMPointerType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMPointerType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMPointerType.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/StateField.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LLVMPointerTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  LLVMPointerType* mLLVMPointerType;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;
  
public:
  
  LLVMPointerTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    mLLVMPointerType = new LLVMPointerType(LLVMPrimitiveTypes::I8);
  }
  
  ~LLVMPointerTypeTest() {
    delete mStringStream;
  }
};

TEST_F(LLVMPointerTypeTest, pointerTypeTest) {
  EXPECT_EQ(Type::getInt8Ty(mLLVMContext)->getPointerTo(), mLLVMPointerType->getLLVMType(mContext));
  EXPECT_STREQ("::llvm::i8*", mLLVMPointerType->getTypeName().c_str());
}

TEST_F(LLVMPointerTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mLLVMPointerType->canAutoCastTo(mContext, LLVMPrimitiveTypes::I8));
  LLVMPointerType* anotherPointerType = new LLVMPointerType(LLVMPrimitiveTypes::I32);
  EXPECT_TRUE(mLLVMPointerType->canAutoCastTo(mContext, anotherPointerType));
}

TEST_F(LLVMPointerTypeTest, canCastTest) {
  EXPECT_FALSE(mLLVMPointerType->canCastTo(mContext, LLVMPrimitiveTypes::I8));
  LLVMPointerType* anotherPointerType = new LLVMPointerType(LLVMPrimitiveTypes::I32);
  EXPECT_TRUE(mLLVMPointerType->canCastTo(mContext, anotherPointerType));
}

TEST_F(LLVMPointerTypeTest, castToTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  LLVMPointerType* anotherPointerType = new LLVMPointerType(LLVMPrimitiveTypes::I32);
  Value* value = ConstantPointerNull::get(mLLVMPointerType->getLLVMType(mContext));
  Value* result = mLLVMPointerType->castTo(mContext, value, anotherPointerType, 0);
  *mStringStream << *result;
  
  EXPECT_STREQ("  %0 = bitcast i8* null to i32*", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LLVMPointerTypeTest, isTypeKindTest) {
  EXPECT_FALSE(mLLVMPointerType->isPrimitive());
  EXPECT_FALSE(mLLVMPointerType->isOwner());
  EXPECT_TRUE(mLLVMPointerType->isReference());
  EXPECT_FALSE(mLLVMPointerType->isArray());
  EXPECT_FALSE(mLLVMPointerType->isFunction());
  EXPECT_FALSE(mLLVMPointerType->isPackage());
  EXPECT_TRUE(mLLVMPointerType->isNative());
}

TEST_F(LLVMPointerTypeTest, isObjectTest) {
  EXPECT_FALSE(mLLVMPointerType->isController());
  EXPECT_FALSE(mLLVMPointerType->isInterface());
  EXPECT_FALSE(mLLVMPointerType->isModel());
  EXPECT_FALSE(mLLVMPointerType->isNode());
  EXPECT_FALSE(mLLVMPointerType->isThread());
}

TEST_F(LLVMPointerTypeTest, getDereferenceTypeTest) {
  EXPECT_EQ(LLVMPrimitiveTypes::I8, mLLVMPointerType->getDereferenceType());
}

TEST_F(LLVMPointerTypeTest, createLocalVariableTest) {
  mLLVMPointerType->createLocalVariable(mContext, "temp");
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %temp = alloca i8*\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LLVMPointerTypeTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  IField* field = new StateField(mLLVMPointerType, "mField");
  ON_CALL(concreteObjectType, findField(_)).WillByDefault(Return(field));
  mLLVMPointerType->createFieldVariable(mContext, "mField", &concreteObjectType);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(LLVMPointerTypeTest, printToStreamTest) {
  stringstream stringStream;
  mLLVMPointerType->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::llvm::i8*", stringStream.str().c_str());
}

TEST_F(LLVMPointerTypeTest, getPointerTypeTest) {
  EXPECT_EQ(mLLVMPointerType, mLLVMPointerType->getPointerType()->getDereferenceType());
}