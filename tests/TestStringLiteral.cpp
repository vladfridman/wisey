//
//  TestStringLiteral.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/23/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link StringLiteral}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "IRGenerationContext.hpp"
#include "PrimitiveTypes.hpp"
#include "StringLiteral.hpp"

using namespace std;
using namespace wisey;

using ::testing::Test;

struct StringLiteralTest : public Test {
  IRGenerationContext mContext;
  string mStringBuffer;
  llvm::raw_string_ostream* mStringStream;

  StringLiteralTest() {
    mStringStream = new llvm::raw_string_ostream(mStringBuffer);
  }
  
  ~StringLiteralTest() {
    delete mStringStream;
  }
};

TEST_F(StringLiteralTest, stringLiteralTest) {
  StringLiteral stringLiteral("test", 0);
  
  llvm::Value* irValue = stringLiteral.generateIR(mContext, PrimitiveTypes::VOID);

  *mStringStream << *mContext.getModule();
  *mStringStream << *irValue;
  std::string expected = std::string() +
    "; ModuleID = 'wisey'\nsource_filename = \"wisey\"\n\n" +
    "@.str = internal constant [5 x i8] c\"test\\00\"\n" +
    "i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str, i32 0, i32 0)";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(StringLiteralTest, stringLiteralEscapeNewlineTest) {
  StringLiteral stringLiteral("test\ntest", 0);
  
  stringLiteral.generateIR(mContext, PrimitiveTypes::VOID);
  
  *mStringStream << *mContext.getModule();
  std::string expected = std::string() +
    "; ModuleID = 'wisey'\nsource_filename = \"wisey\"\n\n" +
    "@.str = internal constant [10 x i8] c\"test\\0Atest\\00\"\n";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(StringLiteralTest, isConstantTest) {
  StringLiteral stringLiteral("test", 0);

  EXPECT_TRUE(stringLiteral.isConstant());
}

TEST_F(StringLiteralTest, isAssignableTest) {
  StringLiteral stringLiteral("test", 0);
  
  EXPECT_FALSE(stringLiteral.isAssignable());
}

TEST_F(StringLiteralTest, printToStreamTest) {
  StringLiteral stringLiteral("test", 0);

  stringstream stringStream;
  stringLiteral.printToStream(mContext, stringStream);
  EXPECT_STREQ("\"test\"", stringStream.str().c_str());
}

