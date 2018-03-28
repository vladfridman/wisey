//
//  TestLLVMPointerOwnerType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/28/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMPointerOwnerType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMPointerOwnerType.hpp"
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

struct LLVMPointerOwnerTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  LLVMPointerOwnerType* mLLVMPointerOwnerType;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;
  
public:
  
  LLVMPointerOwnerTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    StructType* structType = StructType::create(mLLVMContext, "mystruct");
    
    ON_CALL(mConcreteObjectType, isReference()).WillByDefault(Return(true));
    ON_CALL(mConcreteObjectType, isNative()).WillByDefault(Return(false));
    ON_CALL(mConcreteObjectType, getLLVMType(_)).WillByDefault(Return(structType->getPointerTo()));
    
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    mLLVMPointerOwnerType = new LLVMPointerOwnerType(LLVMPrimitiveTypes::I8->getPointerType());
  }
  
  ~LLVMPointerOwnerTypeTest() {
    delete mStringStream;
  }
};

TEST_F(LLVMPointerOwnerTypeTest, pointerTypeTest) {
  EXPECT_EQ(Type::getInt8Ty(mLLVMContext)->getPointerTo(),
            mLLVMPointerOwnerType->getLLVMType(mContext));
  EXPECT_STREQ("::llvm::i8::pointer*", mLLVMPointerOwnerType->getTypeName().c_str());
}

TEST_F(LLVMPointerOwnerTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mLLVMPointerOwnerType->canAutoCastTo(mContext, LLVMPrimitiveTypes::I8));
  LLVMPointerOwnerType* anotherPointerType =
  new LLVMPointerOwnerType(LLVMPrimitiveTypes::I32->getPointerType());
  EXPECT_TRUE(mLLVMPointerOwnerType->canAutoCastTo(mContext, anotherPointerType));
  EXPECT_TRUE(mLLVMPointerOwnerType->canAutoCastTo(mContext, &mConcreteObjectType));
}

TEST_F(LLVMPointerOwnerTypeTest, canCastTest) {
  EXPECT_FALSE(mLLVMPointerOwnerType->canCastTo(mContext, LLVMPrimitiveTypes::I8));
  LLVMPointerOwnerType* anotherPointerType =
  new LLVMPointerOwnerType(LLVMPrimitiveTypes::I32->getPointerType());
  EXPECT_TRUE(mLLVMPointerOwnerType->canCastTo(mContext, anotherPointerType));
  EXPECT_TRUE(mLLVMPointerOwnerType->canCastTo(mContext, &mConcreteObjectType));
}

TEST_F(LLVMPointerOwnerTypeTest, castToTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  LLVMPointerOwnerType* anotherPointerType =
  new LLVMPointerOwnerType(LLVMPrimitiveTypes::I32->getPointerType());
  Value* value = ConstantPointerNull::get(mLLVMPointerOwnerType->getLLVMType(mContext));
  Value* result = mLLVMPointerOwnerType->castTo(mContext, value, anotherPointerType, 0);
  *mStringStream << *result;
  
  EXPECT_STREQ("  %0 = bitcast i8* null to i32*", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LLVMPointerOwnerTypeTest, castToObjectTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  Value* value = ConstantPointerNull::get(mLLVMPointerOwnerType->getLLVMType(mContext));
  Value* result = mLLVMPointerOwnerType->castTo(mContext, value, &mConcreteObjectType, 0);
  *mStringStream << *result;
  
  EXPECT_STREQ("  %0 = bitcast i8* null to %mystruct*", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LLVMPointerOwnerTypeTest, isTypeKindTest) {
  EXPECT_FALSE(mLLVMPointerOwnerType->isPrimitive());
  EXPECT_TRUE(mLLVMPointerOwnerType->isOwner());
  EXPECT_FALSE(mLLVMPointerOwnerType->isReference());
  EXPECT_FALSE(mLLVMPointerOwnerType->isArray());
  EXPECT_FALSE(mLLVMPointerOwnerType->isFunction());
  EXPECT_FALSE(mLLVMPointerOwnerType->isPackage());
  EXPECT_TRUE(mLLVMPointerOwnerType->isNative());
}

TEST_F(LLVMPointerOwnerTypeTest, isObjectTest) {
  EXPECT_FALSE(mLLVMPointerOwnerType->isController());
  EXPECT_FALSE(mLLVMPointerOwnerType->isInterface());
  EXPECT_FALSE(mLLVMPointerOwnerType->isModel());
  EXPECT_FALSE(mLLVMPointerOwnerType->isNode());
  EXPECT_FALSE(mLLVMPointerOwnerType->isThread());
}

TEST_F(LLVMPointerOwnerTypeTest, createLocalVariableTest) {
  mLLVMPointerOwnerType->createLocalVariable(mContext, "temp");
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %temp = alloca i8*\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LLVMPointerOwnerTypeTest, createParameterVariableTest) {
  Value* null = ConstantPointerNull::get(mLLVMPointerOwnerType->getLLVMType(mContext));
  
  mLLVMPointerOwnerType->createParameterVariable(mContext, "parameter", null);
  IVariable* variable = mContext.getScopes().getVariable("parameter");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(LLVMPointerOwnerTypeTest, printToStreamTest) {
  stringstream stringStream;
  mLLVMPointerOwnerType->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::llvm::i8::pointer*", stringStream.str().c_str());
}

TEST_F(LLVMPointerOwnerTypeTest, getReferenceTypeTest) {
  EXPECT_EQ(LLVMPrimitiveTypes::I8->getPointerType(), mLLVMPointerOwnerType->getReferenceType());
}
