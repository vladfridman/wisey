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

#include "MockGlobalStatement.hpp"
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
  NiceMock<MockGlobalStatement> mMockGlobalStatement;
  string mPackage;
  ProgramBlock* mProgramBlock;
  ProgramFile* mProgramFile;
  
  ProgramFileTest() {
    mPackage = "systems.vos.wisey.compiler.tests";
    mProgramBlock = new ProgramBlock();
    mProgramFile = new ProgramFile(mPackage, mProgramBlock);
  }
  
  ~ProgramFileTest() {
    delete mProgramFile;
  }
};

TEST_F(ProgramFileTest, prototypeObjectTest) {
  mProgramBlock->getGlobalStatements().push_back(&mMockGlobalStatement);
  EXPECT_CALL(mMockGlobalStatement, prototypeObjects(_)).Times(1);
  
  mProgramFile->prototypeObjects(mContext);
}

TEST_F(ProgramFileTest, prototypeMethodsTest) {
  mProgramBlock->getGlobalStatements().push_back(&mMockGlobalStatement);
  EXPECT_CALL(mMockGlobalStatement, prototypeMethods(_)).Times(1);
  
  mProgramFile->prototypeMethods(mContext);
}

TEST_F(ProgramFileTest, generateIRTest) {
  mProgramBlock->getGlobalStatements().push_back(&mMockGlobalStatement);
  EXPECT_CALL(mMockGlobalStatement, generateIR(_)).Times(1);
  
  mProgramFile->generateIR(mContext);
}
