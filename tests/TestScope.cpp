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
#include "TestFileRunner.hpp"
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
    vector<IInterfaceTypeSpecifier*> parentInterfaces;
    vector<IObjectElementDefinition*> interfaceElements;
    mInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                         "systems.vos.wisey.compiler.tests.IInterface",
                                         NULL,
                                         parentInterfaces,
                                         interfaceElements);

    StructType* exceptionModelStructType = StructType::create(mLLVMContext, "MExceptionA");
    mExceptionModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                                      "MExceptionA",
                                      exceptionModelStructType);
    
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
  
  mScope.freeOwnedMemory(mContext, 0);
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
  Model* exceptionModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                                          "MExceptionB",
                                          exceptionModelStructType);

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

TEST_F(TestFileRunner, destroyDependencyRunDeathTest) {
  runFile("tests/samples/test_destroy_dependency.yz", "5");
}

