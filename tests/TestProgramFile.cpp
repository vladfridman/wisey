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

#include "MockObjectDefinitionStatement.hpp"
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
  NiceMock<MockObjectDefinitionStatement> mObjectDefinitionStatement;
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
  mProgramBlock->getObjectDefinitions().push_back(&mObjectDefinitionStatement);
  EXPECT_CALL(mObjectDefinitionStatement, prototypeObjects(_)).Times(1);
  
  mProgramFile->prototypeObjects(mContext);
}

TEST_F(ProgramFileTest, prototypeMethodsTest) {
  mProgramBlock->getObjectDefinitions().push_back(&mObjectDefinitionStatement);
  EXPECT_CALL(mObjectDefinitionStatement, prototypeMethods(_)).Times(1);
  
  mProgramFile->prototypeMethods(mContext);
}

TEST_F(ProgramFileTest, generateIRTest) {
  mProgramBlock->getStatements().push_back(&mStatement);
  EXPECT_CALL(mStatement, generateIR(_)).Times(1);
  
  mProgramFile->generateIR(mContext);
}
