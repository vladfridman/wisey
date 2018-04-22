//
//  TestWiseyModelType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link WiseyModelType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/StateField.hpp"
#include "wisey/WiseyObjectOwnerType.hpp"
#include "wisey/WiseyObjectType.hpp"
#include "wisey/WiseyModelType.hpp"
#include "wisey/WiseyModelOwnerType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct WiseyModelTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  WiseyModelType* mWiseyModelType;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;
  
public:
  
  WiseyModelTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
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
    
    mWiseyModelType = new WiseyModelType();
  }
  
  ~WiseyModelTypeTest() {
    delete mStringStream;
  }
};

TEST_F(WiseyModelTypeTest, pointerTypeTest) {
  EXPECT_EQ(Type::getInt8Ty(mLLVMContext)->getPointerTo(), mWiseyModelType->getLLVMType(mContext));
  EXPECT_STREQ("::wisey::model", mWiseyModelType->getTypeName().c_str());
}

TEST_F(WiseyModelTypeTest, canCastTest) {
  EXPECT_FALSE(mWiseyModelType->canCastTo(mContext, LLVMPrimitiveTypes::I8));
  LLVMPointerType* pointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I32);
  EXPECT_TRUE(mWiseyModelType->canCastTo(mContext, pointerType));
  EXPECT_TRUE(mWiseyModelType->canCastTo(mContext, &mConcreteObjectType));
  EXPECT_TRUE(mWiseyModelType->canCastTo(mContext, WiseyObjectType::WISEY_OBJECT_TYPE));
  EXPECT_FALSE(mWiseyModelType->canCastTo(mContext, WiseyObjectOwnerType::WISEY_OBJECT_OWNER_TYPE));
}

TEST_F(WiseyModelTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mWiseyModelType->canAutoCastTo(mContext, LLVMPrimitiveTypes::I8));
  LLVMPointerType* pointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I32);
  EXPECT_TRUE(mWiseyModelType->canAutoCastTo(mContext, pointerType));
  EXPECT_TRUE(mWiseyModelType->canAutoCastTo(mContext, &mConcreteObjectType));
  EXPECT_TRUE(mWiseyModelType->canAutoCastTo(mContext, WiseyObjectType::WISEY_OBJECT_TYPE));
  EXPECT_FALSE(mWiseyModelType->
               canAutoCastTo(mContext, WiseyObjectOwnerType::WISEY_OBJECT_OWNER_TYPE));
}

TEST_F(WiseyModelTypeTest, castToTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  LLVMPointerType* pointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I32);
  Value* value = ConstantPointerNull::get(mWiseyModelType->getLLVMType(mContext));
  Value* result = mWiseyModelType->castTo(mContext, value, pointerType, 0);
  *mStringStream << *result;
  
  EXPECT_STREQ("  %0 = bitcast i8* null to i32*", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(WiseyModelTypeTest, castToObjectTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  Value* value = ConstantPointerNull::get(mWiseyModelType->getLLVMType(mContext));
  Value* result = mWiseyModelType->castTo(mContext, value, &mConcreteObjectType, 0);
  *mStringStream << *result;
  
  EXPECT_STREQ("  %0 = bitcast i8* null to %mystruct*", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(WiseyModelTypeTest, isTypeKindTest) {
  EXPECT_FALSE(mWiseyModelType->isPrimitive());
  EXPECT_FALSE(mWiseyModelType->isOwner());
  EXPECT_TRUE(mWiseyModelType->isReference());
  EXPECT_FALSE(mWiseyModelType->isArray());
  EXPECT_FALSE(mWiseyModelType->isFunction());
  EXPECT_FALSE(mWiseyModelType->isPackage());
  EXPECT_TRUE(mWiseyModelType->isNative());
  EXPECT_FALSE(mWiseyModelType->isPointer());
  EXPECT_TRUE(mWiseyModelType->isImmutable());
}

TEST_F(WiseyModelTypeTest, isObjectTest) {
  EXPECT_FALSE(mWiseyModelType->isController());
  EXPECT_FALSE(mWiseyModelType->isInterface());
  EXPECT_TRUE(mWiseyModelType->isModel());
  EXPECT_FALSE(mWiseyModelType->isNode());
  EXPECT_FALSE(mWiseyModelType->isThread());
}

TEST_F(WiseyModelTypeTest, createLocalVariableTest) {
  mWiseyModelType->createLocalVariable(mContext, "temp");
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

TEST_F(WiseyModelTypeTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  IField* field = new StateField(mWiseyModelType, "mField", 0);
  ON_CALL(concreteObjectType, findField(_)).WillByDefault(Return(field));
  mWiseyModelType->createFieldVariable(mContext, "mField", &concreteObjectType);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(WiseyModelTypeTest, createParameterVariableTest) {
  llvm::Constant* null = ConstantPointerNull::get(mWiseyModelType->getLLVMType(mContext));
  mWiseyModelType->createParameterVariable(mContext, "foo", null);
  IVariable* variable = mContext.getScopes().getVariable("foo");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(WiseyModelTypeTest, printToStreamTest) {
  stringstream stringStream;
  mWiseyModelType->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::wisey::model", stringStream.str().c_str());
}

TEST_F(WiseyModelTypeTest, getOwnerTest) {
  EXPECT_EQ(WiseyModelOwnerType::WISEY_MODEL_OWNER_TYPE, mWiseyModelType->getOwner());
}

TEST_F(WiseyModelTypeTest, injectDeathTest) {
  ::Mock::AllowLeak(&mConcreteObjectType);
  
  InjectionArgumentList arguments;
  EXPECT_EXIT(mWiseyModelType->inject(mContext, arguments, 3),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(3\\): Error: type ::wisey::model is not injectable");
}
