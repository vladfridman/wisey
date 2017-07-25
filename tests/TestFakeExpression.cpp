//
//  TestFakeExpression.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 7/16/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link FakeExpression}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>

#include "MockType.hpp"
#include "wisey/FakeExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::NiceMock;
using ::testing::Test;

struct FakeExpressionTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockType>* mType;
  Value* mValue;
  FakeExpression* mFakeExpression;
  
public:
  
  FakeExpressionTest() :
  mType(new NiceMock<MockType>()),
  mValue(ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 0)) {
    mFakeExpression = new FakeExpression(mValue, mType);
  }
  
  ~FakeExpressionTest() {
    delete mFakeExpression;
  }
};

TEST_F(FakeExpressionTest, fakeExpressionTest) {
  EXPECT_EQ(mFakeExpression->getType(mContext), mType);
  EXPECT_EQ(mFakeExpression->generateIR(mContext), mValue);
  EXPECT_FALSE(mFakeExpression->existsInOuterScope(mContext));

  EXPECT_EXIT(mFakeExpression->releaseOwnership(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Can not release ownership of a fake epxression");
}

TEST_F(FakeExpressionTest, releaseOwnershipOwnerDeathTest) {
  EXPECT_EXIT(mFakeExpression->releaseOwnership(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Can not release ownership of a fake epxression");
}

TEST_F(FakeExpressionTest, addReferenceToOwnerDeathTest) {
  EXPECT_EXIT(mFakeExpression->addReferenceToOwner(mContext, NULL),
              ::testing::ExitedWithCode(1),
              "Error: Can not add a reference to non owner type fake expression");
}
