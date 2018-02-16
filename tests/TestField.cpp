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
  InjectionArgumentList mInjectionArgumentList;
  InjectionArgument* mInjectionArgument;
  
public:
  
  FieldTest() :
  mType(new NiceMock<MockType>()),
  mExpression(new NiceMock<MockExpression>()),
  mName("mField") {
    mInjectionArgument = new InjectionArgument("withFoo", mExpression);
    mInjectionArgumentList.push_back(mInjectionArgument);
    ON_CALL(*mType, printToStream(_, _)).WillByDefault(Invoke(printType));
    ON_CALL(*mExpression, printToStream(_, _)).WillByDefault(Invoke(printExpression));
  }
  
  ~FieldTest() {
    delete mType;
    delete mExpression;
  }

  static void printType(IRGenerationContext& context, iostream& stream) {
    stream << "MObject*";
  }

  static void printExpression(IRGenerationContext& context, iostream& stream) {
    stream << "expression";
  }
};

TEST_F(FieldTest, injectedFieldObjectCreationTest) {
  Field field(FieldKind::INJECTED_FIELD, mType, mType, mName, mInjectionArgumentList);
  
  EXPECT_EQ(field.getType(), mType);
  EXPECT_EQ(field.getInjectedType(), mType);
  EXPECT_STREQ(field.getName().c_str(), "mField");
  EXPECT_EQ(field.getInjectionArguments().size(), 1u);
  EXPECT_EQ(field.getInjectionArguments().at(0), mInjectionArgument);
  EXPECT_TRUE(field.isAssignable());
  EXPECT_EQ(field.getFieldKind(), FieldKind::INJECTED_FIELD);
}

TEST_F(FieldTest, injectedFieldPrintToStreamTest) {
  Field field(FieldKind::INJECTED_FIELD, mType, mType, mName, mInjectionArgumentList);
  
  stringstream stringStream;
  field.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("  inject MObject* mField.withFoo(expression);\n", stringStream.str().c_str());
}

TEST_F(FieldTest, receivedFieldPrintToStreamTest) {
  InjectionArgumentList arguments;
  Field field(FieldKind::RECEIVED_FIELD, PrimitiveTypes::DOUBLE_TYPE, NULL, mName, arguments);

  stringstream stringStream;
  field.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("  receive double mField;\n", stringStream.str().c_str());
}

TEST_F(FieldTest, stateFieldPrintToStreamTest) {
  InjectionArgumentList arguments;
  Field field(FieldKind::STATE_FIELD, PrimitiveTypes::DOUBLE_TYPE, NULL, mName, arguments);

  stringstream stringStream;
  field.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("  state double mField;\n", stringStream.str().c_str());
}

TEST_F(FieldTest, fixedFieldPrintToStreamTest) {
  InjectionArgumentList arguments;
  Field field(FieldKind::FIXED_FIELD, PrimitiveTypes::DOUBLE_TYPE, NULL, mName, arguments);

  stringstream stringStream;
  field.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("  fixed double mField;\n", stringStream.str().c_str());
}

TEST_F(TestFileSampleRunner, nodeWithFixedFieldSetterDeathRunTest) {
  expectFailCompile("tests/samples/test_node_with_fixed_field_setter.yz",
                    1,
                    "Error: Can not assign to field mYear");
}
