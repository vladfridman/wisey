//
//  TestMain.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests main.cpp

#include <gtest/gtest.h>

#include <array>
#include <iostream>

using namespace std;

struct TestMain : public ::testing::Test {
  TestMain() {
    exec("mkdir -p build");
  }
  
  ~TestMain() { }

  string exec(const char* command) {
    array<char, 128> buffer;
    string result;
    shared_ptr<FILE> pipe(popen(command, "r"), pclose);
    if (!pipe) return "popen() failed!";
    while (!feof(pipe.get())) {
      if (fgets(buffer.data(), 128, pipe.get()) != NULL) {
        result += buffer.data();
      }
    }
    return result;
  }
};

TEST_F(TestMain, noArgumentsTest) {
  EXPECT_STREQ(exec("bin/wisey 2>&1").c_str(),
               "Error: Syntax: wisey -o <bitcode_file> <filename1.yz> <filename2.yz>\n");
}

TEST_F(TestMain, missingFileTest) {
  EXPECT_STREQ(exec("bin/wisey tests/samples/missingFile.yz 2>&1").c_str(),
               "Info: opening tests/samples/missingFile.yz\n"
               "Error: File tests/samples/missingFile.yz not found!\n");
}

TEST_F(TestMain, missingOutputFileTest) {
  EXPECT_STREQ(exec("bin/wisey tests/samples/test_addition.yz -o 2>&1").c_str(),
               "Error: You need to specify the output file name after \"-o\"\n");
}

TEST_F(TestMain, helpTest) {
  EXPECT_STREQ(exec("bin/wisey -h 2>&1").c_str(),
               "Error: Syntax: wisey -o <bitcode_file> <filename1.yz> <filename2.yz>\n");
}

TEST_F(TestMain, outputToFileTest) {
  exec("bin/wisey tests/samples/test_addition.yz -o build/test.bc 2>&1");
  exec("/usr/local/bin/llc -filetype=obj build/test.bc");
  exec("g++ -o build/test build/test.o");
  int result = system("build/test");
  int returnValue = WEXITSTATUS(result);
  
  EXPECT_EQ(returnValue, 7);
}

TEST_F(TestMain, runTest) {
  string result = exec("bin/wisey tests/samples/test_addition.yz");

  string lastLine = result.substr(result.find("Info: Result"));
  EXPECT_STREQ(lastLine.c_str(), "Info: Result: 7\n");
}

TEST_F(TestMain, runMultipleFilesTest) {
  string result = exec("bin/wisey tests/samples/test_multifile/Adder.yz "
                       "tests/samples/test_multifile/Runner.yz");
  
  string lastLine = result.substr(result.find("Info: Result"));
  EXPECT_STREQ(lastLine.c_str(), "Info: Result: 5\n");
}
