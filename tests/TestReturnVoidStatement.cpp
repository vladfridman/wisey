//
//  TestReturnVoidStatement.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ReturnVoidStatement}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/ReturnVoidStatement.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

TEST(ReturnVoidStatementTest, returnVoidTest) {
  IRGenerationContext context;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  mStringStream = new raw_string_ostream(mStringBuffer);
  BasicBlock *basicBlock = BasicBlock::Create(context.getLLVMContext(), "entry");
  context.setBasicBlock(basicBlock);
  context.getScopes().pushScope();
  
  ReturnVoidStatement returnVoidStatement;
  returnVoidStatement.generateIR(context);
  
  ASSERT_EQ(1ul, basicBlock->size());
  *mStringStream << basicBlock->front();
  ASSERT_STREQ(mStringStream->str().c_str(), "  ret void");
}
