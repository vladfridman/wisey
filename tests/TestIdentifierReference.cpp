//
//  TestIdentifierReference.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/14/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IdentifierReference}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>

#include "MockMethodDescriptor.hpp"
#include "MockObjectType.hpp"
#include "MockOwnerVariable.hpp"
#include "MockReferenceVariable.hpp"
#include "MockType.hpp"
#include "MockVariable.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IdentifierReference.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/UndefinedType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct IdentifierReferenceTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  
  IdentifierReferenceTest() : mLLVMContext(mContext.getLLVMContext()) {
    mContext.getScopes().pushScope();
  }
  
  ~IdentifierReferenceTest() {
  }
};

TEST_F(IdentifierReferenceTest, generateIRTest) {
  NiceMock<MockVariable> mockVariable;
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  mContext.getScopes().setVariable(&mockVariable);
  
  IdentifierReference identifierReference("foo");
  
  EXPECT_CALL(mockVariable, generateIdentifierReferenceIR(_)).Times(1);
  EXPECT_CALL(mockVariable, generateIdentifierIR(_)).Times(0);
  EXPECT_CALL(mockVariable, generateAssignmentIR(_, _, _, _)).Times(0);
  
  identifierReference.generateIR(mContext, PrimitiveTypes::VOID_TYPE);
}

TEST_F(IdentifierReferenceTest, undeclaredVariableDeathTest) {
  IdentifierReference identifierReference("foo");
  
  EXPECT_EXIT(identifierReference.generateIR(mContext, PrimitiveTypes::VOID_TYPE),
              ::testing::ExitedWithCode(1),
              "Undeclared variable 'foo'");
}

TEST_F(IdentifierReferenceTest, getTypeTest) {
  NiceMock<MockVariable> mockVariable;
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(LLVMPrimitiveTypes::I32));
  mContext.getScopes().setVariable(&mockVariable);
  
  IdentifierReference identifierReference("foo");
  
  EXPECT_EQ(identifierReference.getType(mContext), LLVMPrimitiveTypes::I32->getPointerType());
}

TEST_F(IdentifierReferenceTest, getTypeNonNativeTypeTest) {
  NiceMock<MockVariable> mockVariable;
  ::Mock::AllowLeak(&mockVariable);
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  mContext.getScopes().setVariable(&mockVariable);
  
  IdentifierReference identifierReference("foo");
  
  EXPECT_EXIT(identifierReference.getType(mContext),
              ::testing::ExitedWithCode(1),
              "Can not take a reference of a non-native type variable");
}

TEST_F(IdentifierReferenceTest, getTypeForUndefinedTypeTest) {
  IdentifierReference identifierReference("wisey");
  
  const IType* type = identifierReference.getType(mContext);
  
  EXPECT_EQ(UndefinedType::UNDEFINED_TYPE, type);
}

TEST_F(IdentifierReferenceTest, isConstantTest) {
  IdentifierReference identifierReference("foo");

  EXPECT_FALSE(identifierReference.isConstant());
}

TEST_F(IdentifierReferenceTest, printToStreamTest) {
  IdentifierReference identifierReference("foo");
  
  stringstream stringStream;
  identifierReference.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::llvm::reference(foo)", stringStream.str().c_str());
}
