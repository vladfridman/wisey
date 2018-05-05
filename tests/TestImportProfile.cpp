//
//  TestImportProfile.cpp
//  runtests
//
//  Created by Vladimir Fridman on 11/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ImportProfile}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/ImportProfile.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct ImportProfileTest : public Test {
  
  IRGenerationContext mContext;
  string mPackage = "some.package";
  ImportProfile* mImportProfile;
  
public:
  
  ImportProfileTest() {
    mImportProfile = new ImportProfile(mPackage);
  }
};

TEST_F(ImportProfileTest, importProfileTest) {
  EXPECT_STREQ("some.package.MObject",
               mImportProfile->getFullName("MObject", 0).c_str());
  
  mImportProfile->addImport("MObject", "some.other.MObject");
  EXPECT_STREQ("some.other.MObject",
               mImportProfile->getFullName("MObject", 0).c_str());
}

TEST_F(ImportProfileTest, getFullNameDeathTest) {
  ImportProfile* importProfile = new ImportProfile("");
  importProfile->setSourceFileName(mContext, "/tmp/source.yz");

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(importProfile->getFullName("MObject", 1));
  EXPECT_STREQ("/tmp/source.yz(1): Error: Could not identify packge for object MObject\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}
