//
//  TestCompilerArgumentParser.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/24/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link CompilerArgumentParser}
//

#include <gtest/gtest.h>

#include "wisey/CompilerArgumentParser.hpp"

using namespace std;
using namespace wisey;

struct CompilerArgumentParserTest : public ::testing::Test {

  CompilerArgumentParser mParser;
  
  CompilerArgumentParserTest() { }
};

TEST_F(CompilerArgumentParserTest, noArgumentsTest) {
  char const * argv[] = {"bin/wisey"};
  EXPECT_EXIT(mParser.parse(1, (char**) argv),
              ::testing::ExitedWithCode(1),
              "Error: Syntax: wisey -o <bitcode_file> <filename1.yz> <filename2.yz>\n");
}

TEST_F(CompilerArgumentParserTest, helpTest) {
  char const * argv[] = {"bin/wisey", "-h"};
  
  EXPECT_EXIT(mParser.parse(2, (char**) argv),
              ::testing::ExitedWithCode(1),
              "Error: Syntax: wisey -o <bitcode_file> <filename1.yz> <filename2.yz>\n");
}

TEST_F(CompilerArgumentParserTest, missingOutputFileTest) {
  char const * argv[] = {"bin/wisey", "tests/samples/test_addition.yz", "-o"};
  
  EXPECT_EXIT(mParser.parse(3, (char**) argv),
              ::testing::ExitedWithCode(1),
              "Error: You need to specify the output file name after \"-o\"\n");
}

TEST_F(CompilerArgumentParserTest, unknownArgumentTest) {
  char const * argv[] = {"bin/wisey", "tests/samples/test_addition.yz", "-foo"};
  
  EXPECT_EXIT(mParser.parse(3, (char**) argv),
              ::testing::ExitedWithCode(1),
              "Error: Unknown argument -foo\n");
}

TEST_F(CompilerArgumentParserTest, normalRunTest) {
  char const * argv[] = {"bin/wisey", "tests/samples/test_addition.yz"};
  
  CompilerArguments arguments = mParser.parse(2, (char**) argv);
  EXPECT_EQ(arguments.getSourceFiles().size(), 1u);
  EXPECT_STREQ(arguments.getSourceFiles().front().c_str(), "tests/samples/test_addition.yz");
  EXPECT_STREQ(arguments.getOutputFile().c_str(), "");
  EXPECT_FALSE(arguments.shouldPrintAssembly());
}

TEST_F(CompilerArgumentParserTest, outputToFileTest) {
  char const * argv[] = {"bin/wisey", "tests/samples/test_addition.yz", "-o", "build/test.bc"};
  
  CompilerArguments arguments = mParser.parse(4, (char**) argv);
  EXPECT_EQ(arguments.getSourceFiles().size(), 1u);
  EXPECT_STREQ(arguments.getSourceFiles().front().c_str(), "tests/samples/test_addition.yz");
  EXPECT_STREQ(arguments.getOutputFile().c_str(), "build/test.bc");
  EXPECT_FALSE(arguments.shouldPrintAssembly());
}


TEST_F(CompilerArgumentParserTest, printAssemblyTest) {
  char const * argv[] = {"bin/wisey", "tests/samples/test_addition.yz", "--emit-llvm"};
  
  CompilerArguments arguments = mParser.parse(3, (char**) argv);
  EXPECT_EQ(arguments.getSourceFiles().size(), 1u);
  EXPECT_STREQ(arguments.getSourceFiles().front().c_str(), "tests/samples/test_addition.yz");
  EXPECT_STREQ(arguments.getOutputFile().c_str(), "");
  EXPECT_TRUE(arguments.shouldPrintAssembly());
}

