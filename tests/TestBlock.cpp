//
//  TestBlock.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/20/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Block}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockStatement.hpp"
#include "wisey/Block.hpp"
#include "wisey/IRGenerationContext.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;

struct BlockTest : public ::testing::Test {
  IRGenerationContext mContext;
  NiceMock<MockStatement>* mMockStatement1;
  NiceMock<MockStatement>* mMockStatement2;
  Block* mBlock;

  BlockTest() :
  mMockStatement1(new NiceMock<MockStatement>()),
  mMockStatement2(new NiceMock<MockStatement>()),
  mBlock(new Block(0)) {
  }
  
  ~BlockTest() {
    delete mMockStatement1;
    delete mMockStatement2;
  }
};

TEST_F(BlockTest, generateIRTest) {
  mBlock->getStatements().push_back(mMockStatement1);
  mBlock->getStatements().push_back(mMockStatement2);
  
  EXPECT_CALL(*mMockStatement1, generateIR(_));
  EXPECT_CALL(*mMockStatement2, generateIR(_));
  
  mBlock->generateIR(mContext);
}
