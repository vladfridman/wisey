//
//  TestWiseyModelOwnerType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link WiseyModelOwnerType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "TestPrefix.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/StateField.hpp"
#include "wisey/WiseyObjectOwnerType.hpp"
#include "wisey/WiseyObjectType.hpp"
#include "wisey/WiseyModelOwnerType.hpp"
#include "wisey/WiseyModelType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct WiseyModelOwnerTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  WiseyModelOwnerType* mWiseyModelOwnerType;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;
  
public:
  
  WiseyModelOwnerTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
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
    
    mWiseyModelOwnerType = new WiseyModelOwnerType();
  }
  
  ~WiseyModelOwnerTypeTest() {
    delete mStringStream;
  }
};

TEST_F(WiseyModelOwnerTypeTest, pointerTypeTest) {
  EXPECT_EQ(Type::getInt8Ty(mLLVMContext)->getPointerTo(),
            mWiseyModelOwnerType->getLLVMType(mContext));
  EXPECT_STREQ("::wisey::model*", mWiseyModelOwnerType->getTypeName().c_str());
}

TEST_F(WiseyModelOwnerTypeTest, canCastTest) {
  EXPECT_FALSE(mWiseyModelOwnerType->canCastTo(mContext, LLVMPrimitiveTypes::I8));
  LLVMPointerType* pointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I32);
  EXPECT_TRUE(mWiseyModelOwnerType->canCastTo(mContext, pointerType));
  EXPECT_TRUE(mWiseyModelOwnerType->canCastTo(mContext, &mConcreteObjectType));
  EXPECT_TRUE(mWiseyModelOwnerType->canCastTo(mContext, WiseyObjectType::WISEY_OBJECT_TYPE));
  EXPECT_TRUE(mWiseyModelOwnerType->
               canCastTo(mContext, WiseyObjectOwnerType::WISEY_OBJECT_OWNER_TYPE));
}

TEST_F(WiseyModelOwnerTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mWiseyModelOwnerType->canAutoCastTo(mContext, LLVMPrimitiveTypes::I8));
  LLVMPointerType* pointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I32);
  EXPECT_TRUE(mWiseyModelOwnerType->canAutoCastTo(mContext, pointerType));
  EXPECT_TRUE(mWiseyModelOwnerType->canAutoCastTo(mContext, &mConcreteObjectType));
  EXPECT_TRUE(mWiseyModelOwnerType->canAutoCastTo(mContext, WiseyObjectType::WISEY_OBJECT_TYPE));
  EXPECT_TRUE(mWiseyModelOwnerType->
               canAutoCastTo(mContext, WiseyObjectOwnerType::WISEY_OBJECT_OWNER_TYPE));
}

TEST_F(WiseyModelOwnerTypeTest, castToTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  LLVMPointerType* pointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I32);
  Value* value = ConstantPointerNull::get(mWiseyModelOwnerType->getLLVMType(mContext));
  Value* result = mWiseyModelOwnerType->castTo(mContext, value, pointerType, 0);
  *mStringStream << *result;
  
  EXPECT_STREQ("  %0 = bitcast i8* null to i32*", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(WiseyModelOwnerTypeTest, castToObjectTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  Value* value = ConstantPointerNull::get(mWiseyModelOwnerType->getLLVMType(mContext));
  Value* result = mWiseyModelOwnerType->castTo(mContext, value, &mConcreteObjectType, 0);
  *mStringStream << *result;
  
  EXPECT_STREQ("  %0 = bitcast i8* null to %mystruct*", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(WiseyModelOwnerTypeTest, isTypeKindTest) {
  EXPECT_FALSE(mWiseyModelOwnerType->isPrimitive());
  EXPECT_TRUE(mWiseyModelOwnerType->isOwner());
  EXPECT_FALSE(mWiseyModelOwnerType->isReference());
  EXPECT_FALSE(mWiseyModelOwnerType->isArray());
  EXPECT_FALSE(mWiseyModelOwnerType->isFunction());
  EXPECT_FALSE(mWiseyModelOwnerType->isPackage());
  EXPECT_TRUE(mWiseyModelOwnerType->isNative());
  EXPECT_FALSE(mWiseyModelOwnerType->isPointer());
}

TEST_F(WiseyModelOwnerTypeTest, isObjectTest) {
  EXPECT_FALSE(mWiseyModelOwnerType->isController());
  EXPECT_FALSE(mWiseyModelOwnerType->isInterface());
  EXPECT_TRUE(mWiseyModelOwnerType->isModel());
  EXPECT_FALSE(mWiseyModelOwnerType->isNode());
  EXPECT_FALSE(mWiseyModelOwnerType->isThread());
}

TEST_F(WiseyModelOwnerTypeTest, createLocalVariableTest) {
  mWiseyModelOwnerType->createLocalVariable(mContext, "temp");
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

TEST_F(WiseyModelOwnerTypeTest, createParameterVariableTest) {
  Value* null = ConstantPointerNull::get(mWiseyModelOwnerType->getLLVMType(mContext));
  
  mWiseyModelOwnerType->createParameterVariable(mContext, "parameter", null);
  IVariable* variable = mContext.getScopes().getVariable("parameter");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(WiseyModelOwnerTypeTest, printToStreamTest) {
  stringstream stringStream;
  mWiseyModelOwnerType->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::wisey::model*", stringStream.str().c_str());
}

TEST_F(WiseyModelOwnerTypeTest, getReferenceTest) {
  EXPECT_EQ(WiseyModelType::WISEY_MODEL_TYPE, mWiseyModelOwnerType->getReference());
}

TEST_F(WiseyModelOwnerTypeTest, injectDeathTest) {
  ::Mock::AllowLeak(&mConcreteObjectType);
  
  InjectionArgumentList arguments;
  EXPECT_EXIT(mWiseyModelOwnerType->inject(mContext, arguments, 3),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(3\\): Error: type ::wisey::model\\* is not injectable");
}
