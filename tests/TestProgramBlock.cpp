//
//  TestProgramBlock.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/29/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ProgramBlock}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockObjectDefinitionStatement.hpp"
#include "MockStatement.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/ProgramBlock.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;

struct ProgramBlockTest : public ::testing::Test {
  IRGenerationContext mContext;
  NiceMock<MockStatement> mMockStatement1;
  NiceMock<MockStatement> mMockStatement2;
  NiceMock<MockObjectDefinitionStatement> mMockObjectDefinitionStatement1;
  NiceMock<MockObjectDefinitionStatement> mMockObjectDefinitionStatement2;
  ProgramBlock mProgramBlock;
  
  ProgramBlockTest() { }
};

TEST_F(ProgramBlockTest, prototypeObjectsTest) {
  mProgramBlock.getObjectDefinitions().push_back(&mMockObjectDefinitionStatement1);
  mProgramBlock.getObjectDefinitions().push_back(&mMockObjectDefinitionStatement2);
  
  EXPECT_CALL(mMockObjectDefinitionStatement1, prototypeObjects(_));
  EXPECT_CALL(mMockObjectDefinitionStatement2, prototypeObjects(_));
  
  mProgramBlock.prototypeObjects(mContext);
}

TEST_F(ProgramBlockTest, prototypeMethodsTest) {
  mProgramBlock.getObjectDefinitions().push_back(&mMockObjectDefinitionStatement1);
  mProgramBlock.getObjectDefinitions().push_back(&mMockObjectDefinitionStatement2);
  
  EXPECT_CALL(mMockObjectDefinitionStatement1, prototypeMethods(_));
  EXPECT_CALL(mMockObjectDefinitionStatement2, prototypeMethods(_));
  
  mProgramBlock.prototypeMethods(mContext);
}

TEST_F(ProgramBlockTest, generateIRTest) {
  mProgramBlock.getStatements().push_back(&mMockStatement1);
  mProgramBlock.getStatements().push_back(&mMockStatement2);
  
  EXPECT_CALL(mMockStatement1, generateIR(_));
  EXPECT_CALL(mMockStatement2, generateIR(_));
  
  mProgramBlock.generateIR(mContext);
}
