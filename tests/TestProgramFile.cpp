//
//  TestProgramFile.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 5/17/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ProgramFile}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>

#include "MockStatement.hpp"
#include "yazyk/ProgramFile.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Test;

struct ProgramFileTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockStatement> mStatement;
  Block* mBlock;
  ProgramFile* mProgramFile;
  
  ProgramFileTest() {
    mBlock = new Block();
    mProgramFile = new ProgramFile(mBlock);
  }
  
  ~ProgramFileTest() {
    delete mProgramFile;
  }
};

TEST_F(ProgramFileTest, generateIRTest) {
  mBlock->getStatements().push_back(&mStatement);
  EXPECT_CALL(mStatement, generateIR(_)).Times(1);
  
  mProgramFile->generateIR(mContext);
}
