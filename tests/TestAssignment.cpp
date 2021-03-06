//
//  TestAssignment.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/20/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Assignment}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "MockExpression.hpp"
#include "MockOwnerVariable.hpp"
#include "MockVariable.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "Assignment.hpp"
#include "FakeExpression.hpp"
#include "Identifier.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "LocalReferenceVariable.hpp"
#include "Names.hpp"
#include "PrimitiveTypes.hpp"
#include "ThreadExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct AssignmentTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockExpression>* mExpression;
  Value* mExpressionValue;
  BasicBlock* mBlock;
  Interface* mInterface;
  Controller* mController;

public:
  
  AssignmentTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mExpression(new NiceMock<MockExpression>()) {
    TestPrefix::generateIR(mContext);
    
    mBlock = BasicBlock::Create(mLLVMContext, "entry");
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();
    mExpressionValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
    ON_CALL(*mExpression, generateIR(_, _)).WillByDefault(Return(mExpressionValue));
    ON_CALL(*mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
    ON_CALL(*mExpression, printToStream(_, _)).WillByDefault(Invoke(printExpression));

    vector<IInterfaceTypeSpecifier*> parentInterfaces;
    vector<IObjectElementDefinition*> interfaceElements;
    mInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                         "systems.vos.wisey.compiler.tests.IInterface",
                                         NULL,
                                         parentInterfaces,
                                         interfaceElements,
                                         mContext.getImportProfile(),
                                         0);
    mController = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                            "systems.vos.wisey.compiler.tests.CController",
                                            NULL,
                                            mContext.getImportProfile(),
                                            0);
  }
  
  ~AssignmentTest() {
    delete mBlock;
  }

  static void printExpression(IRGenerationContext& context, iostream& stream) {
    stream << "5";
  }
};

TEST_F(AssignmentTest, isConstantTest) {
  Identifier* identifier = new Identifier("foo", 0);
  Assignment assignment(identifier, mExpression, 0);

  EXPECT_FALSE(assignment.isConstant());
}

TEST_F(AssignmentTest, isAssignableTest) {
  Identifier* identifier = new Identifier("foo", 0);
  Assignment assignment(identifier, mExpression, 0);
  
  EXPECT_TRUE(assignment.isAssignable());
}

TEST_F(AssignmentTest, getVariableTest) {
  NiceMock<MockVariable> mockVariable;
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(PrimitiveTypes::DOUBLE));
  mContext.getScopes().setVariable(mContext, &mockVariable);
  Identifier* identifier = new Identifier("foo", 0);
  Assignment assignment(identifier, mExpression, 0);
  vector<const IExpression*> arrayIndices;

  EXPECT_EQ(identifier->getVariable(mContext, arrayIndices), &mockVariable);
}

TEST_F(AssignmentTest, variableNotDeclaredDeathTest) {
  Identifier* identifier = new Identifier("foo", 0);
  Assignment assignment(identifier, mExpression, 1);
  Mock::AllowLeak(mExpression);

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(assignment.generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(1): Error: Undeclared variable 'foo'\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(AssignmentTest, assignmentExpressionTypeTest) {
  NiceMock<MockVariable> mockVariable;
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(PrimitiveTypes::DOUBLE));
  Identifier* identifier = new Identifier("foo", 0);
  Assignment assignment(identifier, mExpression, 0);
  mContext.getScopes().setVariable(mContext, &mockVariable);

  EXPECT_EQ(assignment.getType(mContext), PrimitiveTypes::DOUBLE);
}

TEST_F(AssignmentTest, generateIRWithInterfaceTypeTest) {
  NiceMock<MockVariable> mockVariable;
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(mInterface->getOwner()));
  mContext.getScopes().setVariable(mContext, &mockVariable);
  
  Identifier* identifier = new Identifier("foo", 0);
  Assignment assignment(identifier, mExpression, 0);
  
  EXPECT_CALL(mockVariable, generateIdentifierIR(_, _)).Times(0);
  EXPECT_CALL(mockVariable, generateAssignmentIR(_, _, _, _)).Times(1);
  
  assignment.generateIR(mContext, PrimitiveTypes::VOID);
}

TEST_F(AssignmentTest, generateIRWithPrimitiveTypeTest) {
  NiceMock<MockVariable> mockVariable;
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(PrimitiveTypes::LONG));
  mContext.getScopes().setVariable(mContext, &mockVariable);
  
  Identifier* identifier = new Identifier("foo", 0);
  Assignment assignment(identifier, mExpression, 0);
  
  EXPECT_CALL(mockVariable, generateIdentifierIR(_, _)).Times(0);
  EXPECT_CALL(mockVariable, generateAssignmentIR(_, _, _, _)).Times(1);
  
  assignment.generateIR(mContext, PrimitiveTypes::VOID);
}

TEST_F(AssignmentTest, generateIRWithOwnerTypeTest) {
  NiceMock<MockOwnerVariable> mockVariable;
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(mInterface->getOwner()));
  mContext.getScopes().setVariable(mContext, &mockVariable);
  Identifier* identifier = new Identifier("foo", 0);
  Assignment assignment(identifier, mExpression, 0);
  EXPECT_CALL(mockVariable, setToNull(_, _));
  
  assignment.generateIR(mContext, mInterface->getOwner());
}

TEST_F(AssignmentTest, printToStreamTest) {
  NiceMock<MockVariable> mockVariable;
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(PrimitiveTypes::DOUBLE));
  mContext.getScopes().setVariable(mContext, &mockVariable);
  Identifier* identifier = new Identifier("foo", 0);
  Assignment assignment(identifier, mExpression, 0);

  stringstream stringStream;
  assignment.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("foo = 5", stringStream.str().c_str());
}

TEST_F(TestFileRunner, assignToAssignmentPrimitiveRunTest) {
  runFile("tests/samples/test_assignment_to_assignment_primitive.yz", 5);
}

TEST_F(TestFileRunner, assignToAssignmentObjectRunTest) {
  runFile("tests/samples/test_assingment_to_assignment_object.yz", 3);
}

