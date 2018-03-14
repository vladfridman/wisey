//
//  TestPointerType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/14/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link PointerType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "wisey/PointerType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/NativeVoidPointerType.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct PointerTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  wisey::PointerType* mPointerType;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;
  
public:
  
  PointerTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
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
    
    mPointerType = new wisey::PointerType(PrimitiveTypes::INT_TYPE);
  }
  
  ~PointerTypeTest() {
    delete mStringStream;
  }
};

TEST_F(PointerTypeTest, pointerTypeTest) {
  EXPECT_EQ(Type::getInt32Ty(mLLVMContext)->getPointerTo(), mPointerType->getLLVMType(mContext));
  EXPECT_STREQ("pointer", mPointerType->getTypeName().c_str());
}

TEST_F(PointerTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mPointerType->canAutoCastTo(mContext, PrimitiveTypes::INT_TYPE));
  NativeVoidPointerType* nativeVoidPointerType = new NativeVoidPointerType();
  EXPECT_TRUE(mPointerType->canAutoCastTo(mContext, nativeVoidPointerType));
}

TEST_F(PointerTypeTest, canCastTest) {
  EXPECT_FALSE(mPointerType->canAutoCastTo(mContext, PrimitiveTypes::INT_TYPE));
  NativeVoidPointerType* nativeVoidPointerType = new NativeVoidPointerType();
  EXPECT_TRUE(mPointerType->canAutoCastTo(mContext, nativeVoidPointerType));
}

TEST_F(PointerTypeTest, castToTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  NativeVoidPointerType* nativeVoidPointerType = new NativeVoidPointerType();
  Value* value = ConstantPointerNull::get(mPointerType->getLLVMType(mContext));
  Value* result = mPointerType->castTo(mContext, value, nativeVoidPointerType, 0);
  *mStringStream << *result;
  
  EXPECT_STREQ("  %0 = bitcast i32* null to i8*", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(PointerTypeTest, isTypeKindTest) {
  EXPECT_FALSE(mPointerType->isPrimitive());
  EXPECT_FALSE(mPointerType->isOwner());
  EXPECT_TRUE(mPointerType->isReference());
  EXPECT_FALSE(mPointerType->isArray());
  EXPECT_FALSE(mPointerType->isFunction());
  EXPECT_FALSE(mPointerType->isPackage());
  EXPECT_TRUE(mPointerType->isNative());
}

TEST_F(PointerTypeTest, isObjectTest) {
  EXPECT_FALSE(mPointerType->isController());
  EXPECT_FALSE(mPointerType->isInterface());
  EXPECT_FALSE(mPointerType->isModel());
  EXPECT_FALSE(mPointerType->isNode());
  EXPECT_FALSE(mPointerType->isThread());
}

TEST_F(PointerTypeTest, getPointerTypeTest) {
  EXPECT_EQ(mPointerType, mPointerType->getPointerType()->getDereferenceType());
}

TEST_F(PointerTypeTest, getDereferenceTypeTest) {
  EXPECT_EQ(PrimitiveTypes::INT_TYPE, mPointerType->getDereferenceType());
}

TEST_F(PointerTypeTest, createLocalVariableTest) {
  mPointerType->createLocalVariable(mContext, "temp");
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %temp = alloca i32*\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}
