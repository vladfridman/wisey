//
//  TestLLVMImmutablePointerTypeSpecifier.cpp
//  runtests
//
//  Created by Vladimir Fridman on 9/5/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMImmutablePointerTypeSpecifier}
//

#include <gtest/gtest.h>

#include "wisey/LLVMImmutablePointerTypeSpecifier.hpp"
#include "wisey/LLVMPointerTypeSpecifier.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/LLVMPrimitiveTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

using ::testing::Test;

struct LLVMImmutablePointerTypeSpecifierTest : public Test {
  IRGenerationContext mContext;
  LLVMImmutablePointerTypeSpecifier* mLLVMImmutablePointerTypeSpecifier;
  
  LLVMImmutablePointerTypeSpecifierTest() {
    const ILLVMTypeSpecifier* i8Specifier = LLVMPrimitiveTypes::I8->newTypeSpecifier(0);
    LLVMPointerTypeSpecifier* pointerSpecifier = new LLVMPointerTypeSpecifier(i8Specifier, 5);
    mLLVMImmutablePointerTypeSpecifier = new LLVMImmutablePointerTypeSpecifier(pointerSpecifier);
  }
};

TEST_F(LLVMImmutablePointerTypeSpecifierTest, getTypeTest) {
  const IType* type = mLLVMImmutablePointerTypeSpecifier->getType(mContext);
  
  EXPECT_TRUE(type->isNative());
  EXPECT_FALSE(type->isReference());
  EXPECT_TRUE(type->isPointer());
  EXPECT_TRUE(type->isImmutable());
  EXPECT_STREQ("::llvm::i8::pointer::immutable", type->getTypeName().c_str());
}

TEST_F(LLVMImmutablePointerTypeSpecifierTest, twoGetsReturnSameTypeObjectTest) {
  const IType* type1 = mLLVMImmutablePointerTypeSpecifier->getType(mContext);
  const IType* type2 = mLLVMImmutablePointerTypeSpecifier->getType(mContext);
  
  EXPECT_EQ(type1, type2);
}

TEST_F(LLVMImmutablePointerTypeSpecifierTest, printToStreamTest) {
  stringstream stringStream;
  mLLVMImmutablePointerTypeSpecifier->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::llvm::i8::pointer::immutable", stringStream.str().c_str());
}
