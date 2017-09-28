//
//  TestFieldFixed.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/5/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link FieldFixed}
//

#include <gtest/gtest.h>

#include "TestFileSampleRunner.hpp"
#include "MockExpression.hpp"
#include "MockType.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::NiceMock;
using ::testing::Test;

struct FieldFixedTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockType>* mType;
  NiceMock<MockExpression>* mExpression;
  string mName;
  unsigned long mIndex;
  ExpressionList mArguments;
  
public:
  
  FieldFixedTest() :
  mType(new NiceMock<MockType>()),
  mExpression(new NiceMock<MockExpression>()),
  mName("mField"),
  mIndex(3u) {
    mArguments.push_back(mExpression);
  }
  
  ~FieldFixedTest() {
  }
};

TEST_F(FieldFixedTest, fieldFixedObjectCreationTest) {
  FieldFixed field(mType, mName, mIndex, mArguments);
  
  EXPECT_EQ(field.getType(), mType);
  EXPECT_STREQ(field.getName().c_str(), "mField");
  EXPECT_EQ(field.getIndex(), mIndex);
  EXPECT_EQ(field.getArguments().size(), 1u);
  EXPECT_EQ(field.getArguments().at(0), mExpression);
  EXPECT_FALSE(field.isAssignable());
  EXPECT_EQ(field.getFieldKind(), FieldKind::FIXED_FIELD);
}

TEST_F(FieldFixedTest, printToStreamTest) {
  FieldFixed field(PrimitiveTypes::DOUBLE_TYPE, mName, mIndex, mArguments);
  
  stringstream stringStream;
  field.printToStream(mContext, stringStream);

  EXPECT_STREQ("  double mField;\n", stringStream.str().c_str());
}

TEST_F(TestFileSampleRunner, nodeWithFixedFieldSetterDeathRunTest) {
  expectFailCompile("tests/samples/test_node_with_fixed_field_setter.yz",
                    1,
                    "Error: Can not assign to field mYear");
}
