//
//  TestStringConstant.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link StringConstant}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/StringConstant.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct StringConstantTest : public Test {
  IRGenerationContext mContext;
  string mStringBuffer;
  raw_string_ostream* mStringStream;

  StringConstantTest() {
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~StringConstantTest() {
    delete mStringStream;
  }
};

TEST_F(StringConstantTest, stringConstantTest) {
  StringConstant stringConstant("\"test\"");
  
  Value* irValue = stringConstant.generateIR(mContext);

  *mStringStream << *mContext.getModule();
  *mStringStream << *irValue;
  std::string expected = std::string() +
    "; ModuleID = 'wisey'\nsource_filename = \"wisey\"\n\n" +
    "@.str = internal constant [5 x i8] c\"test\\00\"\n" +
    "i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str, i32 0, i32 0)";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(StringConstantTest, stringConstantEscapeNewlineTest) {
  StringConstant stringConstant("\"test\ntest\"");
  
  stringConstant.generateIR(mContext);
  
  *mStringStream << *mContext.getModule();
  std::string expected = std::string() +
    "; ModuleID = 'wisey'\nsource_filename = \"wisey\"\n\n" +
    "@.str = internal constant [10 x i8] c\"test\\0Atest\\00\"\n";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(StringConstantTest, existsInOuterScopeTest) {
  StringConstant stringConstant("\"test\"");
  
  EXPECT_FALSE(stringConstant.existsInOuterScope(mContext));
}

TEST_F(StringConstantTest, releaseOwnershipDeathTest) {
  StringConstant stringConstant("\"test\ntest\"");
  
  EXPECT_EXIT(stringConstant.releaseOwnership(mContext),
              ::testing::ExitedWithCode(1),
              "Can not release ownership of a string constant, it is not a heap pointer");
}

TEST_F(StringConstantTest, addReferenceToOwnerDeathTest) {
  StringConstant stringConstant("\"test\ntest\"");
  
  EXPECT_EXIT(stringConstant.addReferenceToOwner(mContext, NULL),
              ::testing::ExitedWithCode(1),
              "Error: Can not add a reference to a string constant expression");
}
