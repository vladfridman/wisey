//
//  TestNativeTypeSpecifier.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/8/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link NativeTypeSpecifier}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MockType.hpp"
#include "wisey/NativeTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Test;

struct NativeTypeSpecifierTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockType>* mNativeType;
  NativeTypeSpecifier* mNativeTypeSpecifier;

  NativeTypeSpecifierTest() :
  mNativeType(new NiceMock<MockType>()),
  mNativeTypeSpecifier(new NativeTypeSpecifier(mNativeType)) {
    EXPECT_CALL(*mNativeType, die());
  }
  
  ~NativeTypeSpecifierTest() {
    delete mNativeType;
  }
  
  static void printNativeType(IRGenerationContext& context, iostream& stream) {
    stream << "native";
  }
};

TEST_F(NativeTypeSpecifierTest, getTypeTest) {
  EXPECT_EQ(mNativeType, mNativeTypeSpecifier->getType(mContext));
}

TEST_F(NativeTypeSpecifierTest, printToStreamTest) {
  ON_CALL(*mNativeType, printToStream(_, _)).WillByDefault(Invoke(printNativeType));
  
  stringstream stringStream;
  mNativeTypeSpecifier->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("native", stringStream.str().c_str());
}

