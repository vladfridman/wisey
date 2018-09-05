//
//  TestLLVMImmutablePointerOwnerTypeSpecifier.cpp
//  runtests
//
//  Created by Vladimir Fridman on 9/5/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMImmutablePointerOwnerTypeSpecifier}
//

#include <gtest/gtest.h>

#include "wisey/LLVMImmutablePointerOwnerTypeSpecifier.hpp"
#include "wisey/LLVMImmutablePointerTypeSpecifier.hpp"
#include "wisey/LLVMPointerTypeSpecifier.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/LLVMPrimitiveTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

using ::testing::Test;

struct LLVMImmutablePointerOwnerTypeSpecifierTest : public Test {
  IRGenerationContext mContext;
  LLVMImmutablePointerOwnerTypeSpecifier* mLLVMImmutablePointerOwnerTypeSpecifier;
  
  LLVMImmutablePointerOwnerTypeSpecifierTest() {
    const ILLVMTypeSpecifier* i8Specifier = LLVMPrimitiveTypes::I8->newTypeSpecifier(5);
    LLVMPointerTypeSpecifier* pointerSpecifier = new LLVMPointerTypeSpecifier(i8Specifier, 5);
    const LLVMImmutablePointerTypeSpecifier* immutablePointerSpecifier =
    new LLVMImmutablePointerTypeSpecifier(pointerSpecifier);
    mLLVMImmutablePointerOwnerTypeSpecifier =
    new LLVMImmutablePointerOwnerTypeSpecifier(immutablePointerSpecifier);
  }
};

TEST_F(LLVMImmutablePointerOwnerTypeSpecifierTest, getTypeTest) {
  const IType* type = mLLVMImmutablePointerOwnerTypeSpecifier->getType(mContext);
  
  EXPECT_TRUE(type->isNative());
  EXPECT_FALSE(type->isReference());
  EXPECT_TRUE(type->isPointer());
  EXPECT_TRUE(type->isOwner());
  EXPECT_TRUE(type->isImmutable());
  EXPECT_STREQ("::llvm::i8::pointer::immutable*", type->getTypeName().c_str());
  EXPECT_EQ(5, mLLVMImmutablePointerOwnerTypeSpecifier->getLine());
}

TEST_F(LLVMImmutablePointerOwnerTypeSpecifierTest, twoGetsReturnSameTypeObjectTest) {
  const IType* type1 = mLLVMImmutablePointerOwnerTypeSpecifier->getType(mContext);
  const IType* type2 = mLLVMImmutablePointerOwnerTypeSpecifier->getType(mContext);
  
  EXPECT_EQ(type1, type2);
}

TEST_F(LLVMImmutablePointerOwnerTypeSpecifierTest, printToStreamTest) {
  stringstream stringStream;
  mLLVMImmutablePointerOwnerTypeSpecifier->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::llvm::i8::pointer::immutable*", stringStream.str().c_str());
}
