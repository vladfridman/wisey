//
//  TestLLVMStructSpecifier.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/18/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMStructSpecifier}
//

#include <gtest/gtest.h>

#include "wisey/LLVMStructType.hpp"
#include "wisey/LLVMStructSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

struct LLVMStructSpecifierTest : public ::testing::Test {
  IRGenerationContext mContext;
  LLVMStructType* mLLVMStructType;
  
  LLVMStructSpecifierTest() {
    StructType* structType = StructType::create(mContext.getLLVMContext(), "mystruct");
    mLLVMStructType = new LLVMStructType(structType);
    mContext.addLLVMStructType(mLLVMStructType);
  }
};

TEST_F(LLVMStructSpecifierTest, createTest) {
  LLVMStructSpecifier llvmStructSpecifier("mystruct");
  
  EXPECT_EQ(llvmStructSpecifier.getType(mContext), mLLVMStructType);
}

TEST_F(LLVMStructSpecifierTest, printToStreamTest) {
  LLVMStructSpecifier llvmStructSpecifier("mystruct");

  stringstream stringStream;
  llvmStructSpecifier.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::llvm::struct::mystruct", stringStream.str().c_str());
}
