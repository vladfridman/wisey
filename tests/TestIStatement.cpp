//
//  TestIStatement.cpp
//  runtests
//
//  Created by Vladimir Fridman on 5/4/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IStatement}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Instructions.h>

#include "MockType.hpp"
#include "TestPrefix.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IStatement.hpp"

using namespace llvm;
using namespace wisey;

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct IStatementTest : public Test {
  
  IRGenerationContext mContext;
  BasicBlock* mBasicBlock;
  
  IStatementTest() {
    TestPrefix::generateIR(mContext);
    
    mBasicBlock = BasicBlock::Create(mContext.getLLVMContext());
    mContext.getScopes().pushScope();
    mContext.setBasicBlock(mBasicBlock);
  }
  
  ~IStatementTest() {
    delete mBasicBlock;
  }
};

TEST_F(IStatementTest, checkUnreachableTest) {
  Value* value = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 1);
  IRWriter::createReturnInst(mContext, value);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(IStatement::checkUnreachable(mContext, 1));
  EXPECT_STREQ("/tmp/source.yz(1): Error: Statement unreachable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}
