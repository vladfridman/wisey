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
  
public:
  
  FakeExpressionTest() :
  mType(new NiceMock<MockType>()),
  mValue(ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 0)) {
  }
  
  ~FakeExpressionTest() {
  }
};

TEST_F(FakeExpressionTest, fakeExpressionTest) {
  FakeExpression fakeExpression(mValue, mType);
  
  EXPECT_EQ(fakeExpression.getType(mContext), mType);
  EXPECT_EQ(fakeExpression.generateIR(mContext), mValue);
  EXPECT_FALSE(fakeExpression.existsInOuterScope(mContext));

  EXPECT_EXIT(fakeExpression.releaseOwnership(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Can not release ownership of a fake epxression");
}
