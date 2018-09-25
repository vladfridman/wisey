//
//  TestFakeExpressionWithCleanup.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/8/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link FakeExpressionWithCleanup}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>

#include "MockType.hpp"
#include "FakeExpressionWithCleanup.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::NiceMock;
using ::testing::Test;

struct FakeExpressionWithCleanupTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockType>* mType;
  Value* mValue;
  FakeExpressionWithCleanup* mFakeExpressionWithCleanup;
  
public:
  
  FakeExpressionWithCleanupTest() :
  mType(new NiceMock<MockType>()),
  mValue(ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 0)) {
    mFakeExpressionWithCleanup = new FakeExpressionWithCleanup(mValue, mType);
  }
  
  ~FakeExpressionWithCleanupTest() {
  }
};

TEST_F(FakeExpressionWithCleanupTest, isConstantTest) {
  EXPECT_FALSE(mFakeExpressionWithCleanup->isConstant());
}

TEST_F(FakeExpressionWithCleanupTest, isAssignableTest) {
  EXPECT_FALSE(mFakeExpressionWithCleanup->isAssignable());
}

TEST_F(FakeExpressionWithCleanupTest, printToStreamTest) {
  std::stringstream stringStream;
  mFakeExpressionWithCleanup->printToStream(mContext, stringStream);
  
  EXPECT_EQ(0u, stringStream.str().size());
}

TEST_F(FakeExpressionWithCleanupTest, destructorTest) {
  EXPECT_CALL(*mType, die());
  delete mFakeExpressionWithCleanup;
}
