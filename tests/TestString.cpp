//
//  TestString.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link String}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/String.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

struct StringTest : public Test {
  IRGenerationContext mContext;
  string mStringBuffer;
  raw_string_ostream* mStringStream;

  StringTest() {
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~StringTest() {
    delete mStringStream;
  }
};

TEST_F(StringTest, StringConstantTest) {
  String string("\"test\"");
  
  Value* irValue = string.generateIR(mContext);

  *mStringStream << *mContext.getModule();
  *mStringStream << *irValue;
  std::string expected = std::string() +
    "; ModuleID = 'yazyk'\nsource_filename = \"yazyk\"\n\n" +
    "@.str = internal constant [5 x i8] c\"test\\00\"\n" +
    "i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str, i32 0, i32 0)";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(StringTest, StringConstantEscapeNewlineTest) {
  String string("\"test\ntest\"");
  
  string.generateIR(mContext);
  
  *mStringStream << *mContext.getModule();
  std::string expected = std::string() +
    "; ModuleID = 'yazyk'\nsource_filename = \"yazyk\"\n\n" +
    "@.str = internal constant [10 x i8] c\"test\\0Atest\\00\"\n";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}
