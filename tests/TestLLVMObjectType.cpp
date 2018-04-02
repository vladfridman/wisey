//
//  TestLLVMObjectType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/2/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMObjectType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"
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

struct LLVMObjectTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  LLVMObjectType* mLLVMObjectType;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;
  
public:
  
  LLVMObjectTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
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
    
    mLLVMObjectType = new LLVMObjectType();
  }
  
  ~LLVMObjectTypeTest() {
    delete mStringStream;
  }
};

TEST_F(LLVMObjectTypeTest, pointerTypeTest) {
  EXPECT_EQ(Type::getInt8Ty(mLLVMContext)->getPointerTo(), mLLVMObjectType->getLLVMType(mContext));
  EXPECT_STREQ("::llvm::object", mLLVMObjectType->getTypeName().c_str());
}

TEST_F(LLVMObjectTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mLLVMObjectType->canAutoCastTo(mContext, LLVMPrimitiveTypes::I8));
  LLVMPointerType* pointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I32);
  EXPECT_TRUE(mLLVMObjectType->canAutoCastTo(mContext, pointerType));
  EXPECT_TRUE(mLLVMObjectType->canAutoCastTo(mContext, &mConcreteObjectType));
}

TEST_F(LLVMObjectTypeTest, canCastTest) {
  EXPECT_FALSE(mLLVMObjectType->canCastTo(mContext, LLVMPrimitiveTypes::I8));
  LLVMPointerType* pointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I32);
  EXPECT_TRUE(mLLVMObjectType->canCastTo(mContext, pointerType));
  EXPECT_TRUE(mLLVMObjectType->canCastTo(mContext, &mConcreteObjectType));
}

TEST_F(LLVMObjectTypeTest, castToTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  LLVMPointerType* pointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I32);
  Value* value = ConstantPointerNull::get(mLLVMObjectType->getLLVMType(mContext));
  Value* result = mLLVMObjectType->castTo(mContext, value, pointerType, 0);
  *mStringStream << *result;
  
  EXPECT_STREQ("  %0 = bitcast i8* null to i32*", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LLVMObjectTypeTest, castToObjectTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  Value* value = ConstantPointerNull::get(mLLVMObjectType->getLLVMType(mContext));
  Value* result = mLLVMObjectType->castTo(mContext, value, &mConcreteObjectType, 0);
  *mStringStream << *result;
  
  EXPECT_STREQ("  %0 = bitcast i8* null to %mystruct*", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LLVMObjectTypeTest, isTypeKindTest) {
  EXPECT_FALSE(mLLVMObjectType->isPrimitive());
  EXPECT_FALSE(mLLVMObjectType->isOwner());
  EXPECT_TRUE(mLLVMObjectType->isReference());
  EXPECT_FALSE(mLLVMObjectType->isArray());
  EXPECT_FALSE(mLLVMObjectType->isFunction());
  EXPECT_FALSE(mLLVMObjectType->isPackage());
  EXPECT_TRUE(mLLVMObjectType->isNative());
}

TEST_F(LLVMObjectTypeTest, isObjectTest) {
  EXPECT_FALSE(mLLVMObjectType->isController());
  EXPECT_FALSE(mLLVMObjectType->isInterface());
  EXPECT_FALSE(mLLVMObjectType->isModel());
  EXPECT_FALSE(mLLVMObjectType->isNode());
  EXPECT_FALSE(mLLVMObjectType->isThread());
}

TEST_F(LLVMObjectTypeTest, createLocalVariableTest) {
  mLLVMObjectType->createLocalVariable(mContext, "temp");
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

TEST_F(LLVMObjectTypeTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  IField* field = new StateField(mLLVMObjectType, "mField");
  ON_CALL(concreteObjectType, findField(_)).WillByDefault(Return(field));
  mLLVMObjectType->createFieldVariable(mContext, "mField", &concreteObjectType);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(LLVMObjectTypeTest, createParameterVariableTest) {
  llvm::Constant* null = ConstantPointerNull::get(mLLVMObjectType->getLLVMType(mContext));
  mLLVMObjectType->createParameterVariable(mContext, "foo", null);
  IVariable* variable = mContext.getScopes().getVariable("foo");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(LLVMObjectTypeTest, printToStreamTest) {
  stringstream stringStream;
  mLLVMObjectType->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::llvm::object", stringStream.str().c_str());
}
