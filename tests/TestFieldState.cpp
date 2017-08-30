//
//  TestFieldState.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link FieldState}
//

#include <gtest/gtest.h>

#include "MockExpression.hpp"
#include "MockType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::NiceMock;
using ::testing::Test;

struct FieldStateTest : public Test {
  NiceMock<MockType>* mType;
  NiceMock<MockExpression>* mExpression;
  string mName;
  unsigned long mIndex;
  ExpressionList mArguments;
  
public:
  
  FieldStateTest() :
  mType(new NiceMock<MockType>()),
  mExpression(new NiceMock<MockExpression>()),
  mName("mField"),
  mIndex(3u) {
    mArguments.push_back(mExpression);
  }
  
  ~FieldStateTest() {
  }
};

TEST_F(FieldStateTest, fieldStateObjectCreationTest) {
  FieldState field(mType, mName, mIndex, mArguments);
  
  EXPECT_EQ(field.getType(), mType);
  EXPECT_STREQ(field.getName().c_str(), "mField");
  EXPECT_EQ(field.getIndex(), mIndex);
  EXPECT_EQ(field.getArguments().size(), 1u);
  EXPECT_EQ(field.getArguments().at(0), mExpression);
}