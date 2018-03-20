//
//  TestInjectedField.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link InjectedField}
//

#include <gtest/gtest.h>

#include "TestFileSampleRunner.hpp"
#include "MockExpression.hpp"
#include "MockType.hpp"
#include "wisey/InjectedField.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct InjectedFieldTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockType>* mType;
  NiceMock<MockType>* mInjectedType;
  NiceMock<MockExpression>* mExpression;
  string mName;
  InjectionArgumentList mInjectionArgumentList;
  InjectionArgument* mInjectionArgument;
  
public:
  
  InjectedFieldTest() :
  mType(new NiceMock<MockType>()),
  mInjectedType(new NiceMock<MockType>()),
  mExpression(new NiceMock<MockExpression>()),
  mName("mField") {
    mInjectionArgument = new InjectionArgument("withFoo", mExpression);
    mInjectionArgumentList.push_back(mInjectionArgument);
    ON_CALL(*mType, printToStream(_, _)).WillByDefault(Invoke(printType));
    ON_CALL(*mInjectedType, printToStream(_, _)).WillByDefault(Invoke(printInjectedType));
    ON_CALL(*mExpression, printToStream(_, _)).WillByDefault(Invoke(printExpression));
    EXPECT_CALL(*mType, die());
    EXPECT_CALL(*mInjectedType, die());
  }
  
  ~InjectedFieldTest() {
    delete mType;
    delete mExpression;
    delete mInjectedType;
  }
  
  static void printType(IRGenerationContext& context, iostream& stream) {
    stream << "MObject*";
  }
  
  static void printInjectedType(IRGenerationContext& context, iostream& stream) {
    stream << "MInjectedObject*";
  }
  
  static void printExpression(IRGenerationContext& context, iostream& stream) {
    stream << "expression";
  }
};

TEST_F(InjectedFieldTest, fieldCreationTest) {
  InjectedField field(mType, mInjectedType, mName, mInjectionArgumentList);
  
  EXPECT_EQ(field.getType(), mType);
  EXPECT_EQ(field.getInjectedType(), mInjectedType);
  EXPECT_STREQ(field.getName().c_str(), "mField");
  EXPECT_EQ(field.getInjectionArguments().size(), 1u);
  EXPECT_EQ(field.getInjectionArguments().at(0), mInjectionArgument);
  EXPECT_TRUE(field.isAssignable());

  EXPECT_FALSE(field.isFixed());
  EXPECT_TRUE(field.isInjected());
  EXPECT_FALSE(field.isReceived());
  EXPECT_FALSE(field.isState());
}

TEST_F(InjectedFieldTest, elementTypeTest) {
  InjectedField field(mType, mInjectedType, mName, mInjectionArgumentList);

  EXPECT_FALSE(field.isConstant());
  EXPECT_TRUE(field.isField());
  EXPECT_FALSE(field.isMethod());
  EXPECT_FALSE(field.isStaticMethod());
  EXPECT_FALSE(field.isMethodSignature());
  EXPECT_FALSE(field.isLLVMFunction());
}

TEST_F(InjectedFieldTest, fieldPrintToStreamTest) {
  InjectedField field(mType, mInjectedType, mName, mInjectionArgumentList);

  stringstream stringStream;
  field.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("  inject MInjectedObject* mField.withFoo(expression);\n",
               stringStream.str().c_str());
}
