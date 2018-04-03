//
//  TestLLVMObjectOwnerType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/28/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMObjectOwnerType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMObjectOwnerType.hpp"
#include "wisey/LLVMObjectType.hpp"
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

struct LLVMObjectOwnerTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  LLVMObjectOwnerType* mLLVMObjectOwnerType;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;
  
public:
  
  LLVMObjectOwnerTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
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
    
    mLLVMObjectOwnerType = new LLVMObjectOwnerType();
  }
  
  ~LLVMObjectOwnerTypeTest() {
    delete mStringStream;
  }
};

TEST_F(LLVMObjectOwnerTypeTest, pointerTypeTest) {
  EXPECT_EQ(Type::getInt8Ty(mLLVMContext)->getPointerTo(),
            mLLVMObjectOwnerType->getLLVMType(mContext));
  EXPECT_STREQ("::llvm::object*", mLLVMObjectOwnerType->getTypeName().c_str());
}

TEST_F(LLVMObjectOwnerTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mLLVMObjectOwnerType->canAutoCastTo(mContext, LLVMPrimitiveTypes::I8));
  LLVMPointerType* pointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I32);
  EXPECT_TRUE(mLLVMObjectOwnerType->canAutoCastTo(mContext, pointerType));
  EXPECT_TRUE(mLLVMObjectOwnerType->canAutoCastTo(mContext, &mConcreteObjectType));
}

TEST_F(LLVMObjectOwnerTypeTest, canCastTest) {
  EXPECT_FALSE(mLLVMObjectOwnerType->canCastTo(mContext, LLVMPrimitiveTypes::I8));
  LLVMPointerType* pointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I32);
  EXPECT_TRUE(mLLVMObjectOwnerType->canCastTo(mContext, pointerType));
  EXPECT_TRUE(mLLVMObjectOwnerType->canCastTo(mContext, &mConcreteObjectType));
}

TEST_F(LLVMObjectOwnerTypeTest, castToTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  LLVMPointerType* pointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I32);
  Value* value = ConstantPointerNull::get(mLLVMObjectOwnerType->getLLVMType(mContext));
  Value* result = mLLVMObjectOwnerType->castTo(mContext, value, pointerType, 0);
  *mStringStream << *result;
  
  EXPECT_STREQ("  %0 = bitcast i8* null to i32*", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LLVMObjectOwnerTypeTest, castToObjectTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  Value* value = ConstantPointerNull::get(mLLVMObjectOwnerType->getLLVMType(mContext));
  Value* result = mLLVMObjectOwnerType->castTo(mContext, value, &mConcreteObjectType, 0);
  *mStringStream << *result;
  
  EXPECT_STREQ("  %0 = bitcast i8* null to %mystruct*", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LLVMObjectOwnerTypeTest, isTypeKindTest) {
  EXPECT_FALSE(mLLVMObjectOwnerType->isPrimitive());
  EXPECT_TRUE(mLLVMObjectOwnerType->isOwner());
  EXPECT_FALSE(mLLVMObjectOwnerType->isReference());
  EXPECT_FALSE(mLLVMObjectOwnerType->isArray());
  EXPECT_FALSE(mLLVMObjectOwnerType->isFunction());
  EXPECT_FALSE(mLLVMObjectOwnerType->isPackage());
  EXPECT_TRUE(mLLVMObjectOwnerType->isNative());
  EXPECT_FALSE(mLLVMObjectOwnerType->isPointer());
}

TEST_F(LLVMObjectOwnerTypeTest, isObjectTest) {
  EXPECT_FALSE(mLLVMObjectOwnerType->isController());
  EXPECT_FALSE(mLLVMObjectOwnerType->isInterface());
  EXPECT_FALSE(mLLVMObjectOwnerType->isModel());
  EXPECT_FALSE(mLLVMObjectOwnerType->isNode());
  EXPECT_FALSE(mLLVMObjectOwnerType->isThread());
}

TEST_F(LLVMObjectOwnerTypeTest, createLocalVariableTest) {
  mLLVMObjectOwnerType->createLocalVariable(mContext, "temp");
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %temp = alloca i8*"
  "\n  store i8* null, i8** %temp\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LLVMObjectOwnerTypeTest, createParameterVariableTest) {
  Value* null = ConstantPointerNull::get(mLLVMObjectOwnerType->getLLVMType(mContext));
  
  mLLVMObjectOwnerType->createParameterVariable(mContext, "parameter", null);
  IVariable* variable = mContext.getScopes().getVariable("parameter");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(LLVMObjectOwnerTypeTest, printToStreamTest) {
  stringstream stringStream;
  mLLVMObjectOwnerType->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::llvm::object*", stringStream.str().c_str());
}

TEST_F(LLVMObjectOwnerTypeTest, getReferenceTest) {
  EXPECT_EQ(LLVMObjectType::LLVM_OBJECT_TYPE, mLLVMObjectOwnerType->getReference());
}
