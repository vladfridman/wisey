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

#include "TestFileSampleRunner.hpp"
#include "wisey/IObjectTypeSpecifier.hpp"
#include "wisey/Model.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct IObjectTypeSpecifierTest : public Test {
  IRGenerationContext mContext;
  vector<string> mPackage;
  
  IObjectTypeSpecifierTest() {
    mPackage.push_back("systems");
    mPackage.push_back("vos");
    mPackage.push_back("wisey");
  }
};

TEST_F(IObjectTypeSpecifierTest, getFullNameNameAndPackageTest) {
  EXPECT_STREQ("systems.vos.wisey.MObject",
               IObjectTypeSpecifier::getFullName(mContext, "MObject", mPackage).c_str());
  
  vector<string> package;
  mContext.setPackage("lang.wisey");
  EXPECT_STREQ("lang.wisey.MObject",
               IObjectTypeSpecifier::getFullName(mContext, "MObject", package).c_str());
  
  Model* model = Model::newModel("other.wisey.MObject", NULL);
  mContext.addImport(model);
  EXPECT_STREQ("other.wisey.MObject",
               IObjectTypeSpecifier::getFullName(mContext, "MObject", package).c_str());
}

TEST_F(TestFileSampleRunner, longObjectTypeSpecifiersRunTest) {
  runFile("tests/samples/test_long_object_type_specifiers.yz", "5");
}


