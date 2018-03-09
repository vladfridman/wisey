//
//  TestFixedFieldDeclaration.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link FixedFieldDeclaration}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MockExpression.hpp"
#include "MockType.hpp"
#include "wisey/FixedFieldDeclaration.hpp"
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

struct FixedFieldDeclarationTest : public Test {
  IRGenerationContext mContext;
  NiceMock<MockType>* mType;
  NiceMock<MockExpression>* mExpression;
  string mName;
  FixedFieldDeclaration* mFieldDeclaration;
  
public:
  
  FixedFieldDeclarationTest() :
  mType(new NiceMock<MockType>()),
  mExpression(new NiceMock<MockExpression>()),
  mName("mField") {
    PrimitiveTypeSpecifier* intSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
    mFieldDeclaration = new FixedFieldDeclaration(intSpecifier, mName);
  }
  
  ~FixedFieldDeclarationTest() {
    delete mFieldDeclaration;
  }
};

TEST_F(FixedFieldDeclarationTest, declareTest) {
  Field* field = mFieldDeclaration->declare(mContext, NULL);
  
  EXPECT_FALSE(field->isConstant());
  EXPECT_TRUE(field->isField());
  EXPECT_FALSE(field->isMethod());
  EXPECT_FALSE(field->isStaticMethod());
  EXPECT_FALSE(field->isMethodSignature());
  
  EXPECT_EQ(field->getType(), PrimitiveTypes::INT_TYPE);
  EXPECT_STREQ(field->getName().c_str(), "mField");
  EXPECT_EQ(field->getInjectionArguments().size(), 0u);
  EXPECT_FALSE(field->isAssignable());
  EXPECT_EQ(field->getFieldKind(), FieldKind::FIXED_FIELD);
}
