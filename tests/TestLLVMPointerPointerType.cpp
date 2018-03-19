//
//  TestLLVMPointerPointerType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMPointerPointerType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMPointerPointerType.hpp"
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

struct LLVMPointerPointerTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  LLVMPointerType* mLLVMPointerType;
  LLVMPointerPointerType* mLLVMPointerPointerType;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;
  
public:
  
  LLVMPointerPointerTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
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
    mLLVMPointerPointerType = new LLVMPointerPointerType(mLLVMPointerType);
  }
  
  ~LLVMPointerPointerTypeTest() {
    delete mStringStream;
  }
};

TEST_F(LLVMPointerPointerTypeTest, pointerPointerTypeTest) {
  EXPECT_EQ(Type::getInt8Ty(mLLVMContext)->getPointerTo()->getPointerTo(),
            mLLVMPointerPointerType->getLLVMType(mContext));
  EXPECT_STREQ("::llvm::i8**", mLLVMPointerPointerType->getTypeName().c_str());
}

TEST_F(LLVMPointerPointerTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mLLVMPointerPointerType->canAutoCastTo(mContext, LLVMPrimitiveTypes::I8));
  LLVMPointerType* anotherPointerType = new LLVMPointerType(LLVMPrimitiveTypes::I32);
  EXPECT_TRUE(mLLVMPointerPointerType->canAutoCastTo(mContext, anotherPointerType));
}

TEST_F(LLVMPointerPointerTypeTest, canCastTest) {
  EXPECT_FALSE(mLLVMPointerPointerType->canCastTo(mContext, LLVMPrimitiveTypes::I8));
  LLVMPointerType* anotherPointerType = new LLVMPointerType(LLVMPrimitiveTypes::I32);
  EXPECT_TRUE(mLLVMPointerPointerType->canCastTo(mContext, anotherPointerType));
}

TEST_F(LLVMPointerPointerTypeTest, castToTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  LLVMPointerType* anotherPointerType = new LLVMPointerType(LLVMPrimitiveTypes::I32);
  Value* value = ConstantPointerNull::get(mLLVMPointerPointerType->getLLVMType(mContext));
  Value* result = mLLVMPointerPointerType->castTo(mContext, value, anotherPointerType, 0);
  *mStringStream << *result;
  
  EXPECT_STREQ("  %0 = bitcast i8** null to i32*", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LLVMPointerPointerTypeTest, isTypeKindTest) {
  EXPECT_FALSE(mLLVMPointerPointerType->isPrimitive());
  EXPECT_FALSE(mLLVMPointerPointerType->isOwner());
  EXPECT_TRUE(mLLVMPointerPointerType->isReference());
  EXPECT_FALSE(mLLVMPointerPointerType->isArray());
  EXPECT_FALSE(mLLVMPointerPointerType->isFunction());
  EXPECT_FALSE(mLLVMPointerPointerType->isPackage());
  EXPECT_TRUE(mLLVMPointerPointerType->isNative());
}

TEST_F(LLVMPointerPointerTypeTest, isObjectTest) {
  EXPECT_FALSE(mLLVMPointerPointerType->isController());
  EXPECT_FALSE(mLLVMPointerPointerType->isInterface());
  EXPECT_FALSE(mLLVMPointerPointerType->isModel());
  EXPECT_FALSE(mLLVMPointerPointerType->isNode());
  EXPECT_FALSE(mLLVMPointerPointerType->isThread());
}

TEST_F(LLVMPointerPointerTypeTest, getDereferenceTypeTest) {
  EXPECT_EQ(mLLVMPointerType, mLLVMPointerPointerType->getDereferenceType());
}

TEST_F(LLVMPointerPointerTypeTest, createLocalVariableTest) {
  mLLVMPointerPointerType->createLocalVariable(mContext, "temp");
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %temp = alloca i8**\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LLVMPointerPointerTypeTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  IField* field = new StateField(mLLVMPointerPointerType, "mField");
  ON_CALL(concreteObjectType, findField(_)).WillByDefault(Return(field));
  mLLVMPointerPointerType->createFieldVariable(mContext, "mField", &concreteObjectType);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(LLVMPointerPointerTypeTest, printToStreamTest) {
  stringstream stringStream;
  mLLVMPointerPointerType->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::llvm::i8**", stringStream.str().c_str());
}
