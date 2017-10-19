//
//  TestConstant.cpp
//  runtests
//
//  Created by Vladimir Fridman on 10/19/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Constant}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MockExpression.hpp"
#include "wisey/Constant.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Test;

struct ConstantTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockExpression>* mExpression;
  string mName;
  Constant* mConstant;
  
public:
  
  ConstantTest() :
  mExpression(new NiceMock<MockExpression>()),
  mName("MYCONSTANT") {
    ON_CALL(*mExpression, printToStream(_, _)).WillByDefault(Invoke(printExpression));
    
    mConstant = new Constant(PUBLIC_ACCESS, PrimitiveTypes::INT_TYPE, mName, mExpression);
  }
  
  ~ConstantTest() {
    delete mExpression;
  }
  
  static void printExpression(IRGenerationContext& context, iostream& stream) {
    stream << "expression";
  }
};

TEST_F(ConstantTest, getObjectElementTypeTest) {
  EXPECT_EQ(mConstant->getObjectElementType(), OBJECT_ELEMENT_CONSTANT);
}

TEST_F(ConstantTest, printPublicConstantToStreamTest) {
  stringstream stringStream;
  mConstant->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("  public constant int MYCONSTANT = expression;\n", stringStream.str().c_str());
}

TEST_F(ConstantTest, printPrivateConstantToStreamTest) {
  stringstream stringStream;
  Constant* constant = new Constant(PRIVATE_ACCESS, PrimitiveTypes::INT_TYPE, mName, mExpression);
  constant->printToStream(mContext, stringStream);

  EXPECT_EQ(stringStream.str().size(), 0u);
}
