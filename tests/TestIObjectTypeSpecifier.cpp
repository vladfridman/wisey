//
//  TestIObjectTypeSpecifier.cpp
//  runtests
//
//  Created by Vladimir Fridman on 9/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IObjectTypeSpecifier}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MockObjectType.hpp"
#include "TestFileRunner.hpp"
#include "FakeExpression.hpp"
#include "IObjectTypeSpecifier.hpp"
#include "Model.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct IObjectTypeSpecifierTest : public Test {
  IRGenerationContext mContext;
  string mPackage = "systems.vos.wisey";

  IObjectTypeSpecifierTest() {
  }
};

TEST_F(IObjectTypeSpecifierTest, getFullNameNameWithPackageTest) {
  PackageType* packageType = new PackageType(mPackage);
  FakeExpression* packageExpression = new FakeExpression(NULL, packageType);
  EXPECT_STREQ("systems.vos.wisey.MObject",
               IObjectTypeSpecifier::getFullName(mContext, "MObject", packageExpression, 0).c_str());
}

TEST_F(IObjectTypeSpecifierTest, getFullNameNameWithImportProfileTest) {
  ImportProfile* importProfile = new ImportProfile("lang.wisey");
  mContext.setImportProfile(importProfile);
  EXPECT_STREQ("lang.wisey.MObject",
               IObjectTypeSpecifier::getFullName(mContext, "MObject", NULL, 0).c_str());
  
  importProfile->addImport("MObject", "other.wisey.MObject");
  EXPECT_STREQ("other.wisey.MObject",
               IObjectTypeSpecifier::getFullName(mContext, "MObject", NULL, 0).c_str());
}

TEST_F(IObjectTypeSpecifierTest, getFullNameInnerObjectSameNameAsCurrentObjectTest) {
  NiceMock<MockObjectType> mockObject;
  string fullName = "systems.vos.wisey.MOuterObject.MInnerObject";
  ON_CALL(mockObject, getShortName()).WillByDefault(Return("MInnerObject"));
  ON_CALL(mockObject, getTypeName()).WillByDefault(Return(fullName));
  mContext.setObjectType(&mockObject);
  
  EXPECT_STREQ(fullName.c_str(),
               IObjectTypeSpecifier::getFullName(mContext, "MInnerObject", NULL, 0).c_str());
}

TEST_F(IObjectTypeSpecifierTest, getFullNameInnerObjectInnerOfCurrentObjectTest) {
  NiceMock<MockObjectType> outerObject;
  ON_CALL(outerObject, getShortName()).WillByDefault(Return("MOuterObject"));
  ON_CALL(outerObject, getTypeName()).WillByDefault(Return("systems.vos.wisey.MOuterObject"));
  NiceMock<MockObjectType> innerObject;
  ON_CALL(innerObject, getShortName()).WillByDefault(Return("MInnerObject"));
  ON_CALL(innerObject, getTypeName()).
  WillByDefault(Return("systems.vos.wisey.MOuterObject.MInnerObject"));
  ON_CALL(outerObject, getInnerObject(_)).WillByDefault(Return(&innerObject));
  mContext.setObjectType(&outerObject);
  
  EXPECT_STREQ("systems.vos.wisey.MOuterObject.MInnerObject",
               IObjectTypeSpecifier::getFullName(mContext, "MInnerObject", NULL, 0).c_str());
}

TEST_F(TestFileRunner, longObjectTypeSpecifiersRunTest) {
  runFile("tests/samples/test_long_object_type_specifiers.yz", 5);
}


