//
//  TestYzcArgumentParser.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/24/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link YzcArgumentParser}
//

#include <gtest/gtest.h>

#include "wisey/YzcArgumentParser.hpp"

using namespace std;
using namespace wisey;

struct YzcArgumentParserTest : public ::testing::Test {

  YzcArgumentParser mParser;
  
  YzcArgumentParserTest() { }
};

TEST_F(YzcArgumentParserTest, noArgumentsTest) {
  vector<string> arguments;
  EXPECT_EXIT(mParser.parse(arguments),
              ::testing::ExitedWithCode(1),
              "Syntax: wiseyc "
              "\\[-d|--destructor-debug\\] "
              "\\[-e|--emit-llvm\\] "
              "\\[-h|--help\\] "
              "\\[-v|--verbouse\\] "
              "\\[-H|--headers <header_file.yzh>\\] "
              "\\[-o|--output <object_file.o>\\] "
              "\\[-n|--no-output\\] <source_file.yz>...\n");
}

TEST_F(YzcArgumentParserTest, helpTest) {
  vector<string> arguments;
  arguments.push_back("-h");

  EXPECT_EXIT(mParser.parse(arguments),
              ::testing::ExitedWithCode(1),
              "Syntax: wiseyc "
              "\\[-d|--destructor-debug\\] "
              "\\[-e|--emit-llvm\\] "
              "\\[-h|--help\\] "
              "\\[-v|--verbouse\\] "
              "\\[-H|--headers <header_file.yzh>\\] "
              "\\[-o|--output <object_file.o>\\] "
              "\\[-n|--no-output\\] <source_file.yz>...\n");
}

TEST_F(YzcArgumentParserTest, missingOutputFileTest) {
  vector<string> arguments;
  arguments.push_back("tests/samples/test_addition.yz");
  arguments.push_back("-o");
  
  EXPECT_EXIT(mParser.parse(arguments),
              ::testing::ExitedWithCode(1),
              "Error: You need to specify the output file name after \"-o\"\n");
}

TEST_F(YzcArgumentParserTest, unknownArgumentTest) {
  vector<string> arguments;
  arguments.push_back("tests/samples/test_addition.yz");
  arguments.push_back("-foo");

  EXPECT_EXIT(mParser.parse(arguments),
              ::testing::ExitedWithCode(1),
              "Error: Unknown argument -foo\n");
}

TEST_F(YzcArgumentParserTest, normalRunTest) {
  vector<string> arguments;
  arguments.push_back("tests/samples/test_addition.yz");
  
  CompilerArguments parsedArguments = mParser.parse(arguments);
  EXPECT_EQ(parsedArguments.getSourceFiles().size(), 1u);
  EXPECT_STREQ(parsedArguments.getSourceFiles().front().c_str(), "tests/samples/test_addition.yz");
  EXPECT_STREQ(parsedArguments.getOutputFile().c_str(), "");
  EXPECT_FALSE(parsedArguments.shouldPrintAssembly());
}

TEST_F(YzcArgumentParserTest, outputToFileTest) {
  vector<string> arguments;
  arguments.push_back("tests/samples/test_addition.yz");
  arguments.push_back("-o");
  arguments.push_back("build/test.bc");
  
  CompilerArguments parsedArguments = mParser.parse(arguments);
  EXPECT_EQ(parsedArguments.getSourceFiles().size(), 1u);
  EXPECT_STREQ(parsedArguments.getSourceFiles().front().c_str(), "tests/samples/test_addition.yz");
  EXPECT_STREQ(parsedArguments.getOutputFile().c_str(), "build/test.bc");
  EXPECT_FALSE(parsedArguments.shouldPrintAssembly());
}

TEST_F(YzcArgumentParserTest, printAssemblyTest) {
  vector<string> arguments;
  arguments.push_back("tests/samples/test_addition.yz");
  arguments.push_back("--emit-llvm");
  
  CompilerArguments parsedArguments = mParser.parse(arguments);
  EXPECT_EQ(parsedArguments.getSourceFiles().size(), 1u);
  EXPECT_STREQ(parsedArguments.getSourceFiles().front().c_str(), "tests/samples/test_addition.yz");
  EXPECT_STREQ(parsedArguments.getOutputFile().c_str(), "");
  EXPECT_TRUE(parsedArguments.shouldPrintAssembly());
}

