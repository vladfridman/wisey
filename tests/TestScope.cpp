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

#include "MockOwnerVariable.hpp"
#include "MockReferenceVariable.hpp"
#include "wisey/Scope.hpp"
#include "wisey/IRGenerationContext.hpp"
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
  NiceMock<MockReferenceVariable>* mReferenceVariable;
  NiceMock<MockOwnerVariable>* mOwnerVariable;
  Model* mExceptionModel;
  Interface* mInterface;

public:

  ScopeTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mReferenceVariable(new NiceMock<MockReferenceVariable>()),
  mOwnerVariable(new NiceMock<MockOwnerVariable>()) {
    vector<InterfaceTypeSpecifier*> parentInterfaces;
    vector<IObjectElementDeclaration*> interfaceElements;
    mInterface = Interface::newInterface("systems.vos.wisey.compiler.tests.IInterface",
                                         NULL,
                                         parentInterfaces,
                                         interfaceElements);

    StructType* exceptionModelStructType = StructType::create(mLLVMContext, "MExceptionA");
    mExceptionModel = Model::newModel("MExceptionA", exceptionModelStructType);
    
    ON_CALL(*mReferenceVariable, getType()).WillByDefault(Return(mInterface));
    ON_CALL(*mOwnerVariable, getType()).WillByDefault(Return(mInterface->getOwner()));
  }
};

TEST_F(ScopeTest, localsTest) {
  mScope.setVariable("foo", mReferenceVariable);
  
  EXPECT_EQ(mScope.findVariable("foo"), mReferenceVariable);
  EXPECT_EQ(mScope.findVariable("bar"), nullptr);
  
  mScope.setVariable("bar", mOwnerVariable);
  
  EXPECT_EQ(mScope.findVariable("foo"), mReferenceVariable);
  EXPECT_EQ(mScope.findVariable("bar"), mOwnerVariable);
}

TEST_F(ScopeTest, freeOwnedMemoryTest) {
  mScope.setVariable("bar", mOwnerVariable);
  mScope.setVariable("foo", mReferenceVariable);

  EXPECT_CALL(*mOwnerVariable, free(_));
  EXPECT_CALL(*mReferenceVariable, decrementReferenceCounter(_));
  
  mScope.freeOwnedMemory(mContext);
}

TEST_F(ScopeTest, addExceptionTest) {
  ASSERT_EQ(mScope.getExceptions().size(), 0u);
  
  mScope.addException(mExceptionModel);
  
  ASSERT_EQ(mScope.getExceptions().size(), 1u);
  
  mScope.removeException(mExceptionModel);
  
  ASSERT_EQ(mScope.getExceptions().size(), 0u);

  delete mReferenceVariable;
  delete mOwnerVariable;
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
  
  delete mReferenceVariable;
  delete mOwnerVariable;
}

TEST_F(ScopeTest, getReferenceVariablesTest) {
  mScope.setVariable("foo", mReferenceVariable);
  
  EXPECT_EQ(mScope.getReferenceVariables().size(), 1u);
  EXPECT_EQ(mScope.getOwnerVariables().size(), 0u);
  EXPECT_EQ(mScope.getReferenceVariables().front(), mReferenceVariable);
  
  delete mOwnerVariable;
}

TEST_F(ScopeTest, getOwnerVariablesTest) {
  mScope.setVariable("bar", mOwnerVariable);
  
  EXPECT_EQ(mScope.getReferenceVariables().size(), 0u);
  EXPECT_EQ(mScope.getOwnerVariables().size(), 1u);
  EXPECT_EQ(mScope.getOwnerVariables().front(), mOwnerVariable);
  
  delete mReferenceVariable;
}
