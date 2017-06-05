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
  NiceMock<MockGlobalStatement>* mMockGlobalStatement;
  string mPackage;
  ProgramBlock* mProgramBlock;
  
  ProgramFileTest() : mMockGlobalStatement(new NiceMock<MockGlobalStatement>()) {
    mPackage = "systems.vos.wisey.compiler.tests";
    mProgramBlock = new ProgramBlock();
  }
};

TEST_F(ProgramFileTest, prototypeObjectTest) {
  ProgramFile programFile(mPackage, mProgramBlock);
  mProgramBlock->getGlobalStatements().push_back(mMockGlobalStatement);
  EXPECT_CALL(*mMockGlobalStatement, prototypeObjects(_)).Times(1);
  
  programFile.prototypeObjects(mContext);
}

TEST_F(ProgramFileTest, prototypeMethodsTest) {
  ProgramFile programFile(mPackage, mProgramBlock);
  mProgramBlock->getGlobalStatements().push_back(mMockGlobalStatement);
  EXPECT_CALL(*mMockGlobalStatement, prototypeMethods(_)).Times(1);
  
  programFile.prototypeMethods(mContext);
}

TEST_F(ProgramFileTest, generateIRTest) {
  ProgramFile programFile(mPackage, mProgramBlock);
  mProgramBlock->getGlobalStatements().push_back(mMockGlobalStatement);
  EXPECT_CALL(*mMockGlobalStatement, generateIR(_)).Times(1);
  
  programFile.generateIR(mContext);
}
