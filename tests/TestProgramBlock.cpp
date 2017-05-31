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

#include "MockStatement.hpp"
#include "MockGlobalStatement.hpp"
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
  NiceMock<MockGlobalStatement> mMockGlobalStatement1;
  NiceMock<MockGlobalStatement> mMockGlobalStatement2;
  ProgramBlock mProgramBlock;
  
  ProgramBlockTest() { }
};

TEST_F(ProgramBlockTest, prototypeObjectsTest) {
  mProgramBlock.getGlobalStatements().push_back(&mMockGlobalStatement1);
  mProgramBlock.getGlobalStatements().push_back(&mMockGlobalStatement2);
  
  EXPECT_CALL(mMockGlobalStatement1, prototypeObjects(_));
  EXPECT_CALL(mMockGlobalStatement2, prototypeObjects(_));
  
  mProgramBlock.prototypeObjects(mContext);
}

TEST_F(ProgramBlockTest, prototypeMethodsTest) {
  mProgramBlock.getGlobalStatements().push_back(&mMockGlobalStatement1);
  mProgramBlock.getGlobalStatements().push_back(&mMockGlobalStatement2);
  
  EXPECT_CALL(mMockGlobalStatement1, prototypeMethods(_));
  EXPECT_CALL(mMockGlobalStatement2, prototypeMethods(_));
  
  mProgramBlock.prototypeMethods(mContext);
}

TEST_F(ProgramBlockTest, generateIRTest) {
  mProgramBlock.getStatements().push_back(&mMockStatement1);
  mProgramBlock.getStatements().push_back(&mMockStatement2);
  
  EXPECT_CALL(mMockStatement1, generateIR(_));
  EXPECT_CALL(mMockStatement2, generateIR(_));
  
  mProgramBlock.generateIR(mContext);
}
