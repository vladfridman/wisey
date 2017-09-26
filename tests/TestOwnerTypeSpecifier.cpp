//
//  TestOwnerTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link OwnerTypeSpecifier}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MockObjectOwnerType.hpp"
#include "MockObjectTypeSpecifier.hpp"
#include "MockObjectType.hpp"
#include "wisey/OwnerTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Test;

struct OwnerTypeSpecifierTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockObjectTypeSpecifier>* mObjectTypeSpecifier;
  
  OwnerTypeSpecifierTest() : mObjectTypeSpecifier(new NiceMock<MockObjectTypeSpecifier>()) {
  }
  
  static void printObjectTypeSpecifier(IRGenerationContext& context, iostream& stream) {
    stream << "systems.vos.wisey.compiler.tests.NElement";
  }
};

TEST_F(OwnerTypeSpecifierTest, getTypeTest) {
  NiceMock<MockObjectType> mockObjectType;
  NiceMock<MockObjectOwnerType> mockOwnerObjectType;
  ON_CALL(mockObjectType, getOwner()).WillByDefault(Return(&mockOwnerObjectType));
  ON_CALL(*mObjectTypeSpecifier, getType(_)).WillByDefault(Return(&mockObjectType));
  
  OwnerTypeSpecifier ownerTypeSpecifier(mObjectTypeSpecifier);
  
  EXPECT_EQ(ownerTypeSpecifier.getType(mContext), &mockOwnerObjectType);
}

TEST_F(OwnerTypeSpecifierTest, printToStreamTest) {
  ON_CALL(*mObjectTypeSpecifier, printToStream(_, _))
    .WillByDefault(Invoke(printObjectTypeSpecifier));
  OwnerTypeSpecifier ownerTypeSpecifier(mObjectTypeSpecifier);

  stringstream stringStream;
  ownerTypeSpecifier.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("systems.vos.wisey.compiler.tests.NElement*", stringStream.str().c_str());
}
