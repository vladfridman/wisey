//
//  TestLLVMPrimitiveTypeSpecifier.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/18/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMPrimitiveTypeSpecifier}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "wisey/LLVMPrimitiveTypeSpecifier.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct LLVMPrimitiveTypeSpecifierTest : public Test {
  IRGenerationContext mContext;
  
  LLVMPrimitiveTypeSpecifierTest() { }
};

TEST_F(LLVMPrimitiveTypeSpecifierTest, creationTest) {
  LLVMPrimitiveTypeSpecifier i1TypeSpecifier(LLVMPrimitiveTypes::I1);
  LLVMPrimitiveTypeSpecifier i8TypeSpecifier(LLVMPrimitiveTypes::I8);
  LLVMPrimitiveTypeSpecifier i16TypeSpecifier(LLVMPrimitiveTypes::I16);
  LLVMPrimitiveTypeSpecifier i32TypeSpecifier(LLVMPrimitiveTypes::I32);
  LLVMPrimitiveTypeSpecifier i64TypeSpecifier(LLVMPrimitiveTypes::I64);
  
  EXPECT_EQ(i1TypeSpecifier.getType(mContext), LLVMPrimitiveTypes::I1);
  EXPECT_EQ(i8TypeSpecifier.getType(mContext), LLVMPrimitiveTypes::I8);
  EXPECT_EQ(i16TypeSpecifier.getType(mContext), LLVMPrimitiveTypes::I16);
  EXPECT_EQ(i32TypeSpecifier.getType(mContext), LLVMPrimitiveTypes::I32);
  EXPECT_EQ(i64TypeSpecifier.getType(mContext), LLVMPrimitiveTypes::I64);
}

TEST_F(LLVMPrimitiveTypeSpecifierTest, printToStreamTest) {
  LLVMPrimitiveTypeSpecifier i1TypeSpecifier(LLVMPrimitiveTypes::I1);
  stringstream i1StringStream;
  i1TypeSpecifier.printToStream(mContext, i1StringStream);
  EXPECT_STREQ("::llvm::i1", i1StringStream.str().c_str());
  
  LLVMPrimitiveTypeSpecifier i8TypeSpecifier(LLVMPrimitiveTypes::I8);
  stringstream i8StringStream;
  i8TypeSpecifier.printToStream(mContext, i8StringStream);
  EXPECT_STREQ("::llvm::i8", i8StringStream.str().c_str());

  LLVMPrimitiveTypeSpecifier i16TypeSpecifier(LLVMPrimitiveTypes::I16);
  stringstream i16StringStream;
  i16TypeSpecifier.printToStream(mContext, i16StringStream);
  EXPECT_STREQ("::llvm::i16", i16StringStream.str().c_str());

  LLVMPrimitiveTypeSpecifier i32TypeSpecifier(LLVMPrimitiveTypes::I32);
  stringstream i32StringStream;
  i32TypeSpecifier.printToStream(mContext, i32StringStream);
  EXPECT_STREQ("::llvm::i32", i32StringStream.str().c_str());

  LLVMPrimitiveTypeSpecifier i64TypeSpecifier(LLVMPrimitiveTypes::I64);
  stringstream i64StringStream;
  i64TypeSpecifier.printToStream(mContext, i64StringStream);
  EXPECT_STREQ("::llvm::i64", i64StringStream.str().c_str());
}
