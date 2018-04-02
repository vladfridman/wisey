//
//  TestLLVMObjectOwnerTypeSpecifier.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/28/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMObjectOwnerTypeSpecifier}
//

#include <gtest/gtest.h>

#include "wisey/LLVMObjectOwnerTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

using ::testing::Test;

struct LLVMObjectOwnerTypeSpecifierTest : public Test {
  IRGenerationContext mContext;
  LLVMObjectOwnerTypeSpecifier* mLLVMObjectOwnerTypeSpecifier;
  
  LLVMObjectOwnerTypeSpecifierTest() {
    mLLVMObjectOwnerTypeSpecifier = new LLVMObjectOwnerTypeSpecifier();
  }
};

TEST_F(LLVMObjectOwnerTypeSpecifierTest, getTypeTest) {
  const IType* type = mLLVMObjectOwnerTypeSpecifier->getType(mContext);
  
  EXPECT_TRUE(type->isNative());
  EXPECT_FALSE(type->isReference());
  EXPECT_TRUE(type->isOwner());
  EXPECT_STREQ("::llvm::object*", type->getTypeName().c_str());
}

TEST_F(LLVMObjectOwnerTypeSpecifierTest, twoGetsReturnSameTypeObjectTest) {
  const IType* type1 = mLLVMObjectOwnerTypeSpecifier->getType(mContext);
  const IType* type2 = mLLVMObjectOwnerTypeSpecifier->getType(mContext);
  
  EXPECT_EQ(type1, type2);
}

TEST_F(LLVMObjectOwnerTypeSpecifierTest, printToStreamTest) {
  stringstream stringStream;
  mLLVMObjectOwnerTypeSpecifier->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::llvm::object*", stringStream.str().c_str());
}
