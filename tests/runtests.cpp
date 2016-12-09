#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::NiceMock;
using ::testing::Return;

class IB {
public:
  virtual int getSpeed() = 0;
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

TEST(AClassTest, TestInitialValue) {
  NiceMock<MockB> niceMockB;
  ON_CALL(niceMockB, getSpeed()).WillByDefault(Return(2));
  A a(niceMockB);
  
  ASSERT_EQ(2, a.getValue());
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
