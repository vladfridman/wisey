//
//  TestLLVMArrayTypeSpecifer.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMArrayTypeSpecifer}
//

#include <gtest/gtest.h>

#include "LLVMArrayTypeSpecifer.hpp"
#include "LLVMPrimitiveTypes.hpp"
#include "LLVMPrimitiveTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

using ::testing::Test;

struct LLVMArrayTypeSpeciferTest : public Test {
  IRGenerationContext mContext;
  LLVMArrayTypeSpecifer* mLLVMArrayTypeSpecifier;
  
  LLVMArrayTypeSpeciferTest() {
    const ILLVMTypeSpecifier* i8Specifier = LLVMPrimitiveTypes::I8->newTypeSpecifier(3);
    list<unsigned long> dimensions;
    dimensions.push_back(5u);
    dimensions.push_back(2u);
    mLLVMArrayTypeSpecifier = new LLVMArrayTypeSpecifer(i8Specifier, dimensions, 3);
  }
};

TEST_F(LLVMArrayTypeSpeciferTest, creationTest) {
  const LLVMArrayType* arrayType = mLLVMArrayTypeSpecifier->getType(mContext);
  
  EXPECT_TRUE(arrayType->isArray());
  EXPECT_FALSE(arrayType->isReference());
  EXPECT_STREQ("::llvm::array(::llvm::i8, 5, 2)", arrayType->getTypeName().c_str());
  
  EXPECT_EQ(LLVMPrimitiveTypes::I8, arrayType->getElementType());
  EXPECT_EQ(2u, arrayType->getNumberOfDimensions());
  EXPECT_EQ(2u, arrayType->getDimensions().back());
  EXPECT_EQ(5u, arrayType->getDimensions().front());
  EXPECT_EQ(3, mLLVMArrayTypeSpecifier->getLine());
}

TEST_F(LLVMArrayTypeSpeciferTest, twoGetsReturnSameTypeObjectTest) {
  const LLVMArrayType* arrayType1 = mLLVMArrayTypeSpecifier->getType(mContext);
  const LLVMArrayType* arrayType2 = mLLVMArrayTypeSpecifier->getType(mContext);

  EXPECT_EQ(arrayType1, arrayType2);
}

TEST_F(LLVMArrayTypeSpeciferTest, printToStreamTest) {
  stringstream stringStream;
  mLLVMArrayTypeSpecifier->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::llvm::array(::llvm::i8, 5, 2)", stringStream.str().c_str());
}

