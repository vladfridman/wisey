//
//  TestScope.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Scope}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>

#include "MockVariable.hpp"
#include "wisey/Scope.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/StackVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ScopeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Scope mScope;
  NiceMock<MockVariable>* mFooVariable;
  NiceMock<MockVariable>* mBarVariable;
  Model* mExceptionModel;
  Interface* mInterface;

public:

  ScopeTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mFooVariable(new NiceMock<MockVariable>()),
  mBarVariable(new NiceMock<MockVariable>()) {
    vector<InterfaceTypeSpecifier*> parentInterfaces;
    vector<MethodSignatureDeclaration*> interfaceMethods;
    mInterface = Interface::newInterface("systems.vos.wisey.compiler.tests.IInterface",
                                         NULL,
                                         parentInterfaces,
                                         interfaceMethods);

    StructType* exceptionModelStructType = StructType::create(mLLVMContext, "MExceptionA");
    mExceptionModel = Model::newModel("MExceptionA", exceptionModelStructType);
    
    ON_CALL(*mFooVariable, getType()).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    ON_CALL(*mBarVariable, getType()).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  }
};

TEST_F(ScopeTest, localsTest) {
  mScope.setVariable("foo", mFooVariable);
  
  EXPECT_EQ(mScope.findVariable("foo"), mFooVariable);
  EXPECT_EQ(mScope.findVariable("bar"), nullptr);
  
  mScope.setVariable("bar", mBarVariable);
  
  EXPECT_EQ(mScope.findVariable("foo"), mFooVariable);
  EXPECT_EQ(mScope.findVariable("bar"), mBarVariable);
}

TEST_F(ScopeTest, freeOwnedMemoryTest) {
  mScope.setVariable("foo", mFooVariable);
  
  EXPECT_CALL(*mFooVariable, free(_));
  
  map<string, IVariable*> clearedVariables;
  mScope.freeOwnedMemory(mContext, clearedVariables);

  delete mBarVariable;
}

TEST_F(ScopeTest, addExceptionTest) {
  ASSERT_EQ(mScope.getExceptions().size(), 0u);
  
  mScope.addException(mExceptionModel);
  
  ASSERT_EQ(mScope.getExceptions().size(), 1u);
  
  mScope.removeException(mExceptionModel);
  
  ASSERT_EQ(mScope.getExceptions().size(), 0u);

  delete mFooVariable;
  delete mBarVariable;
}

TEST_F(ScopeTest, addExceptionsTest) {
  StructType* exceptionModelStructType = StructType::create(mLLVMContext, "MExceptionB");
  Model* exceptionModel = Model::newModel("MExceptionB", exceptionModelStructType);

  ASSERT_EQ(mScope.getExceptions().size(), 0u);
  
  vector<const Model*> exceptions;
  exceptions.push_back(mExceptionModel);
  exceptions.push_back(exceptionModel);
  
  mScope.addExceptions(exceptions);
  
  ASSERT_EQ(mScope.getExceptions().size(), 2u);
  
  mScope.removeException(mExceptionModel);
  mScope.removeException(exceptionModel);
  
  ASSERT_EQ(mScope.getExceptions().size(), 0u);
  
  delete mFooVariable;
  delete mBarVariable;
}

TEST_F(ScopeTest, getClearedVariablesTest) {
  mScope.setVariable("foo", mFooVariable);
  mScope.setVariable("bar", mBarVariable);
  map<string, IVariable*> allClearedVariables;
  allClearedVariables["foo"] = mFooVariable;

  vector<string> clearedVariables = mScope.getClearedVariables(allClearedVariables);
  EXPECT_EQ(clearedVariables.size(), 1u);
  EXPECT_STREQ(clearedVariables.at(0).c_str(), "foo");
}

TEST_F(ScopeTest, getReferenceVariablesTest) {
  ON_CALL(*mFooVariable, getType()).WillByDefault(Return(mInterface));
  mScope.setVariable("foo", mFooVariable);
  
  EXPECT_EQ(mScope.getReferenceVariables().size(), 1u);
  EXPECT_EQ(mScope.getOwnerVariables().size(), 0u);
  EXPECT_EQ(mScope.getReferenceVariables().front(), mFooVariable);
  
  delete mBarVariable;
}

TEST_F(ScopeTest, getOwnerVariablesTest) {
  ON_CALL(*mFooVariable, getType()).WillByDefault(Return(mInterface->getOwner()));
  mScope.setVariable("foo", mFooVariable);
  
  EXPECT_EQ(mScope.getReferenceVariables().size(), 0u);
  EXPECT_EQ(mScope.getOwnerVariables().size(), 1u);
  EXPECT_EQ(mScope.getOwnerVariables().front(), mFooVariable);
  
  delete mBarVariable;
}
