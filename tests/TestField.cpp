//
//  FieldTest.cpp
//  runtests
//
//  Created by Vladimir Fridman on 9/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Field}
//

#include <gtest/gtest.h>

#include "TestFileSampleRunner.hpp"
#include "MockExpression.hpp"
#include "MockType.hpp"
#include "wisey/Field.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct FieldTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockType>* mType;
  NiceMock<MockExpression>* mExpression;
  string mName;
  ExpressionList mArguments;
  
public:
  
  FieldTest() :
  mType(new NiceMock<MockType>()),
  mExpression(new NiceMock<MockExpression>()),
  mName("mField") {
    mArguments.push_back(mExpression);
    ON_CALL(*mType, getName()).WillByDefault(Return("MObject*"));
    ON_CALL(*mExpression, printToStream(_, _)).WillByDefault(Invoke(printExpression));
  }
  
  ~FieldTest() {
    delete mType;
    delete mExpression;
  }

  static void printExpression(IRGenerationContext& context, iostream& stream) {
    stream << "expression";
  }
};

TEST_F(FieldTest, injectedFieldObjectCreationTest) {
  Field field(FieldKind::INJECTED_FIELD, mType, mName, mArguments);
  
  EXPECT_EQ(field.getType(), mType);
  EXPECT_STREQ(field.getName().c_str(), "mField");
  EXPECT_EQ(field.getArguments().size(), 1u);
  EXPECT_EQ(field.getArguments().at(0), mExpression);
  EXPECT_FALSE(field.isAssignable());
  EXPECT_EQ(field.getFieldKind(), FieldKind::INJECTED_FIELD);
}

TEST_F(FieldTest, injectedFieldPrintToStreamTest) {
  Field field(FieldKind::INJECTED_FIELD, mType, mName, mArguments);
  
  stringstream stringStream;
  field.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("  inject MObject* mField(expression);\n", stringStream.str().c_str());
}

TEST_F(FieldTest, receivedFieldPrintToStreamTest) {
  ExpressionList arguments;
  Field field(FieldKind::RECEIVED_FIELD, PrimitiveTypes::DOUBLE_TYPE, mName, arguments);

  stringstream stringStream;
  field.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("  receive double mField;\n", stringStream.str().c_str());
}

TEST_F(FieldTest, stateFieldPrintToStreamTest) {
  ExpressionList arguments;
  Field field(FieldKind::STATE_FIELD, PrimitiveTypes::DOUBLE_TYPE, mName, arguments);

  stringstream stringStream;
  field.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("  state double mField;\n", stringStream.str().c_str());
}

TEST_F(FieldTest, fixedFieldPrintToStreamTest) {
  ExpressionList arguments;
  Field field(FieldKind::FIXED_FIELD, PrimitiveTypes::DOUBLE_TYPE, mName, arguments);

  stringstream stringStream;
  field.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("  fixed double mField;\n", stringStream.str().c_str());
}

TEST_F(TestFileSampleRunner, nodeWithFixedFieldSetterDeathRunTest) {
  expectFailCompile("tests/samples/test_node_with_fixed_field_setter.yz",
                    1,
                    "Error: Can not assign to field mYear");
}
