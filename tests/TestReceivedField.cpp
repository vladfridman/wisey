//
//  TestReceivedField.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ReceivedField}
//

#include <gtest/gtest.h>

#include "MockConcreteObjectType.hpp"
#include "MockType.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ReceivedField.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ReceivedFieldTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockType>* mType;
  NiceMock<MockConcreteObjectType>* mObject;
  string mName;
  ReceivedField* mField;
  
public:
  
  ReceivedFieldTest() :
  mType(new NiceMock<MockType>()),
  mObject(new NiceMock<MockConcreteObjectType>()),
  mName("mField") {
    TestPrefix::generateIR(mContext);
    
    ON_CALL(*mType, getTypeName()).WillByDefault(Return("MObject*"));
    EXPECT_CALL(*mType, die());
    
    mField = new ReceivedField(mType, mName, 7);
  }
  
  ~ReceivedFieldTest() {
    delete mType;
    delete mField;
    delete mObject;
  }
};

TEST_F(ReceivedFieldTest, fieldCreationTest) {
  EXPECT_EQ(mField->getType(), mType);
  EXPECT_STREQ(mField->getName().c_str(), "mField");
  
  EXPECT_FALSE(mField->isInjected());
  EXPECT_TRUE(mField->isReceived());
  EXPECT_FALSE(mField->isState());
}

TEST_F(ReceivedFieldTest, isAssignableTest) {
  ON_CALL(*mObject, isController()).WillByDefault(Return(true));
  EXPECT_TRUE(mField->isAssignable(mObject));

  ON_CALL(*mObject, isController()).WillByDefault(Return(false));
  EXPECT_FALSE(mField->isAssignable(mObject));
}

TEST_F(ReceivedFieldTest, elementTypeTest) {
  EXPECT_FALSE(mField->isConstant());
  EXPECT_TRUE(mField->isField());
  EXPECT_FALSE(mField->isMethod());
  EXPECT_FALSE(mField->isStaticMethod());
  EXPECT_FALSE(mField->isMethodSignature());
  EXPECT_FALSE(mField->isLLVMFunction());
}

TEST_F(ReceivedFieldTest, checkTypePrimitiveTypeTest) {
  ON_CALL(*mType, isPrimitive()).WillByDefault(Return(true));
  
  EXPECT_NO_THROW(mField->checkType(mContext));
}

TEST_F(ReceivedFieldTest, checkTypeModelTypeTest) {
  ON_CALL(*mType, isModel()).WillByDefault(Return(true));
  
  EXPECT_NO_THROW(mField->checkType(mContext));
}

TEST_F(ReceivedFieldTest, checkTypeInterfaceTypeTest) {
  ON_CALL(*mType, isInterface()).WillByDefault(Return(true));
  
  EXPECT_NO_THROW(mField->checkType(mContext));
}

TEST_F(ReceivedFieldTest, checkTypeArrayTypeTest) {
  ON_CALL(*mType, isArray()).WillByDefault(Return(true));
  ON_CALL(*mType, isImmutable()).WillByDefault(Return(true));
  ON_CALL(*mType, isOwner()).WillByDefault(Return(true));
  
  EXPECT_NO_THROW(mField->checkType(mContext));
}

TEST_F(ReceivedFieldTest, checkTypeNonImmutableTypeDeathTest) {
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mField->checkType(mContext));
  EXPECT_STREQ("/tmp/source.yz(7): Error: Model receive fields can only be of primitive, model or array type\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(ReceivedFieldTest, checkTypeNonImmutableArrayTypeDeathTest) {
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  ON_CALL(*mType, isArray()).WillByDefault(Return(true));
  
  EXPECT_ANY_THROW(mField->checkType(mContext));
  EXPECT_STREQ("/tmp/source.yz(7): Error: Model receive array fields can only be of immutable array type\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(ReceivedFieldTest, fieldPrintToStreamTest) {
  stringstream stringStream;
  mField->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("  receive MObject* mField;\n", stringStream.str().c_str());
}

TEST_F(TestFileRunner, modelImmutableArrayReferenceFieldRunTest) {
  runFile("tests/samples/test_model_immutable_array_reference_field.yz", 5);
}

TEST_F(TestFileRunner, nodeWithRecievedFieldSetterDeathRunTest) {
  expectFailCompile("tests/samples/test_node_with_received_field_setter.yz",
                    1,
                    "tests/samples/test_node_with_received_field_setter.yz\\(9\\): "
                    "Error: Can not assign to received field mYear");
}
