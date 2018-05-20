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
  
  EXPECT_FALSE(mField->isFixed());
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

TEST_F(ReceivedFieldTest, fieldPrintToStreamTest) {
  stringstream stringStream;
  mField->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("  receive MObject* mField;\n", stringStream.str().c_str());
}
