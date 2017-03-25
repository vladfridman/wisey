//
//  TestIdentifier.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/17/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Identifier}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/Identifier.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/LocalStackVariable.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

class MockVariable : public IVariable {
public:
  MOCK_CONST_METHOD0(getName, string ());
  MOCK_CONST_METHOD0(getType, IType* ());
  MOCK_CONST_METHOD0(getValue, Value* ());
  MOCK_CONST_METHOD2(generateIdentifierIR, Value* (IRGenerationContext&, string));
  MOCK_METHOD2(generateAssignmentIR, Value* (IRGenerationContext&, IExpression&));
  MOCK_CONST_METHOD1(free, void (IRGenerationContext&));
};

struct IdentifierTest : public Test {
  IRGenerationContext mContext;

  IdentifierTest() {
    mContext.getScopes().pushScope();
  }
  
  ~IdentifierTest() { }
};

TEST_F(IdentifierTest, UndeclaredVariableDeathTest) {
  Identifier identifier("foo", "bar");

  EXPECT_EXIT(identifier.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Undeclared variable 'foo'");
}

TEST_F(IdentifierTest, VariableTypeTest) {
  LocalStackVariable* variable = new LocalStackVariable("foo", PrimitiveTypes::INT_TYPE, NULL);
  mContext.getScopes().setVariable(variable);
  Identifier identifier("foo", "bar");

  EXPECT_EQ(identifier.getType(mContext), PrimitiveTypes::INT_TYPE);
}

TEST_F(IdentifierTest, GenerateIdentifierIR) {
  NiceMock<MockVariable> mockVariable;
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  mContext.getScopes().setVariable(&mockVariable);
  
  Identifier identifier("foo", "bar");
  
  EXPECT_CALL(mockVariable, generateIdentifierIR(_, "bar")).Times(1);
  EXPECT_CALL(mockVariable, generateAssignmentIR(_, _)).Times(0);
  
  identifier.generateIR(mContext);
}

TEST_F(IdentifierTest, releaseOwnership) {
  NiceMock<MockVariable> mockVariable;
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  mContext.getScopes().setVariable(&mockVariable);
  Identifier identifier("foo", "bar");
  
  EXPECT_EQ(mContext.getScopes().getVariable("foo"), &mockVariable);

  identifier.releaseOwnership(mContext);
  
  EXPECT_EQ(mContext.getScopes().getVariable("foo"), nullptr);
}

