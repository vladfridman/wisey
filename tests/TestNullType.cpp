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
  
  NullTypeTest() : mLLVMContext(mContext.getLLVMContext()) { }

};

TEST_F(NullTypeTest, getNameTest) {
  ASSERT_STREQ(NullType::NULL_TYPE->getName().c_str(), "null");
}

TEST_F(NullTypeTest, getLLVMTypeTest) {
  Type* int8PointerType = Type::getInt8Ty(mLLVMContext)->getPointerTo();
  ASSERT_EQ(NullType::NULL_TYPE->getLLVMType(mLLVMContext), int8PointerType);
}

TEST_F(NullTypeTest, getTypeKindTest) {
  ASSERT_EQ(NullType::NULL_TYPE->getTypeKind(), NULL_TYPE_KIND);
}

TEST_F(NullTypeTest, canCastToTest) {
  string modelFullName = "systems.vos.wisey.compiler.tests.MSquare";
  StructType* structType = StructType::create(mLLVMContext, modelFullName);
  Model* model = new Model(modelFullName, structType);

  ASSERT_TRUE(NullType::NULL_TYPE->canCastTo(model));
  ASSERT_TRUE(NullType::NULL_TYPE->canCastTo(model->getOwner()));
  
  NullType anotherNullType;
  ASSERT_TRUE(NullType::NULL_TYPE->canCastTo(&anotherNullType));
  
  ASSERT_FALSE(NullType::NULL_TYPE->canCastTo(PrimitiveTypes::INT_TYPE));
}

TEST_F(NullTypeTest, canAutoCastToTest) {
  string modelFullName = "systems.vos.wisey.compiler.tests.MSquare";
  StructType* structType = StructType::create(mLLVMContext, modelFullName);
  Model* model = new Model(modelFullName, structType);
  
  ASSERT_TRUE(NullType::NULL_TYPE->canAutoCastTo(model));
  ASSERT_TRUE(NullType::NULL_TYPE->canAutoCastTo(model->getOwner()));
  
  NullType anotherNullType;
  ASSERT_TRUE(NullType::NULL_TYPE->canAutoCastTo(&anotherNullType));
  
  ASSERT_FALSE(NullType::NULL_TYPE->canAutoCastTo(PrimitiveTypes::INT_TYPE));
}

TEST_F(NullTypeTest, castToTest) {
  string modelFullName = "systems.vos.wisey.compiler.tests.MSquare";
  StructType* structType = StructType::create(mLLVMContext, modelFullName);
  Model* model = new Model(modelFullName, structType);
  
  Value* expected = ConstantExpr::getNullValue(structType->getPointerTo());
  ASSERT_EQ(NullType::NULL_TYPE->castTo(mContext, NULL, model), expected);
  ASSERT_EQ(NullType::NULL_TYPE->castTo(mContext, NULL, model->getOwner()), expected);

  expected = ConstantExpr::getNullValue(Type::getInt8Ty(mLLVMContext)->getPointerTo());
  NullType anotherNullType;
  ASSERT_EQ(NullType::NULL_TYPE->castTo(mContext, NULL, &anotherNullType), expected);
}
