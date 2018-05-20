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

#include "MockConcreteObjectType.hpp"
#include "MockType.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
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
  NiceMock<MockConcreteObjectType>* mObject;
  string mName;
  FixedField* mField;
  
public:
  
  FixedFieldTest() :
  mType(new NiceMock<MockType>()),
  mObject(new NiceMock<MockConcreteObjectType>()),
  mName("mField") {
    TestPrefix::generateIR(mContext);
    
    ON_CALL(*mType, getTypeName()).WillByDefault(Return("MObject*"));
    EXPECT_CALL(*mType, die());
    
    mField = new FixedField(mType, mName, 3);
  }
  
  ~FixedFieldTest() {
    delete mType;
    delete mField;
    delete mObject;
  }
};

TEST_F(FixedFieldTest, fieldCreationTest) {
  EXPECT_EQ(mField->getType(), mType);
  EXPECT_STREQ(mField->getName().c_str(), "mField");
  EXPECT_FALSE(mField->isAssignable(mObject));
  
  EXPECT_TRUE(mField->isFixed());
  EXPECT_FALSE(mField->isInjected());
  EXPECT_FALSE(mField->isReceived());
  EXPECT_FALSE(mField->isState());
}

TEST_F(FixedFieldTest, elementTypeTest) {
  EXPECT_FALSE(mField->isConstant());
  EXPECT_TRUE(mField->isField());
  EXPECT_FALSE(mField->isMethod());
  EXPECT_FALSE(mField->isStaticMethod());
  EXPECT_FALSE(mField->isMethodSignature());
  EXPECT_FALSE(mField->isLLVMFunction());
}

TEST_F(FixedFieldTest, fieldPrintToStreamTest) {
  stringstream stringStream;
  mField->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("  fixed MObject* mField;\n", stringStream.str().c_str());
}

TEST_F(FixedFieldTest, checkTypePrimitiveTypeTest) {
  ON_CALL(*mType, isPrimitive()).WillByDefault(Return(true));
  
  EXPECT_NO_THROW(mField->checkType(mContext));
}

TEST_F(FixedFieldTest, checkTypeModelTypeTest) {
  ON_CALL(*mType, isModel()).WillByDefault(Return(true));
  
  EXPECT_NO_THROW(mField->checkType(mContext));
}

TEST_F(FixedFieldTest, checkTypeInterfaceTypeTest) {
  ON_CALL(*mType, isInterface()).WillByDefault(Return(true));
  
  EXPECT_NO_THROW(mField->checkType(mContext));
}

TEST_F(FixedFieldTest, checkTypeArrayTypeTest) {
  ON_CALL(*mType, isArray()).WillByDefault(Return(true));
  ON_CALL(*mType, isImmutable()).WillByDefault(Return(true));
  ON_CALL(*mType, isOwner()).WillByDefault(Return(true));

  EXPECT_NO_THROW(mField->checkType(mContext));
}

TEST_F(FixedFieldTest, checkTypeNonImmutableTypeDeathTest) {
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mField->checkType(mContext));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Model fixed fields can only be of primitive, model or array type\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(FixedFieldTest, checkTypeNonImmutableArrayTypeDeathTest) {
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  ON_CALL(*mType, isArray()).WillByDefault(Return(true));

  EXPECT_ANY_THROW(mField->checkType(mContext));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Model fixed array fields can only be of immutable array type\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, modelImmutableArrayReferenceFieldRunTest) {
  runFile("tests/samples/test_model_immutable_array_reference_field.yz", "5");
}

TEST_F(TestFileRunner, nodeWithFixedFieldSetterDeathRunTest) {
  expectFailCompile("tests/samples/test_node_with_fixed_field_setter.yz",
                    1,
                    "tests/samples/test_node_with_fixed_field_setter.yz\\(9\\): "
                    "Error: Can not assign to received field mYear");
}
