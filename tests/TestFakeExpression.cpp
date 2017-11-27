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

TEST_F(FakeExpressionTest, getVariableTest) {
  EXPECT_EQ(mFakeExpression->getVariable(mContext), nullptr);
}

TEST_F(FakeExpressionTest, isConstantTest) {
  EXPECT_FALSE(mFakeExpression->isConstant());
}

TEST_F(FakeExpressionTest, printToStreamTest) {
  std::stringstream stringStream;
  mFakeExpression->printToStream(mContext, stringStream);
  
  EXPECT_EQ(0u, stringStream.str().size());
}

