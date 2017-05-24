//
//  TestProgramFile.cpp
//  Wisey
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
#include "wisey/ProgramFile.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Test;

struct ProgramFileTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockStatement> mStatement;
  string mPackage;
  Block* mBlock;
  ProgramFile* mProgramFile;
  
  ProgramFileTest() {
    mPackage = "systems.vos.wisey.compiler.tests";
    mBlock = new Block();
    mProgramFile = new ProgramFile(mPackage, mBlock);
  }
  
  ~ProgramFileTest() {
    delete mProgramFile;
  }
};

TEST_F(ProgramFileTest, prototypeTest) {
  mBlock->getStatements().push_back(&mStatement);
  EXPECT_CALL(mStatement, prototype(_)).Times(1);
  
  mProgramFile->prototype(mContext);
}

TEST_F(ProgramFileTest, generateIRTest) {
  mBlock->getStatements().push_back(&mStatement);
  EXPECT_CALL(mStatement, generateIR(_)).Times(1);
  
  mProgramFile->generateIR(mContext);
}
