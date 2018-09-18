//
//  TestCompilerArguments.cpp
//  runtests
//
//  Created by Vladimir Fridman on 9/11/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link CompilerArguments}
//

#include <gtest/gtest.h>

#include "wisey/CompilerArguments.hpp"

using namespace std;
using namespace wisey;

struct CompilerArgumentsTest : public ::testing::Test {
  CompilerArguments mCompilerArguments;
  
  CompilerArgumentsTest() { }
};

TEST_F(CompilerArgumentsTest, testGetForYzc) {
  EXPECT_STREQ("", mCompilerArguments.getForYzc().c_str());

  mCompilerArguments.addLibraryName("-lsomelib");
  mCompilerArguments.addLibraryPath("-Llib");
  mCompilerArguments.addSourceFile("file.yz");
  mCompilerArguments.setShouldOptimize(false);
  mCompilerArguments.setVerbouse(true);
  mCompilerArguments.setHeaderFile("header.yz");
  mCompilerArguments.setOutputFile("output.o");
  EXPECT_STREQ("--no-optimization -H header.yz --verbouse --output output.o file.yz ",
               mCompilerArguments.getForYzc().c_str());
}