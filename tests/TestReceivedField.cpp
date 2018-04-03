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

#include "TestFileRunner.hpp"
#include "MockType.hpp"
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
  string mName;
  
public:
  
  ReceivedFieldTest() :
  mType(new NiceMock<MockType>()),
  mName("mField") {
    ON_CALL(*mType, printToStream(_, _)).WillByDefault(Invoke(printType));
    EXPECT_CALL(*mType, die());
  }
  
  ~ReceivedFieldTest() {
    delete mType;
  }
  
  static void printType(IRGenerationContext& context, iostream& stream) {
    stream << "MObject*";
  }
};

TEST_F(ReceivedFieldTest, fieldCreationTest) {
  ReceivedField field(mType, mName);
  
  EXPECT_EQ(field.getType(), mType);
  EXPECT_STREQ(field.getName().c_str(), "mField");
  EXPECT_TRUE(field.isAssignable());
  
  EXPECT_FALSE(field.isFixed());
  EXPECT_FALSE(field.isInjected());
  EXPECT_TRUE(field.isReceived());
  EXPECT_FALSE(field.isState());
}

TEST_F(ReceivedFieldTest, elementTypeTest) {
  ReceivedField field(mType, mName);
  
  EXPECT_FALSE(field.isConstant());
  EXPECT_TRUE(field.isField());
  EXPECT_FALSE(field.isMethod());
  EXPECT_FALSE(field.isStaticMethod());
  EXPECT_FALSE(field.isMethodSignature());
  EXPECT_FALSE(field.isLLVMFunction());
}

TEST_F(ReceivedFieldTest, fieldPrintToStreamTest) {
  ReceivedField field(mType, mName);
  
  stringstream stringStream;
  field.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("  receive MObject* mField;\n", stringStream.str().c_str());
}
