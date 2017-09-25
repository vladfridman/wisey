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

#include "wisey/IObjectTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct IObjectTypeSpecifierTest : public Test {
  vector<string> mPackage;
  
  IObjectTypeSpecifierTest() {
    mPackage.push_back("systems");
    mPackage.push_back("vos");
    mPackage.push_back("wisey");
  }
};

TEST_F(IObjectTypeSpecifierTest, printPackageToStreamTest) {
  stringstream stringStream;
  IObjectTypeSpecifier::printPackageToStream(stringStream, mPackage);
  
  EXPECT_STREQ("systems.vos.wisey.", stringStream.str().c_str());
}



