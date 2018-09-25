//
//  TestObjectOwnerTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ObjectOwnerTypeSpecifier}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MockObjectOwnerType.hpp"
#include "MockObjectTypeSpecifier.hpp"
#include "MockObjectType.hpp"
#include "ObjectOwnerTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ObjectOwnerTypeSpecifierTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockObjectTypeSpecifier>* mObjectTypeSpecifier;
  
  ObjectOwnerTypeSpecifierTest() : mObjectTypeSpecifier(new NiceMock<MockObjectTypeSpecifier>()) {
  }
  
  static void printObjectTypeSpecifier(IRGenerationContext& context, iostream& stream) {
    stream << "systems.vos.wisey.compiler.tests.NElement";
  }
};

TEST_F(ObjectOwnerTypeSpecifierTest, getTypeTest) {
  NiceMock<MockObjectType> mockObjectType;
  NiceMock<MockObjectOwnerType> mockOwnerObjectType;
  ON_CALL(mockObjectType, getOwner()).WillByDefault(Return(&mockOwnerObjectType));
  ON_CALL(*mObjectTypeSpecifier, getType(_)).WillByDefault(Return(&mockObjectType));
  ON_CALL(*mObjectTypeSpecifier, getLine()).WillByDefault(Return(7));

  ObjectOwnerTypeSpecifier ownerTypeSpecifier(mObjectTypeSpecifier);
  
  EXPECT_EQ(&mockOwnerObjectType, ownerTypeSpecifier.getType(mContext));
  EXPECT_EQ(7, ownerTypeSpecifier.getLine());
}

TEST_F(ObjectOwnerTypeSpecifierTest, printToStreamTest) {
  ON_CALL(*mObjectTypeSpecifier, printToStream(_, _))
    .WillByDefault(Invoke(printObjectTypeSpecifier));
  ObjectOwnerTypeSpecifier ownerTypeSpecifier(mObjectTypeSpecifier);

  stringstream stringStream;
  ownerTypeSpecifier.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("systems.vos.wisey.compiler.tests.NElement*", stringStream.str().c_str());
}
