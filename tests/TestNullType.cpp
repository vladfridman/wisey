//
//  TestNullType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link NullType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "TestPrefix.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/NullType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct NullTypeTest : public Test {

  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  Function* mFunction;
  raw_string_ostream* mStringStream;
  
  NullTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    FunctionType* functionType = FunctionType::get(Type::getVoidTy(mLLVMContext), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "test",
                                 mContext.getModule());

    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
};

TEST_F(NullTypeTest, getNameTest) {
  ASSERT_STREQ("null", NullType::NULL_TYPE->getTypeName().c_str());
}

TEST_F(NullTypeTest, getLLVMTypeTest) {
  Type* int8PointerType = Type::getInt8Ty(mLLVMContext)->getPointerTo();
  ASSERT_EQ(int8PointerType, NullType::NULL_TYPE->getLLVMType(mContext));
}

TEST_F(NullTypeTest, canCastToTest) {
  string modelFullName = "systems.vos.wisey.compiler.tests.MSquare";
  StructType* structType = StructType::create(mLLVMContext, modelFullName);
  Model* model = Model::newModel(AccessLevel::PUBLIC_ACCESS, modelFullName, structType, 0);

  ASSERT_TRUE(NullType::NULL_TYPE->canCastTo(mContext, model));
  ASSERT_TRUE(NullType::NULL_TYPE->canCastTo(mContext, model->getOwner()));
  
  NullType anotherNullType;
  ASSERT_TRUE(NullType::NULL_TYPE->canCastTo(mContext, &anotherNullType));
  
  ASSERT_FALSE(NullType::NULL_TYPE->canCastTo(mContext, PrimitiveTypes::INT_TYPE));
}

TEST_F(NullTypeTest, canAutoCastToTest) {
  string modelFullName = "systems.vos.wisey.compiler.tests.MSquare";
  StructType* structType = StructType::create(mLLVMContext, modelFullName);
  Model* model = Model::newModel(AccessLevel::PUBLIC_ACCESS, modelFullName, structType, 0);
  
  ASSERT_TRUE(NullType::NULL_TYPE->canAutoCastTo(mContext, model));
  ASSERT_TRUE(NullType::NULL_TYPE->canAutoCastTo(mContext, model->getOwner()));
  
  NullType anotherNullType;
  ASSERT_TRUE(NullType::NULL_TYPE->canAutoCastTo(mContext, &anotherNullType));
  
  ASSERT_FALSE(NullType::NULL_TYPE->canAutoCastTo(mContext, PrimitiveTypes::INT_TYPE));
}

TEST_F(NullTypeTest, castToModelTest) {
  string modelFullName = "systems.vos.wisey.compiler.tests.MSquare";
  StructType* structType = StructType::create(mLLVMContext, modelFullName);
  Model* model = Model::newModel(AccessLevel::PUBLIC_ACCESS, modelFullName, structType, 0);
  
  Value* cast = NullType::NULL_TYPE->castTo(mContext, NULL, model, 0);

  EXPECT_EQ(cast, ConstantPointerNull::get(structType->getPointerTo()));
}

TEST_F(NullTypeTest, castToModelOwnerTest) {
  string modelFullName = "systems.vos.wisey.compiler.tests.MSquare";
  StructType* structType = StructType::create(mLLVMContext, modelFullName);
  Model* model = Model::newModel(AccessLevel::PUBLIC_ACCESS, modelFullName, structType, 0);

  Value* cast = NullType::NULL_TYPE->castTo(mContext, NULL, model->getOwner(), 0);
  
  EXPECT_EQ(cast, ConstantPointerNull::get(structType->getPointerTo()));
}

TEST_F(NullTypeTest, isTypeKindTest) {
  EXPECT_FALSE(NullType::NULL_TYPE->isPrimitive());
  EXPECT_FALSE(NullType::NULL_TYPE->isOwner());
  EXPECT_FALSE(NullType::NULL_TYPE->isReference());
  EXPECT_FALSE(NullType::NULL_TYPE->isArray());
  EXPECT_FALSE(NullType::NULL_TYPE->isFunction());
  EXPECT_FALSE(NullType::NULL_TYPE->isPackage());
  EXPECT_FALSE(NullType::NULL_TYPE->isNative());
  EXPECT_FALSE(NullType::NULL_TYPE->isPointer());
  EXPECT_FALSE(NullType::NULL_TYPE->isPointer());
}

TEST_F(NullTypeTest, isObjectTest) {
  EXPECT_FALSE(NullType::NULL_TYPE->isController());
  EXPECT_FALSE(NullType::NULL_TYPE->isInterface());
  EXPECT_FALSE(NullType::NULL_TYPE->isModel());
  EXPECT_FALSE(NullType::NULL_TYPE->isNode());
  EXPECT_FALSE(NullType::NULL_TYPE->isThread());
}

TEST_F(NullTypeTest, injectDeathTest) {
  InjectionArgumentList arguments;
  EXPECT_EXIT(NullType::NULL_TYPE->inject(mContext, arguments, 3),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(3\\): Error: type null is not injectable");
}
