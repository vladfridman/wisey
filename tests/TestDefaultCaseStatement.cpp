//
//  TestDefaultCaseStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/2/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link DefaultCaseStatement}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MockStatement.hpp"
#include "wisey/Block.hpp"
#include "wisey/DefaultCaseStatement.hpp"
#include "wisey/IRGenerationContext.hpp"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace wisey;

struct DefaultCaseStatementTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockStatement>* mMockStatement1;
  NiceMock<MockStatement>* mMockStatement2;
  Block* mBlock;
  
  DefaultCaseStatementTest() :
  mMockStatement1(new NiceMock<MockStatement>()),
  mMockStatement2(new NiceMock<MockStatement>()),
  mBlock(new Block(0)) {
  }
};

TEST_F(DefaultCaseStatementTest, generateIRTest) {
  mBlock->getStatements().push_back(mMockStatement1);
  mBlock->getStatements().push_back(mMockStatement2);
  
  DefaultCaseStatement defaultCaseStatement(mBlock, 0);
  
  EXPECT_CALL(*mMockStatement1, generateIR(_));
  EXPECT_CALL(*mMockStatement2, generateIR(_));
  
  defaultCaseStatement.generateIR(mContext);
}
