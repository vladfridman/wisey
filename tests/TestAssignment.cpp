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
#include "MockVariable.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/Assignment.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct AssignmentTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockExpression> mExpression;
  Value* mExpressionValue;
  BasicBlock* mBlock;
  Interface* mInterface;
  Controller* mController;

public:
  
  AssignmentTest() : mLLVMContext(mContext.getLLVMContext()) {
    mBlock = BasicBlock::Create(mLLVMContext, "entry");
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();
    mExpressionValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
    ON_CALL(mExpression, generateIR(_)).WillByDefault(Return(mExpressionValue));
    ON_CALL(mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));

    mInterface = new Interface("systems.vos.wisey.compiler.tests.IInterface", NULL);
    
    mController = new Controller("systems.vos.wisey.compiler.tests.CController", NULL);
  }
  
  ~AssignmentTest() {
    delete mBlock;
  }
};

TEST_F(AssignmentTest, variableNotDeclaredDeathTest) {
  Identifier identifier("foo", "bar");
  Assignment assignment(identifier, mExpression);
  Mock::AllowLeak(&mExpression);

  EXPECT_EXIT(assignment.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "undeclared variable foo");
}

TEST_F(AssignmentTest, assignmentExpressionTypeTest) {
  NiceMock<MockVariable> mockVariable;
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  Identifier identifier("foo", "bar");
  Assignment assignment(identifier, mExpression);
  mContext.getScopes().setVariable(&mockVariable);
  ON_CALL(mockVariable, getType()).WillByDefault(Return(PrimitiveTypes::DOUBLE_TYPE));

  EXPECT_EQ(assignment.getType(mContext), PrimitiveTypes::DOUBLE_TYPE);
}

TEST_F(AssignmentTest, generateIRWithInterfaceTypeTest) {
  NiceMock<MockVariable> mockVariable;
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(mInterface));
  mContext.getScopes().setVariable(&mockVariable);
  
  Identifier identifier("foo", "bar");
  Assignment assignment(identifier, mExpression);
  
  EXPECT_CALL(mockVariable, generateIdentifierIR(_, _)).Times(0);
  EXPECT_CALL(mockVariable, generateAssignmentIR(_, _)).Times(1);
  EXPECT_CALL(mExpression, releaseOwnership(_)).Times(1);
  
  assignment.generateIR(mContext);
}

TEST_F(AssignmentTest, generateIRWithPrimitiveTypeTest) {
  NiceMock<MockVariable> mockVariable;
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(PrimitiveTypes::LONG_TYPE));
  mContext.getScopes().setVariable(&mockVariable);
  
  Identifier identifier("foo", "bar");
  Assignment assignment(identifier, mExpression);
  
  EXPECT_CALL(mockVariable, generateIdentifierIR(_, _)).Times(0);
  EXPECT_CALL(mockVariable, generateAssignmentIR(_, _)).Times(1);
  EXPECT_CALL(mExpression, releaseOwnership(_)).Times(0);
  
  assignment.generateIR(mContext);
}

TEST_F(AssignmentTest, releaseOwnershipTest) {
  NiceMock<MockVariable> mockVariable;
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(mInterface));
  mContext.getScopes().setVariable(&mockVariable);
  
  Identifier identifier("foo", "bar");
  Assignment assignment(identifier, mExpression);
  
  assignment.releaseOwnership(mContext);
  
  EXPECT_EQ(mContext.getScopes().getVariable("foo"), nullptr);
}

TEST_F(AssignmentTest, generateIRWithControllerTypeDeathTest) {
  NiceMock<MockVariable> mockVariable;
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(mController));
  mContext.getScopes().setVariable(&mockVariable);
  
  Identifier identifier("foo", "bar");
  Assignment assignment(identifier, mExpression);

  Mock::AllowLeak(&mExpression);
  Mock::AllowLeak(&mockVariable);
  
  EXPECT_EXIT(assignment.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Can not assign to controllers");
}

TEST_F(TestFileSampleRunner, assignToControllerRunDeathTest) {
  expectFailCompile("tests/samples/test_assign_to_controller.yz",
                    1,
                    "Error: Can not assign to controllers");
}
