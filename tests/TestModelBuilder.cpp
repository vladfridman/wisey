//
//  TestModelBuilder.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 1/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ModelBuilder}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/ModelBuilder.hpp"
#include "yazyk/ModelTypeSpecifier.hpp"

using ::testing::NiceMock;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace yazyk;

class MockExpression : public IExpression {
public:
  MOCK_CONST_METHOD1(generateIR, Value* (IRGenerationContext&));
};

struct ModelBuilderTest : Test {
  IRGenerationContext mContext;
  NiceMock<MockExpression> mFieldValue1;
  NiceMock<MockExpression> mFieldValue2;
  ModelTypeSpecifier mModelTypeSpecifier;
  
  ModelBuilderTest() : mModelTypeSpecifier(ModelTypeSpecifier("Shape")) { }
};

TEST_F(ModelBuilderTest, ValidModelBuilderArgumentsTest) {
  string argumentSpecifier1("withWidth");
  ModelBuilderArgument *argument1 = new ModelBuilderArgument(argumentSpecifier1, mFieldValue1);
  string argumentSpecifier2("withHeight");
  ModelBuilderArgument *argument2 = new ModelBuilderArgument(argumentSpecifier2, mFieldValue2);
  ModelBuilderArgumentList* argumentList = new ModelBuilderArgumentList();
  argumentList->push_back(argument1);
  argumentList->push_back(argument2);
  
  ModelBuilder modelBuilder(mModelTypeSpecifier, argumentList);

  EXPECT_TRUE(modelBuilder.generateIR(mContext) == NULL);
}

TEST_F(ModelBuilderTest, InvalidModelBuilderArgumentsDeathTest) {
  string argumentSpecifier1("width");
  ModelBuilderArgument *argument1 = new ModelBuilderArgument(argumentSpecifier1, mFieldValue1);
  string argumentSpecifier2("withHeight");
  ModelBuilderArgument *argument2 = new ModelBuilderArgument(argumentSpecifier2, mFieldValue2);
  ModelBuilderArgumentList* argumentList = new ModelBuilderArgumentList();
  argumentList->push_back(argument1);
  argumentList->push_back(argument2);
  
  ModelBuilder modelBuilder(mModelTypeSpecifier, argumentList);
  
  const char *expected =
    "Error: Model builder argument should start with 'with'. e.g. .withField\\(value\\)."
    "\nError: Some arguments for the model builder are not well formed";

  EXPECT_EXIT(modelBuilder.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              expected);
}