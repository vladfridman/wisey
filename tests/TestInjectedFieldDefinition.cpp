//
//  TestFieldDeclaration.cpp
//  runtests
//
//  Created by Vladimir Fridman on 10/19/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link InjectedFieldDefinition}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MockExpression.hpp"
#include "MockObjectTypeSpecifier.hpp"
#include "MockType.hpp"
#include "TestPrefix.hpp"
#include "wisey/InjectedFieldDefinition.hpp"
#include "wisey/ObjectOwnerTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct InjectedFieldDefinitionTest : public Test {
  IRGenerationContext mContext;
  InjectionArgumentList mArguments;
  InjectionArgument* mInjectionArgument;
  InjectedFieldDefinition* mFieldDeclaration;
  NiceMock<MockType>* mType;
  NiceMock<MockExpression>* mExpression;
  Controller* mController;
  const NiceMock<MockObjectTypeSpecifier>* mObjectSpecifier;
  string mName;

public:
  
  InjectedFieldDefinitionTest() :
  mType(new NiceMock<MockType>()),
  mExpression(new NiceMock<MockExpression>()),
  mObjectSpecifier(new NiceMock<MockObjectTypeSpecifier>()),
  mName("mField") {
    TestPrefix::generateIR(mContext);
    
    NiceMock<MockObjectTypeSpecifier>* mObjectSpecifier = new NiceMock<MockObjectTypeSpecifier>();
    
    mController = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                            "systems.vos.wisey.compiler.tests.CController",
                                            NULL,
                                            mContext.getImportProfile(),
                                            0);
    ON_CALL(*mObjectSpecifier, getType(_)).WillByDefault(Return(mController));
    ObjectOwnerTypeSpecifier* ownerSpecifier = new ObjectOwnerTypeSpecifier(mObjectSpecifier);

    mFieldDeclaration = new InjectedFieldDefinition(ownerSpecifier, mName, mArguments, 0);
  }
  
  ~InjectedFieldDefinitionTest() {
    delete mFieldDeclaration;
  }
};

TEST_F(InjectedFieldDefinitionTest, declareTest) {
  IField* field = mFieldDeclaration->define(mContext, mController);
  
  EXPECT_FALSE(field->isConstant());
  EXPECT_TRUE(field->isField());
  EXPECT_FALSE(field->isMethod());
  EXPECT_FALSE(field->isStaticMethod());
  EXPECT_FALSE(field->isMethodSignature());
  EXPECT_FALSE(field->isLLVMFunction());

  EXPECT_EQ(mController->getOwner(), field->getType());
  EXPECT_STREQ("mField", field->getName().c_str());
  EXPECT_FALSE(field->isAssignable());

  EXPECT_FALSE(field->isFixed());
  EXPECT_TRUE(field->isInjected());
  EXPECT_FALSE(field->isReceived());
  EXPECT_FALSE(field->isState());
}

