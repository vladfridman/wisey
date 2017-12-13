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
#include "TestFileSampleRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/Assignment.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalReferenceVariable.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ThreadExpression.hpp"

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
  NiceMock<MockVariable>* mThreadVariable;

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
    ON_CALL(*mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    ON_CALL(*mExpression, printToStream(_, _)).WillByDefault(Invoke(printExpression));

    vector<IInterfaceTypeSpecifier*> parentInterfaces;
    vector<IObjectElementDeclaration*> interfaceElements;
    mInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                         "systems.vos.wisey.compiler.tests.IInterface",
                                         NULL,
                                         parentInterfaces,
                                         interfaceElements);
    mController = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                            "systems.vos.wisey.compiler.tests.CController",
                                            NULL);

    Controller* threadController = mContext.getController(Names::getThreadControllerFullName());
    Value* threadObject = ConstantPointerNull::get(threadController->getLLVMType(mContext));
    mThreadVariable = new NiceMock<MockVariable>();
    ON_CALL(*mThreadVariable, getName()).WillByDefault(Return(ThreadExpression::THREAD));
    ON_CALL(*mThreadVariable, getType()).WillByDefault(Return(threadController));
    ON_CALL(*mThreadVariable, generateIdentifierIR(_)).WillByDefault(Return(threadObject));
    mContext.getScopes().setVariable(mThreadVariable);
  }
  
  ~AssignmentTest() {
    delete mBlock;
    delete mThreadVariable;
  }

  static void printExpression(IRGenerationContext& context, iostream& stream) {
    stream << "5";
  }
};

TEST_F(AssignmentTest, isConstantTest) {
  Identifier* identifier = new Identifier("foo");
  Assignment assignment(identifier, mExpression, 0);

  EXPECT_FALSE(assignment.isConstant());
}

TEST_F(AssignmentTest, getVariableTest) {
  NiceMock<MockVariable> mockVariable;
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(PrimitiveTypes::DOUBLE_TYPE));
  mContext.getScopes().setVariable(&mockVariable);
  Identifier* identifier = new Identifier("foo");
  Assignment assignment(identifier, mExpression, 0);

  EXPECT_EQ(identifier->getVariable(mContext), &mockVariable);
}

TEST_F(AssignmentTest, variableNotDeclaredDeathTest) {
  Identifier* identifier = new Identifier("foo");
  Assignment assignment(identifier, mExpression, 0);
  Mock::AllowLeak(mExpression);
  Mock::AllowLeak(mThreadVariable);

  EXPECT_EXIT(assignment.generateIR(mContext, IR_GENERATION_NORMAL),
              ::testing::ExitedWithCode(1),
              "Error: Undeclared variable 'foo'");
}

TEST_F(AssignmentTest, assignmentExpressionTypeTest) {
  NiceMock<MockVariable> mockVariable;
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(PrimitiveTypes::DOUBLE_TYPE));
  Identifier* identifier = new Identifier("foo");
  Assignment assignment(identifier, mExpression, 0);
  mContext.getScopes().setVariable(&mockVariable);

  EXPECT_EQ(assignment.getType(mContext), PrimitiveTypes::DOUBLE_TYPE);
}

TEST_F(AssignmentTest, generateIRWithInterfaceTypeTest) {
  NiceMock<MockVariable> mockVariable;
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(mInterface->getOwner()));
  mContext.getScopes().setVariable(&mockVariable);
  
  Identifier* identifier = new Identifier("foo");
  Assignment assignment(identifier, mExpression, 0);
  
  EXPECT_CALL(mockVariable, generateIdentifierIR(_)).Times(0);
  EXPECT_CALL(mockVariable, generateAssignmentIR(_, _, _)).Times(1);
  
  assignment.generateIR(mContext, IR_GENERATION_NORMAL);
}

TEST_F(AssignmentTest, generateIRWithPrimitiveTypeTest) {
  NiceMock<MockVariable> mockVariable;
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(PrimitiveTypes::LONG_TYPE));
  mContext.getScopes().setVariable(&mockVariable);
  
  Identifier* identifier = new Identifier("foo");
  Assignment assignment(identifier, mExpression, 0);
  
  EXPECT_CALL(mockVariable, generateIdentifierIR(_)).Times(0);
  EXPECT_CALL(mockVariable, generateAssignmentIR(_, _, _)).Times(1);
  
  assignment.generateIR(mContext, IR_GENERATION_NORMAL);
}

TEST_F(AssignmentTest, generateIRWithOwnerTypeTest) {
  NiceMock<MockOwnerVariable> mockVariable;
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(mInterface->getOwner()));
  mContext.getScopes().setVariable(&mockVariable);
  Identifier* identifier = new Identifier("foo");
  Assignment assignment(identifier, mExpression, 0);
  EXPECT_CALL(mockVariable, setToNull(_));
  
  assignment.generateIR(mContext, IR_GENERATION_RELEASE);
}

TEST_F(AssignmentTest, printToStreamTest) {
  NiceMock<MockVariable> mockVariable;
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(PrimitiveTypes::DOUBLE_TYPE));
  mContext.getScopes().setVariable(&mockVariable);
  Identifier* identifier = new Identifier("foo");
  Assignment assignment(identifier, mExpression, 0);

  stringstream stringStream;
  assignment.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("foo = 5", stringStream.str().c_str());
}

TEST_F(TestFileSampleRunner, assignToAssignmentPrimitiveRunTest) {
  runFile("tests/samples/test_assignment_to_assignment_primitive.yz", "5");
}

TEST_F(TestFileSampleRunner, assignToAssignmentObjectRunTest) {
  runFile("tests/samples/test_assingment_to_assignment_object.yz", "3");
}

TEST_F(TestFileSampleRunner, assignToControllerRunDeathTest) {
  expectFailCompile("tests/samples/test_assign_to_controller.yz",
                    1,
                    "Error: Can not assign to field mAdder1");
}
