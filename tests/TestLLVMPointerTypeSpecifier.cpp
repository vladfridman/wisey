//
//  TestLLVMPointerTypeSpecifier.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMPointerTypeSpecifier}
//

#include <gtest/gtest.h>

#include "LLVMPointerTypeSpecifier.hpp"
#include "LLVMPrimitiveTypes.hpp"
#include "LLVMPrimitiveTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

using ::testing::Test;

struct LLVMPointerTypeSpecifierTest : public Test {
  IRGenerationContext mContext;
  LLVMPointerTypeSpecifier* mLLVMPointerTypeSpecifier;
  
  LLVMPointerTypeSpecifierTest() {
    const ILLVMTypeSpecifier* i8Specifier = LLVMPrimitiveTypes::I8->newTypeSpecifier(0);
    mLLVMPointerTypeSpecifier = new LLVMPointerTypeSpecifier(i8Specifier, 0);
  }
};

TEST_F(LLVMPointerTypeSpecifierTest, getTypeTest) {
  const IType* type = mLLVMPointerTypeSpecifier->getType(mContext);
  
  EXPECT_TRUE(type->isNative());
  EXPECT_FALSE(type->isReference());
  EXPECT_TRUE(type->isPointer());
  EXPECT_STREQ("::llvm::i8::pointer", type->getTypeName().c_str());
}

TEST_F(LLVMPointerTypeSpecifierTest, twoGetsReturnSameTypeObjectTest) {
  const IType* type1 = mLLVMPointerTypeSpecifier->getType(mContext);
  const IType* type2 = mLLVMPointerTypeSpecifier->getType(mContext);
  
  EXPECT_EQ(type1, type2);
}

TEST_F(LLVMPointerTypeSpecifierTest, printToStreamTest) {
  stringstream stringStream;
  mLLVMPointerTypeSpecifier->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::llvm::i8::pointer", stringStream.str().c_str());
}
