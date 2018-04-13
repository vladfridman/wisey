//
//  TestLLVMPointerOwnerTypeSpecifier.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/13/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMPointerOwnerTypeSpecifier}
//

#include <gtest/gtest.h>

#include "wisey/LLVMPointerOwnerTypeSpecifier.hpp"
#include "wisey/LLVMPointerTypeSpecifier.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/LLVMPrimitiveTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

using ::testing::Test;

struct LLVMPointerOwnerTypeSpecifierTest : public Test {
  IRGenerationContext mContext;
  LLVMPointerOwnerTypeSpecifier* mLLVMPointerOwnerTypeSpecifier;
  
  LLVMPointerOwnerTypeSpecifierTest() {
    const ILLVMTypeSpecifier* i8Specifier = LLVMPrimitiveTypes::I8->newTypeSpecifier();
    const LLVMPointerTypeSpecifier* pointerSpecifier = new LLVMPointerTypeSpecifier(i8Specifier);
    mLLVMPointerOwnerTypeSpecifier = new LLVMPointerOwnerTypeSpecifier(pointerSpecifier);
  }
};

TEST_F(LLVMPointerOwnerTypeSpecifierTest, getTypeTest) {
  const IType* type = mLLVMPointerOwnerTypeSpecifier->getType(mContext);
  
  EXPECT_TRUE(type->isNative());
  EXPECT_FALSE(type->isReference());
  EXPECT_TRUE(type->isPointer());
  EXPECT_TRUE(type->isOwner());
  EXPECT_STREQ("::llvm::i8::pointer*", type->getTypeName().c_str());
}

TEST_F(LLVMPointerOwnerTypeSpecifierTest, twoGetsReturnSameTypeObjectTest) {
  const IType* type1 = mLLVMPointerOwnerTypeSpecifier->getType(mContext);
  const IType* type2 = mLLVMPointerOwnerTypeSpecifier->getType(mContext);
  
  EXPECT_EQ(type1, type2);
}

TEST_F(LLVMPointerOwnerTypeSpecifierTest, printToStreamTest) {
  stringstream stringStream;
  mLLVMPointerOwnerTypeSpecifier->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::llvm::i8::pointer*", stringStream.str().c_str());
}
