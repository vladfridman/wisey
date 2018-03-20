//
//  TestFixedField.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link FixedField}
//

#include <gtest/gtest.h>

#include "TestFileSampleRunner.hpp"
#include "MockType.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct FixedFieldTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockType>* mType;
  string mName;
  
public:
  
  FixedFieldTest() :
  mType(new NiceMock<MockType>()),
  mName("mField") {
    ON_CALL(*mType, printToStream(_, _)).WillByDefault(Invoke(printType));
    EXPECT_CALL(*mType, die());
  }
  
  ~FixedFieldTest() {
    delete mType;
  }
  
  static void printType(IRGenerationContext& context, iostream& stream) {
    stream << "MObject*";
  }
};

TEST_F(FixedFieldTest, fieldCreationTest) {
  FixedField field(mType, mName);
  
  EXPECT_EQ(field.getType(), mType);
  EXPECT_STREQ(field.getName().c_str(), "mField");
  EXPECT_FALSE(field.isAssignable());
  
  EXPECT_TRUE(field.isFixed());
  EXPECT_FALSE(field.isInjected());
  EXPECT_FALSE(field.isReceived());
  EXPECT_FALSE(field.isState());
}

TEST_F(FixedFieldTest, elementTypeTest) {
  FixedField field(mType, mName);

  EXPECT_FALSE(field.isConstant());
  EXPECT_TRUE(field.isField());
  EXPECT_FALSE(field.isMethod());
  EXPECT_FALSE(field.isStaticMethod());
  EXPECT_FALSE(field.isMethodSignature());
  EXPECT_FALSE(field.isLLVMFunction());
}

TEST_F(FixedFieldTest, fieldPrintToStreamTest) {
  FixedField field(mType, mName);

  stringstream stringStream;
  field.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("  fixed MObject* mField;\n", stringStream.str().c_str());
}

TEST_F(TestFileSampleRunner, nodeWithFixedFieldSetterDeathRunTest) {
  expectFailCompile("tests/samples/test_node_with_fixed_field_setter.yz",
                    1,
                    "Error: Can not assign to field mYear");
}
