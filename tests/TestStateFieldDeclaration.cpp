//
//  TestStateFieldDeclaration.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link StateFieldDeclaration}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MockExpression.hpp"
#include "MockType.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/StateFieldDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct StateFieldDeclarationTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockType>* mType;
  NiceMock<MockExpression>* mExpression;
  string mName;
  StateFieldDeclaration* mFieldDeclaration;
  
public:
  
  StateFieldDeclarationTest() :
  mType(new NiceMock<MockType>()),
  mExpression(new NiceMock<MockExpression>()),
  mName("mField") {
    PrimitiveTypeSpecifier* intSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
    mFieldDeclaration = new StateFieldDeclaration(intSpecifier, mName);
  }
  
  ~StateFieldDeclarationTest() {
    delete mFieldDeclaration;
  }
};

TEST_F(StateFieldDeclarationTest, declareTest) {
  IField* field = mFieldDeclaration->declare(mContext, NULL);
  
  EXPECT_FALSE(field->isConstant());
  EXPECT_TRUE(field->isField());
  EXPECT_FALSE(field->isMethod());
  EXPECT_FALSE(field->isStaticMethod());
  EXPECT_FALSE(field->isMethodSignature());
  
  EXPECT_EQ(field->getType(), PrimitiveTypes::INT_TYPE);
  EXPECT_STREQ(field->getName().c_str(), "mField");
  EXPECT_TRUE(field->isAssignable());
  
  EXPECT_FALSE(field->isFixed());
  EXPECT_FALSE(field->isInjected());
  EXPECT_FALSE(field->isReceived());
  EXPECT_TRUE(field->isState());
}

