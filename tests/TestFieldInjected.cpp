//
//  TestFieldInjected.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link FieldInjected}
//

#include <gtest/gtest.h>

#include "MockExpression.hpp"
#include "MockType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct FieldInjectedTest : public Test {
  NiceMock<MockType>* mType;
  NiceMock<MockExpression>* mExpression;
  string mName;
  unsigned long mIndex;
  ExpressionList mArguments;
  
public:
  
  FieldInjectedTest() :
  mType(new NiceMock<MockType>()),
  mExpression(new NiceMock<MockExpression>()),
  mName("mField"),
  mIndex(3u) {
    mArguments.push_back(mExpression);
    ON_CALL(*mType, getName()).WillByDefault(Return("MObject*"));
    ON_CALL(*mExpression, printToStream(_)).WillByDefault(Invoke(printExpression));
  }
  
  ~FieldInjectedTest() {
    delete mType;
    delete mExpression;
  }
  
  static void printExpression(iostream& stream) {
    stream << "expression";
  }
};

TEST_F(FieldInjectedTest, fieldInjectedObjectCreationTest) {
  FieldInjected field(mType, mName, mIndex, mArguments);
  
  EXPECT_EQ(field.getType(), mType);
  EXPECT_STREQ(field.getName().c_str(), "mField");
  EXPECT_EQ(field.getIndex(), mIndex);
  EXPECT_EQ(field.getArguments().size(), 1u);
  EXPECT_EQ(field.getArguments().at(0), mExpression);
  EXPECT_FALSE(field.isAssignable());
}

TEST_F(FieldInjectedTest, extractHeaderTest) {
  FieldInjected field(mType, mName, mIndex, mArguments);
  
  stringstream stringStream;
  field.extractHeader(stringStream);
  
  EXPECT_STREQ("  inject MObject* mField(expression);\n", stringStream.str().c_str());
}
