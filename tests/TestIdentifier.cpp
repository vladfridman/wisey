//
//  TestIdentifier.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/17/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Identifier}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "yazyk/Identifier.hpp"
#include "yazyk/IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

struct IdentifierTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext = mContext.getLLVMContext();
  BasicBlock* mBlock = BasicBlock::Create(mLLVMContext, "entry");
  string mStringBuffer;
  raw_string_ostream* mStringStream;

  IdentifierTest() {
    mContext.pushBlock(mBlock);
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~IdentifierTest() {
    delete mBlock;
    delete mStringStream;
  }
};

TEST_F(IdentifierTest, UndeclaredVariableDeathTest) {
  Identifier identifier("foo", "bar");

  EXPECT_EXIT(identifier.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Undeclared variable foo");
}

TEST_F(IdentifierTest, VariableIdentifierTest) {
  string name = "foo";
  AllocaInst* alloc = new AllocaInst(Type::getInt32Ty(mLLVMContext),
                                     name,
                                     mBlock);
  mContext.locals()[name] = alloc;
  Identifier identifier(name, "bar");
  identifier.generateIR(mContext);
  
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
