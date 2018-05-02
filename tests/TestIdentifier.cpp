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

#include "MockMethodDescriptor.hpp"
#include "MockObjectType.hpp"
#include "MockOwnerVariable.hpp"
#include "MockReferenceVariable.hpp"
#include "MockType.hpp"
#include "MockVariable.hpp"
#include "TestPrefix.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/Identifier.hpp"
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

struct IdentifierTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;

  IdentifierTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    mContext.getScopes().pushScope();
  }
  
  ~IdentifierTest() {
  }
};

TEST_F(IdentifierTest, generateIRForPrimitiveVariableTest) {
  NiceMock<MockVariable> mockVariable;
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(PrimitiveTypes::INT));
  mContext.getScopes().setVariable(&mockVariable);
  
  Identifier identifier("foo", 0);
  
  EXPECT_CALL(mockVariable, generateIdentifierIR(_)).Times(1);
  EXPECT_CALL(mockVariable, generateAssignmentIR(_, _, _, _)).Times(0);
  
  identifier.generateIR(mContext, PrimitiveTypes::VOID);
}

TEST_F(IdentifierTest, undeclaredVariableDeathTest) {
  Identifier identifier("foo", 0);

  EXPECT_EXIT(identifier.generateIR(mContext, PrimitiveTypes::VOID),
              ::testing::ExitedWithCode(1),
              "Undeclared variable 'foo'");
}

TEST_F(IdentifierTest, generateIRForObjectOwnerVariableSetToNullTest) {
  NiceMock<MockOwnerVariable> mockVariable;
  NiceMock<MockType> mockType;
  string modelFullName = "systems.vos.wisey.compiler.tests.MObject";
  StructType* modelStructType = StructType::create(mLLVMContext, modelFullName);
  Value* objectPointer = ConstantPointerNull::get(modelStructType->getPointerTo());
  ON_CALL(mockVariable, getType()).WillByDefault(Return(&mockType));
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  ON_CALL(mockVariable, generateIdentifierIR(_)).WillByDefault(Return(objectPointer));
  ON_CALL(mockType, isModel()).WillByDefault(Return(true));
  ON_CALL(mockType, isOwner()).WillByDefault(Return(true));
  EXPECT_CALL(mockVariable, setToNull(_, _));
  EXPECT_CALL(mockType, die());
  mContext.getScopes().setVariable(&mockVariable);
  Model* model = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                                 modelFullName,
                                 modelStructType,
                                 mContext.getImportProfile(),
                                 0);

  Identifier identifier("foo", 0);
  Value* result = identifier.generateIR(mContext, model->getOwner());

  EXPECT_EQ(objectPointer, result);
}

TEST_F(IdentifierTest, generateIRForMethodTest) {
  NiceMock<MockObjectType> mockObjecType;
  NiceMock<MockMethodDescriptor> mockMethodDescriptor;
  NiceMock<MockReferenceVariable> mockVariable;
  string modelFullName = "systems.vos.wisey.compiler.tests.MObject";
  StructType* modelStructType = StructType::create(mLLVMContext, modelFullName);
  Value* objectPointer = ConstantPointerNull::get(modelStructType->getPointerTo());
  ON_CALL(mockObjecType, findMethod("foo")).WillByDefault(Return(&mockMethodDescriptor));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(&mockObjecType));
  ON_CALL(mockVariable, getName()).WillByDefault(Return(IObjectType::THIS));
  ON_CALL(mockVariable, generateIdentifierIR(_)).WillByDefault(Return(objectPointer));
  mContext.setObjectType(&mockObjecType);
  mContext.getScopes().setVariable(&mockVariable);

  Identifier identifier("foo", 0);
  Value* result = identifier.generateIR(mContext, PrimitiveTypes::VOID);
  
  EXPECT_EQ(objectPointer, result);
}

TEST_F(IdentifierTest, getTypeForMethodTest) {
  NiceMock<MockObjectType> mockObjecType;
  NiceMock<MockMethodDescriptor> mockMethodDescriptor;
  NiceMock<MockReferenceVariable> mockVariable;
  ON_CALL(mockObjecType, findMethod("foo")).WillByDefault(Return(&mockMethodDescriptor));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(&mockObjecType));
  ON_CALL(mockVariable, getName()).WillByDefault(Return(IObjectType::THIS));
  mContext.setObjectType(&mockObjecType);
  mContext.getScopes().setVariable(&mockVariable);
  
  Identifier identifier("foo", 0);
  const IType* type = identifier.getType(mContext);
  
  EXPECT_EQ(&mockMethodDescriptor, type);
}

TEST_F(IdentifierTest, getTypeForPrimitiveVariableTest) {
  NiceMock<MockVariable> mockVariable;
  ON_CALL(mockVariable, getName()).WillByDefault(Return("foo"));
  ON_CALL(mockVariable, getType()).WillByDefault(Return(PrimitiveTypes::INT));
  mContext.getScopes().setVariable(&mockVariable);
  
  Identifier identifier("foo", 0);

  EXPECT_EQ(identifier.getType(mContext), PrimitiveTypes::INT);
}

TEST_F(IdentifierTest, getTypeForUndefinedTypeTest) {
  Identifier identifier("wisey", 0);
  
  const IType* type = identifier.getType(mContext);
  
  EXPECT_EQ(UndefinedType::UNDEFINED_TYPE, type);
}

TEST_F(IdentifierTest, isConstantTest) {
  Identifier* identifier = new Identifier("foo", 0);
  
  EXPECT_FALSE(identifier->isConstant());
}

TEST_F(IdentifierTest, isAssignableTest) {
  Identifier* identifier = new Identifier("foo", 0);
  
  EXPECT_TRUE(identifier->isAssignable());
}

TEST_F(IdentifierTest, printToStreamTest) {
  Identifier identifier("foo", 0);

  stringstream stringStream;
  identifier.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("foo", stringStream.str().c_str());
}
