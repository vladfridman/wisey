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

#include "MockOwnerVariable.hpp"
#include "MockVariable.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LocalPrimitiveVariable.hpp"
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
    vector<IObjectElementDeclaration*> interfaceElements;
    mInterface = Interface::newInterface("systems.vos.wisey.compiler.tests.IInterface",
                                         NULL,
                                         parentInterfaces,
                                         interfaceElements);

    mContext.getScopes().pushScope();
  }
  
  ~IdentifierTest() { }
};

TEST_F(IdentifierTest, getVariableTest) {
  LocalPrimitiveVariable* variable = new LocalPrimitiveVariable("foo",
                                                                PrimitiveTypes::INT_TYPE,
                                                                NULL);
  mContext.getScopes().setVariable(variable);
  Identifier identifier("foo");

  EXPECT_EQ(identifier.getVariable(mContext), variable);
}

TEST_F(IdentifierTest, undeclaredVariableDeathTest) {
  Identifier identifier("foo");

  EXPECT_EXIT(identifier.generateIR(mContext, IR_GENERATION_NORMAL),
              ::testing::ExitedWithCode(1),
              "Undeclared variable 'foo'");
}

TEST_F(IdentifierTest, getTypeTest) {
  LocalPrimitiveVariable* variable = new LocalPrimitiveVariable("foo",
                                                                PrimitiveTypes::INT_TYPE,
                                                                NULL);
  mContext.getScopes().setVariable(variable);
  Identifier identifier("foo");

  EXPECT_EQ(identifier.getType(mContext), PrimitiveTypes::INT_TYPE);
}

TEST_F(IdentifierTest, generateIRTest) {
  NiceMock<MockVariable> mockVariable;
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  mContext.getScopes().setVariable(&mockVariable);
  
  Identifier identifier("foo");
  
  EXPECT_CALL(mockVariable, generateIdentifierIR(_)).Times(1);
  EXPECT_CALL(mockVariable, generateAssignmentIR(_, _)).Times(0);
  
  identifier.generateIR(mContext, IR_GENERATION_NORMAL);
}

TEST_F(IdentifierTest, releaseOwnershipTest) {
  NiceMock<MockOwnerVariable> mockVariable;
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(mInterface->getOwner()));
  mContext.getScopes().setVariable(&mockVariable);
  Identifier identifier("foo");
  
  EXPECT_EQ(mContext.getScopes().getVariable("foo"), &mockVariable);
  EXPECT_CALL(mockVariable, setToNull(_));

  identifier.releaseOwnership(mContext);
}

TEST_F(IdentifierTest, isConstantTest) {
  Identifier* identifier = new Identifier("foo");

  EXPECT_FALSE(identifier->isConstant());
}

TEST_F(IdentifierTest, printToStreamTest) {
  Identifier identifier("foo");

  stringstream stringStream;
  identifier.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("foo", stringStream.str().c_str());
}
