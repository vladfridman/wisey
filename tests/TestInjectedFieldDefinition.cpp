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

#include "MockConcreteObjectType.hpp"
#include "MockObjectTypeSpecifier.hpp"
#include "MockObjectOwnerType.hpp"
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
  NiceMock<MockConcreteObjectType>* mObjectType;
  NiceMock<MockObjectOwnerType>* mObjectOwnerType;
  const NiceMock<MockObjectTypeSpecifier>* mObjectSpecifier;
  string mName;

public:
  
  InjectedFieldDefinitionTest() :
  mObjectType(new NiceMock<MockConcreteObjectType>()),
  mObjectOwnerType(new NiceMock<MockObjectOwnerType>()),
  mObjectSpecifier(new NiceMock<MockObjectTypeSpecifier>()),
  mName("mField") {
    TestPrefix::generateIR(mContext);
    
    NiceMock<MockObjectTypeSpecifier>* mObjectSpecifier = new NiceMock<MockObjectTypeSpecifier>();
    
    ON_CALL(*mObjectSpecifier, getType(_)).WillByDefault(Return(mObjectType));
    ObjectOwnerTypeSpecifier* ownerSpecifier = new ObjectOwnerTypeSpecifier(mObjectSpecifier);
    ON_CALL(*mObjectType, getOwner()).WillByDefault(Return(mObjectOwnerType));
    ON_CALL(*mObjectType, isController()).WillByDefault(Return(true));
    ON_CALL(*mObjectOwnerType, isOwner()).WillByDefault(Return(true));
    ON_CALL(*mObjectOwnerType, isController()).WillByDefault(Return(true));
    ON_CALL(*mObjectOwnerType, getReference()).WillByDefault(Return(mObjectType));

    mFieldDeclaration = InjectedFieldDefinition::createDelayed(ownerSpecifier,
                                                               mName,
                                                               mArguments,
                                                               0);
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
  EXPECT_TRUE(field->isAssignable(mObjectType));

  EXPECT_TRUE(field->isInjected());
  EXPECT_FALSE(field->isReceived());
  EXPECT_FALSE(field->isState());
}

