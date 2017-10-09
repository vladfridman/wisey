//
//  TestIdentifier.cpp
//  Wisey
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

#include "MockVariable.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/StackVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct IdentifierTest : public Test {
  IRGenerationContext mContext;
  Interface* mInterface;

  IdentifierTest() {
    vector<InterfaceTypeSpecifier*> parentInterfaces;
    vector<MethodSignatureDeclaration*> interfaceMethods;
    mInterface = Interface::newInterface("systems.vos.wisey.compiler.tests.IInterface",
                                         NULL,
                                         parentInterfaces,
                                         interfaceMethods);

    mContext.getScopes().pushScope();
  }
  
  ~IdentifierTest() { }
};

TEST_F(IdentifierTest, getVariableTest) {
  StackVariable* variable = new StackVariable("foo", PrimitiveTypes::INT_TYPE, NULL);
  mContext.getScopes().setVariable(variable);
  Identifier identifier("foo", "bar");

  EXPECT_EQ(identifier.getVariable(mContext), variable);
}

TEST_F(IdentifierTest, undeclaredVariableDeathTest) {
  Identifier identifier("foo", "bar");

  EXPECT_EXIT(identifier.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Undeclared variable 'foo'");
}

TEST_F(IdentifierTest, variableTypeTest) {
  StackVariable* variable = new StackVariable("foo", PrimitiveTypes::INT_TYPE, NULL);
  mContext.getScopes().setVariable(variable);
  Identifier identifier("foo", "bar");

  EXPECT_EQ(identifier.getType(mContext), PrimitiveTypes::INT_TYPE);
}

TEST_F(IdentifierTest, generateIdentifierIR) {
  NiceMock<MockVariable> mockVariable;
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  mContext.getScopes().setVariable(&mockVariable);
  
  Identifier identifier("foo", "bar");
  
  EXPECT_CALL(mockVariable, generateIdentifierIR(_, "bar")).Times(1);
  EXPECT_CALL(mockVariable, generateAssignmentIR(_, _)).Times(0);
  
  identifier.generateIR(mContext);
}

TEST_F(IdentifierTest, releaseOwnership) {
  NiceMock<MockVariable> mockVariable;
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  mContext.getScopes().setVariable(&mockVariable);
  Identifier identifier("foo", "bar");
  
  EXPECT_EQ(mContext.getScopes().getVariable("foo"), &mockVariable);

  identifier.releaseOwnership(mContext);
  
  EXPECT_EQ(mContext.getScopes().getVariable("foo"), nullptr);
}

TEST_F(IdentifierTest, addReferenceToOwnerTest) {
  NiceMock<MockVariable> mockVariable;
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(mInterface->getOwner()));
  mContext.getScopes().setVariable(&mockVariable);
  NiceMock<MockVariable> referenceVariable;
  ON_CALL(referenceVariable, getName()).WillByDefault(Return("bar"));
  ON_CALL(referenceVariable, getType()).WillByDefault(Return(mInterface));
  mContext.getScopes().setVariable(&referenceVariable);
  
  Identifier* identifier = new Identifier("foo", "foo");
  
  identifier->addReferenceToOwner(mContext, &referenceVariable);
  
  map<string, IVariable*> owners = mContext.getScopes().getOwnersForReference(&referenceVariable);
  EXPECT_EQ(owners.size(), 1u);
  EXPECT_EQ(owners.begin()->second, &mockVariable);
}

TEST_F(IdentifierTest, printToStreamTest) {
  Identifier identifier("foo", "bar");

  stringstream stringStream;
  identifier.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("foo", stringStream.str().c_str());
}
