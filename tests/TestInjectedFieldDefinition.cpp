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
#include "MockObjectOwnerType.hpp"
#include "MockObjectType.hpp"
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
  NiceMock<MockObjectOwnerType>* mObjectOwnerType;
  NiceMock<MockObjectType>* mObjectType;
  const NiceMock<MockObjectTypeSpecifier>* mObjectSpecifier;
  string mName;

public:
  
  InjectedFieldDefinitionTest() :
  mType(new NiceMock<MockType>()),
  mExpression(new NiceMock<MockExpression>()),
  mObjectOwnerType(new NiceMock<MockObjectOwnerType>()),
  mObjectType(new NiceMock<MockObjectType>()),
  mObjectSpecifier(new NiceMock<MockObjectTypeSpecifier>()),
  mName("mField") {
    TestPrefix::generateIR(mContext);
    
    ON_CALL(*mObjectOwnerType, isOwner()).WillByDefault(Return(true));
    ON_CALL(*mObjectOwnerType, isController()).WillByDefault(Return(true));
    ON_CALL(*mObjectType, getOwner()).WillByDefault(Return(mObjectOwnerType));
    ON_CALL(*mObjectOwnerType, getReference()).WillByDefault(Return(mObjectType));
    NiceMock<MockObjectTypeSpecifier>* mObjectSpecifier = new NiceMock<MockObjectTypeSpecifier>();
    ON_CALL(*mObjectSpecifier, getType(_)).WillByDefault(Return(mObjectType));
    ObjectOwnerTypeSpecifier* ownerSpecifier = new ObjectOwnerTypeSpecifier(mObjectSpecifier);

    mFieldDeclaration = new InjectedFieldDefinition(ownerSpecifier, mName, mArguments, 0);
  }
  
  ~InjectedFieldDefinitionTest() {
    delete mFieldDeclaration;
    delete mObjectType;
    delete mObjectOwnerType;
  }
};

TEST_F(InjectedFieldDefinitionTest, declareTest) {
  IField* field = mFieldDeclaration->define(mContext, mObjectType);
  
  EXPECT_FALSE(field->isConstant());
  EXPECT_TRUE(field->isField());
  EXPECT_FALSE(field->isMethod());
  EXPECT_FALSE(field->isStaticMethod());
  EXPECT_FALSE(field->isMethodSignature());
  EXPECT_FALSE(field->isLLVMFunction());

  EXPECT_EQ(mObjectOwnerType, field->getType());
  EXPECT_STREQ("mField", field->getName().c_str());
  EXPECT_TRUE(field->isAssignable());

  EXPECT_FALSE(field->isFixed());
  EXPECT_TRUE(field->isInjected());
  EXPECT_FALSE(field->isReceived());
  EXPECT_FALSE(field->isState());
}

