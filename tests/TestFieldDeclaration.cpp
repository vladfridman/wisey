//
//  TestFieldDeclaration.cpp
//  runtests
//
//  Created by Vladimir Fridman on 10/19/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link FieldDeclaration}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MockExpression.hpp"
#include "MockType.hpp"
#include "wisey/FieldDeclaration.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct FieldDeclarationTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockType>* mType;
  NiceMock<MockExpression>* mExpression;
  string mName;
  ExpressionList mArguments;
  FieldDeclaration* mFieldDeclaration;
  
public:
  
  FieldDeclarationTest() :
  mType(new NiceMock<MockType>()),
  mExpression(new NiceMock<MockExpression>()),
  mName("mField") {
    mArguments.push_back(mExpression);
    
    PrimitiveTypeSpecifier* intSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
    mFieldDeclaration = new FieldDeclaration(FieldKind::FIXED_FIELD,
                                             intSpecifier,
                                             mName,
                                             mArguments);
  }
  
  ~FieldDeclarationTest() {
    delete mFieldDeclaration;
  }
};

TEST_F(FieldDeclarationTest, declareTest) {
  Field* field = mFieldDeclaration->declare(mContext);
  
  EXPECT_EQ(field->getType(), PrimitiveTypes::INT_TYPE);
  EXPECT_STREQ(field->getName().c_str(), "mField");
  EXPECT_EQ(field->getArguments().size(), 1u);
  EXPECT_EQ(field->getArguments().at(0), mExpression);
  EXPECT_FALSE(field->isAssignable());
  EXPECT_EQ(field->getFieldKind(), FieldKind::FIXED_FIELD);
}

