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
#include "TestPrefix.hpp"
#include "IRGenerationContext.hpp"
#include "IdentifierReference.hpp"
#include "LLVMPrimitiveTypes.hpp"
#include "PrimitiveTypes.hpp"
#include "UndefinedType.hpp"

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
    TestPrefix::generateIR(mContext);
    
    mContext.getScopes().pushScope();
  }
  
  ~IdentifierReferenceTest() {
  }
};

TEST_F(IdentifierReferenceTest, generateIRTest) {
  NiceMock<MockVariable> mockVariable;
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(PrimitiveTypes::INT));
  mContext.getScopes().setVariable(mContext, &mockVariable);
  
  IdentifierReference identifierReference("foo", 0);
  
  EXPECT_CALL(mockVariable, generateIdentifierReferenceIR(_, _)).Times(1);
  EXPECT_CALL(mockVariable, generateIdentifierIR(_, _)).Times(0);
  EXPECT_CALL(mockVariable, generateAssignmentIR(_, _, _, _)).Times(0);
  
  identifierReference.generateIR(mContext, PrimitiveTypes::VOID);
}

TEST_F(IdentifierReferenceTest, undeclaredVariableDeathTest) {
  IdentifierReference identifierReference("foo", 5);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(identifierReference.generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(5): Error: Undeclared variable 'foo'\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(IdentifierReferenceTest, getTypeTest) {
  NiceMock<MockVariable> mockVariable;
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(LLVMPrimitiveTypes::I32));
  mContext.getScopes().setVariable(mContext, &mockVariable);
  
  IdentifierReference identifierReference("foo", 0);
  
  EXPECT_EQ(identifierReference.getType(mContext),
            LLVMPrimitiveTypes::I32->getPointerType(mContext, 0));
}

TEST_F(IdentifierReferenceTest, getTypeNonNativeTypeTest) {
  NiceMock<MockVariable> mockVariable;
  ::Mock::AllowLeak(&mockVariable);
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(PrimitiveTypes::INT));
  mContext.getScopes().setVariable(mContext, &mockVariable);
  
  IdentifierReference identifierReference("foo", 5);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(identifierReference.getType(mContext));
  EXPECT_STREQ("/tmp/source.yz(5): Error: Can not take a reference of a non-native type variable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(IdentifierReferenceTest, getTypeForUndefinedTypeTest) {
  IdentifierReference identifierReference("wisey", 0);
  
  const IType* type = identifierReference.getType(mContext);
  
  EXPECT_EQ(UndefinedType::UNDEFINED, type);
}

TEST_F(IdentifierReferenceTest, isConstantTest) {
  IdentifierReference identifierReference("foo", 0);
  
  EXPECT_FALSE(identifierReference.isConstant());
}

TEST_F(IdentifierReferenceTest, isAssignableTest) {
  IdentifierReference identifierReference("foo", 0);
  
  EXPECT_FALSE(identifierReference.isAssignable());
}

TEST_F(IdentifierReferenceTest, printToStreamTest) {
  IdentifierReference identifierReference("foo", 0);
  
  stringstream stringStream;
  identifierReference.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::llvm::reference(foo)", stringStream.str().c_str());
}
