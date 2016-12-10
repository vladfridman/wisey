//
//  Tests {@link AddditiveMultiplicativeExpression}
//  Yazyk
//
//  Created by Vladimir Fridman on 12/9/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "node.hpp"
#include "codegen.hpp"

using ::testing::NiceMock;
using ::testing::Return;
using namespace yazyk;

class IB {
public:
  virtual int getSpeed() = 0;
  virtual ~IB() {}
};

class B : public IB {
  int speed;
public:
  B(int speed) : speed(speed) { }
  int getSpeed() { return speed; }
};

class A {
  IB &speedHolder;
  
public:
  A(IB &speedHolder) : speedHolder(speedHolder) { }
  int getValue() { return speedHolder.getSpeed(); }
};

class MockB : public IB {
public:
  MOCK_METHOD0(getSpeed, int());
};

class MockExpression : public IExpression {
  MOCK_METHOD1(generateIR, llvm::Value*(IRGenerationContext&));
};

TEST(AddditiveMultiplicativeExpressionTest, SimpleTest) {
  NiceMock<MockExpression> lhs;
  NiceMock<MockExpression> rhs;
  
  AddditiveMultiplicativeExpression expression(lhs, '+', rhs);

  NiceMock<MockB> niceMockB;
  ON_CALL(niceMockB, getSpeed()).WillByDefault(Return(2));
  A a(niceMockB);
  
  ASSERT_EQ(2, a.getValue());
}
