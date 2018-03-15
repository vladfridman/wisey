//
//  TestPointerPointerType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/14/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link PointerPointerType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "wisey/PointerPointerType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/NativeType.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct PointerPointerTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  wisey::PointerType* mPointerType;
  PointerPointerType* mPointerPointerType;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;
  
public:
  
  PointerPointerTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    mPointerType = new wisey::PointerType(PrimitiveTypes::INT_TYPE);
    mPointerPointerType = new PointerPointerType(mPointerType);
  }
  
  ~PointerPointerTypeTest() {
    delete mStringStream;
  }
};

TEST_F(PointerPointerTypeTest, pointerPointerTypeTest) {
  EXPECT_EQ(Type::getInt32Ty(mLLVMContext)->getPointerTo()->getPointerTo(),
            mPointerPointerType->getLLVMType(mContext));
  EXPECT_STREQ("pointerpointer", mPointerPointerType->getTypeName().c_str());
}

TEST_F(PointerPointerTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mPointerPointerType->canAutoCastTo(mContext, PrimitiveTypes::INT_TYPE));
  NativeType* voidPointerType = new NativeType(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  EXPECT_TRUE(mPointerPointerType->canAutoCastTo(mContext, voidPointerType));
}

TEST_F(PointerPointerTypeTest, canCastTest) {
  EXPECT_FALSE(mPointerPointerType->canAutoCastTo(mContext, PrimitiveTypes::INT_TYPE));
  NativeType* voidPointerType = new NativeType(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  EXPECT_TRUE(mPointerPointerType->canAutoCastTo(mContext, voidPointerType));
}

TEST_F(PointerPointerTypeTest, castToTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  
  NativeType* voidPointerType = new NativeType(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  Value* value = ConstantPointerNull::get(mPointerPointerType->getLLVMType(mContext));
  Value* result = mPointerPointerType->castTo(mContext, value, voidPointerType, 0);
  *mStringStream << *result;
  
  EXPECT_STREQ("  %0 = bitcast i32** null to i8*", mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(PointerPointerTypeTest, isTypeKindTest) {
  EXPECT_FALSE(mPointerPointerType->isPrimitive());
  EXPECT_FALSE(mPointerPointerType->isOwner());
  EXPECT_TRUE(mPointerPointerType->isReference());
  EXPECT_FALSE(mPointerPointerType->isArray());
  EXPECT_FALSE(mPointerPointerType->isFunction());
  EXPECT_FALSE(mPointerPointerType->isPackage());
  EXPECT_TRUE(mPointerPointerType->isNative());
}

TEST_F(PointerPointerTypeTest, isObjectTest) {
  EXPECT_FALSE(mPointerPointerType->isController());
  EXPECT_FALSE(mPointerPointerType->isInterface());
  EXPECT_FALSE(mPointerPointerType->isModel());
  EXPECT_FALSE(mPointerPointerType->isNode());
  EXPECT_FALSE(mPointerPointerType->isThread());
}

TEST_F(PointerPointerTypeTest, getDereferenceTypeTest) {
  EXPECT_EQ(mPointerType, mPointerPointerType->getDereferenceType());
}

