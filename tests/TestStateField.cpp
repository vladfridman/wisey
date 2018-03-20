//
//  TestStateField.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link StateField}
//

#include <gtest/gtest.h>

#include "TestFileSampleRunner.hpp"
#include "MockType.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/StateField.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct StateFieldTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockType>* mType;
  string mName;
  
public:
  
  StateFieldTest() :
  mType(new NiceMock<MockType>()),
  mName("mField") {
    ON_CALL(*mType, printToStream(_, _)).WillByDefault(Invoke(printType));
    EXPECT_CALL(*mType, die());
  }
  
  ~StateFieldTest() {
    delete mType;
  }
  
  static void printType(IRGenerationContext& context, iostream& stream) {
    stream << "MObject*";
  }
};

TEST_F(StateFieldTest, fieldCreationTest) {
  StateField field(mType, mName);
  
  EXPECT_EQ(field.getType(), mType);
  EXPECT_STREQ(field.getName().c_str(), "mField");
  EXPECT_TRUE(field.isAssignable());
  
  EXPECT_FALSE(field.isFixed());
  EXPECT_FALSE(field.isInjected());
  EXPECT_FALSE(field.isReceived());
  EXPECT_TRUE(field.isState());
}

TEST_F(StateFieldTest, elementTypeTest) {
  StateField field(mType, mName);
  
  EXPECT_FALSE(field.isConstant());
  EXPECT_TRUE(field.isField());
  EXPECT_FALSE(field.isMethod());
  EXPECT_FALSE(field.isStaticMethod());
  EXPECT_FALSE(field.isMethodSignature());
  EXPECT_FALSE(field.isLLVMFunction());
}

TEST_F(StateFieldTest, fieldPrintToStreamTest) {
  StateField field(mType, mName);

  stringstream stringStream;
  field.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("  state MObject* mField;\n", stringStream.str().c_str());
}

