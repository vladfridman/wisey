//
//  TestBlock.cpp
//  Yazyk
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
using namespace yazyk;

using ::testing::_;
using ::testing::NiceMock;

TEST(BlockTest, simpleTest) {
  NiceMock<MockStatement> mockStatement1;
  NiceMock<MockStatement> mockStatement2;
  IRGenerationContext context;
  Block block;
  block.getStatements().push_back(&mockStatement1);
  block.getStatements().push_back(&mockStatement2);
  
  EXPECT_CALL(mockStatement1, generateIR(_));
  EXPECT_CALL(mockStatement2, generateIR(_));
  
  block.generateIR(context);
}
