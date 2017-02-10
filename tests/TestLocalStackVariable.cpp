//
//  TestLocalStackVariable.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LocalStackVariable}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/LocalStackVariable.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

struct LocalStackVariableTest : public Test {
  IRGenerationContext mContext;
  BasicBlock* mBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  LocalStackVariableTest() {
    mBlock = BasicBlock::Create(mContext.getLLVMContext(), "entry");
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~LocalStackVariableTest() {
    delete mBlock;
    delete mStringStream;
  }
};

TEST_F(LocalStackVariableTest, GenerateAssignmentIRTest) {
  AllocaInst* alloc = new AllocaInst(Type::getInt32Ty(mContext.getLLVMContext()),
                                     "foo",
                                     mBlock);
  LocalStackVariable localStackVariable("foo", PrimitiveTypes::INT_TYPE, alloc);
  Value* assignValue = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 5);
  
  localStackVariable.generateAssignmentIR(mContext, assignValue);
  
  ASSERT_EQ(2ul, mBlock->size());
  BasicBlock::iterator iterator = mBlock->begin();
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %foo = alloca i32");
  mStringBuffer.clear();
  
  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  store i32 5, i32* %foo");
  mStringBuffer.clear();
}

TEST_F(LocalStackVariableTest, GenerateIdentifierIRTest) {
  AllocaInst* alloc = new AllocaInst(Type::getInt32Ty(mContext.getLLVMContext()),
                                     "foo",
                                     mBlock);
  LocalStackVariable localStackVariable("foo", PrimitiveTypes::INT_TYPE, alloc);
  
  localStackVariable.generateIdentifierIR(mContext, "bar");

  ASSERT_EQ(2ul, mBlock->size());
  
  BasicBlock::iterator iterator = mBlock->begin();
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %foo = alloca i32");
  mStringBuffer.clear();
  
  iterator++;
  *mStringStream << *iterator;
  EXPECT_STREQ(mStringStream->str().c_str(), "  %bar = load i32, i32* %foo");
  mStringBuffer.clear();
}

TEST_F(LocalStackVariableTest, TestFree) {
  LocalStackVariable localStackVariable("foo", PrimitiveTypes::INT_TYPE, NULL);

  localStackVariable.free(mBlock);
  
  ASSERT_EQ(mBlock->size(), 0u);
}
