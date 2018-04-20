//
//  TestLLVMFunctionTypeSpecifier.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMFunctionTypeSpecifier}
//

#include <gtest/gtest.h>

#include "wisey/LLVMFunctionTypeSpecifier.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/LLVMPrimitiveTypeSpecifier.hpp"

using namespace std;
using namespace wisey;

using ::testing::Test;

struct LLVMFunctionTypeSpecifierTest : public Test {
  IRGenerationContext mContext;
  LLVMFunctionTypeSpecifier* mLLVMFunctionTypeSpecifier;
  
  LLVMFunctionTypeSpecifierTest() {
    const ILLVMTypeSpecifier* i8Specifier = LLVMPrimitiveTypes::I8->newTypeSpecifier(0);
    vector<const ITypeSpecifier*> argumentTypeSpecifiers;
    argumentTypeSpecifiers.push_back(LLVMPrimitiveTypes::I16->newTypeSpecifier(0));
    argumentTypeSpecifiers.push_back(LLVMPrimitiveTypes::I64->newTypeSpecifier(0));
    mLLVMFunctionTypeSpecifier =
    new LLVMFunctionTypeSpecifier(i8Specifier, argumentTypeSpecifiers, 5);
  }
};

TEST_F(LLVMFunctionTypeSpecifierTest, creationTest) {
  const LLVMFunctionType* functionType = mLLVMFunctionTypeSpecifier->getType(mContext);
  
  EXPECT_FALSE(functionType->isArray());
  EXPECT_FALSE(functionType->isReference());
  EXPECT_TRUE(functionType->isFunction());
  EXPECT_STREQ("::llvm::i8 (::llvm::i16, ::llvm::i64)", functionType->getTypeName().c_str());
  
  EXPECT_EQ(LLVMPrimitiveTypes::I8, functionType->getReturnType());
  EXPECT_EQ(2u, functionType->getArgumentTypes().size());
  EXPECT_EQ(LLVMPrimitiveTypes::I16, functionType->getArgumentTypes().front());
  EXPECT_EQ(LLVMPrimitiveTypes::I64, functionType->getArgumentTypes().back());
  EXPECT_EQ(5, mLLVMFunctionTypeSpecifier->getLine());
}

TEST_F(LLVMFunctionTypeSpecifierTest, twoGetsReturnSameTypeObjectTest) {
  const LLVMFunctionType* functionType1 = mLLVMFunctionTypeSpecifier->getType(mContext);
  const LLVMFunctionType* functionType2 = mLLVMFunctionTypeSpecifier->getType(mContext);

  EXPECT_EQ(functionType1, functionType2);
}

TEST_F(LLVMFunctionTypeSpecifierTest, printToStreamTest) {
  stringstream stringStream;
  mLLVMFunctionTypeSpecifier->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::llvm::i8 (::llvm::i16, ::llvm::i64)", stringStream.str().c_str());
}
