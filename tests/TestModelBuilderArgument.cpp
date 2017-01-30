//
//  TestModelBuilderArgument.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ModelBuilderArgument}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/ModelBuilderArgument.hpp"

using ::testing::NiceMock;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace yazyk;

class MockExpression : public IExpression {
public:
  MOCK_CONST_METHOD1(generateIR, Value* (IRGenerationContext&));
};

struct ModelBuilderArgumentTest : Test {
  NiceMock<MockExpression> mFieldValue;
};

TEST_F(ModelBuilderArgumentTest, TestValidModelBuilderArgument) {
  string argumentSpecifier("withFieldA");
  ModelBuilderArgument argument(argumentSpecifier, mFieldValue);
  
  EXPECT_TRUE(argument.checkArgument());
}

TEST_F(ModelBuilderArgumentTest, TestInalidModelBuilderArgument) {
  string argumentSpecifier("fieldA");
  ModelBuilderArgument argument(argumentSpecifier, mFieldValue);
  
  stringstream errorBuffer;
  streambuf *streamBuffer = std::cerr.rdbuf();
  cerr.rdbuf(errorBuffer.rdbuf());
  
  EXPECT_FALSE(argument.checkArgument());
  EXPECT_STREQ("Error: Model builder argument should start with 'with'. e.g. .withField(value).\n",
               errorBuffer.str().c_str());

  cerr.rdbuf(streamBuffer);
}
