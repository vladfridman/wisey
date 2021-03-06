//
//  TestIdentifierChain.cpp
//  runtests
//
//  Created by Vladimir Fridman on 12/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IdentifierChain}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockOwnerVariable.hpp"
#include "MockExpression.hpp"
#include "MockObjectType.hpp"
#include "MockMethodDescriptor.hpp"
#include "MockType.hpp"
#include "MockVariable.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "IInterfaceTypeSpecifier.hpp"
#include "IRGenerationContext.hpp"
#include "IdentifierChain.hpp"
#include "LocalPrimitiveVariable.hpp"
#include "PrimitiveTypes.hpp"
#include "UndefinedType.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct IdentifierChainTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  
  IdentifierChainTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
  }
  
  ~IdentifierChainTest() {
  }

  static void printUndefinedTypeExpression(IRGenerationContext& context, iostream& stream) {
    stream << "wisey";
  }

  static void printPackageTypeExpression(IRGenerationContext& context, iostream& stream) {
    stream << "systems.vos.wisey.compiler";
  }

  static void printObjectTypeExpression(IRGenerationContext& context, iostream& stream) {
    stream << "object";
  }
};

TEST_F(IdentifierChainTest, isConstantTest) {
  NiceMock<MockExpression>* mockExpression = new NiceMock<MockExpression>();
  ON_CALL(*mockExpression, printToStream(_, _)).WillByDefault(Invoke(printPackageTypeExpression));
  IdentifierChain identifierChain(mockExpression, "test", 0);
  
  EXPECT_FALSE(identifierChain.isConstant());
}

TEST_F(IdentifierChainTest, isAssignableTest) {
  NiceMock<MockExpression>* mockExpression = new NiceMock<MockExpression>();
  ON_CALL(*mockExpression, printToStream(_, _)).WillByDefault(Invoke(printPackageTypeExpression));
  IdentifierChain identifierChain(mockExpression, "test", 0);
  
  EXPECT_FALSE(identifierChain.isAssignable());
}

TEST_F(IdentifierChainTest, printToStreamTest) {
  NiceMock<MockExpression>* mockExpression = new NiceMock<MockExpression>();
  ON_CALL(*mockExpression, printToStream(_, _)).WillByDefault(Invoke(printPackageTypeExpression));
  IdentifierChain identifierChain(mockExpression, "test", 0);
  
  stringstream stringStream;
  identifierChain.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("systems.vos.wisey.compiler.test", stringStream.str().c_str());
}

TEST_F(IdentifierChainTest, getTypeForUndefinedBaseTypeTest) {
  NiceMock<MockExpression>* mockExpression = new NiceMock<MockExpression>();
  ON_CALL(*mockExpression, getType(_)).WillByDefault(Return(UndefinedType::UNDEFINED));
  ON_CALL(*mockExpression, printToStream(_, _)).WillByDefault(Invoke(printUndefinedTypeExpression));
  
  IdentifierChain identifierChain(mockExpression, "lang", 0);
  const IType* type = identifierChain.getType(mContext);
  
  EXPECT_TRUE(type->isPackage());
  EXPECT_STREQ("wisey.lang", type->getTypeName().c_str());
}

TEST_F(IdentifierChainTest, getTypeForPackageBaseTypeTest) {
  NiceMock<MockExpression>* mockExpression = new NiceMock<MockExpression>();
  NiceMock<MockType> mockType;
  ON_CALL(*mockExpression, getType(_)).WillByDefault(Return(&mockType));
  ON_CALL(*mockExpression, printToStream(_, _)).WillByDefault(Invoke(printPackageTypeExpression));
  ON_CALL(mockType, isPackage()).WillByDefault(Return(true));
  EXPECT_CALL(mockType, die());
  
  IdentifierChain identifierChain(mockExpression, "tests", 0);
  const IType* type = identifierChain.getType(mContext);
  
  EXPECT_TRUE(type->isPackage());
  EXPECT_STREQ("systems.vos.wisey.compiler.tests", type->getTypeName().c_str());
}

TEST_F(IdentifierChainTest, getTypeForObjectBaseTypeTest) {
  NiceMock<MockExpression>* mockExpression = new NiceMock<MockExpression>();
  NiceMock<MockObjectType> mockObjectType;
  NiceMock<MockMethodDescriptor> mockMethodDescriptor;
  ON_CALL(*mockExpression, getType(_)).WillByDefault(Return(&mockObjectType));
  ON_CALL(*mockExpression, printToStream(_, _)).WillByDefault(Invoke(printObjectTypeExpression));
  ON_CALL(mockObjectType, isModel()).WillByDefault(Return(true));
  ON_CALL(mockObjectType, isReference()).WillByDefault(Return(true));
  ON_CALL(mockObjectType, findMethod("foo")).WillByDefault(Return(&mockMethodDescriptor));
  ON_CALL(mockMethodDescriptor, getParentObject()).WillByDefault(Return(&mockObjectType));

  IdentifierChain identifierChain(mockExpression, "foo", 0);
  const IType* type = identifierChain.getType(mContext);
  
  EXPECT_EQ(&mockMethodDescriptor, type);
}

TEST_F(IdentifierChainTest, getTypeForPrimitiveBaseTypeDeathTest) {
  NiceMock<MockExpression>* mockExpression = new NiceMock<MockExpression>();
  NiceMock<MockObjectType> mockObjectType;
  ON_CALL(*mockExpression, getType(_)).WillByDefault(Return(&mockObjectType));
  ON_CALL(*mockExpression, printToStream(_, _)).WillByDefault(Invoke(printObjectTypeExpression));
  ON_CALL(mockObjectType, isPrimitive()).WillByDefault(Return(true));
  
  IdentifierChain identifierChain(mockExpression, "foo", 5);
  
  Mock::AllowLeak(mockExpression);
  Mock::AllowLeak(&mockObjectType);
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(identifierChain.getType(mContext));
  EXPECT_STREQ("/tmp/source.yz(5): Error: Attempt to call a method 'foo' on an expression that is not of object type\n",
              buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(IdentifierChainTest, generateIRForUndefinedBaseTypeDeathTest) {
  NiceMock<MockExpression>* mockExpression = new NiceMock<MockExpression>();
  ON_CALL(*mockExpression, getType(_)).WillByDefault(Return(UndefinedType::UNDEFINED));
  ON_CALL(*mockExpression, printToStream(_, _)).WillByDefault(Invoke(printObjectTypeExpression));
  
  IdentifierChain identifierChain(mockExpression, "foo", 3);
  Mock::AllowLeak(mockExpression);
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(identifierChain.generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Attempt to call a method 'foo' on undefined type expression\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(IdentifierChainTest, getTypeForObjectBaseTypeMethodNotFoundDeathTest) {
  NiceMock<MockExpression>* mockExpression = new NiceMock<MockExpression>();
  NiceMock<MockObjectType> mockObjectType;
  string modelFullName = "systems.vos.wisey.compiler.tests.MObject";
  ON_CALL(*mockExpression, getType(_)).WillByDefault(Return(&mockObjectType));
  ON_CALL(*mockExpression, printToStream(_, _)).WillByDefault(Invoke(printObjectTypeExpression));
  ON_CALL(mockObjectType, isModel()).WillByDefault(Return(true));
  ON_CALL(mockObjectType, isReference()).WillByDefault(Return(true));
  ON_CALL(mockObjectType, getTypeName()).WillByDefault(Return(modelFullName));

  IdentifierChain identifierChain(mockExpression, "foo", 1);

  Mock::AllowLeak(mockExpression);
  Mock::AllowLeak(&mockObjectType);
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(identifierChain.getType(mContext));
  EXPECT_STREQ("/tmp/source.yz(1): Error: Method 'foo' is not found in object systems.vos.wisey.compiler.tests.MObject\n",
              buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(IdentifierChainTest, generateIRTest) {
  NiceMock<MockExpression>* mockExpression = new NiceMock<MockExpression>();
  NiceMock<MockObjectType> mockObjectType;
  NiceMock<MockMethodDescriptor> mockMethodDescriptor;
  string modelFullName = "systems.vos.wisey.compiler.tests.MObject";
  StructType* modelStructType = StructType::create(mLLVMContext, modelFullName);
  Value* objectPointer = ConstantPointerNull::get(modelStructType->getPointerTo());
  ON_CALL(*mockExpression, getType(_)).WillByDefault(Return(&mockObjectType));
  ON_CALL(*mockExpression, printToStream(_, _)).WillByDefault(Invoke(printObjectTypeExpression));
  ON_CALL(mockObjectType, isModel()).WillByDefault(Return(true));
  ON_CALL(mockObjectType, isReference()).WillByDefault(Return(true));
  ON_CALL(mockObjectType, findMethod("foo")).WillByDefault(Return(&mockMethodDescriptor));
  ON_CALL(mockObjectType, getTypeName()).WillByDefault(Return(modelFullName));
  ON_CALL(mockMethodDescriptor, getParentObject()).WillByDefault(Return(&mockObjectType));
  ON_CALL(mockMethodDescriptor, isPublic()).WillByDefault(Return(true));
  ON_CALL(*mockExpression, generateIR(_, _)).WillByDefault(Return(objectPointer));

  IdentifierChain identifierChain(mockExpression, "foo", 0);
  Value* result = identifierChain.generateIR(mContext, PrimitiveTypes::VOID);
  
  EXPECT_EQ(objectPointer, result);
}

TEST_F(IdentifierChainTest, generateIRForPrivateMethodDeathTest) {
  NiceMock<MockExpression>* mockExpression = new NiceMock<MockExpression>();
  NiceMock<MockObjectType> mockObjectType;
  NiceMock<MockMethodDescriptor> mockMethodDescriptor;
  string modelFullName = "systems.vos.wisey.compiler.tests.MObject";
  ON_CALL(*mockExpression, getType(_)).WillByDefault(Return(&mockObjectType));
  ON_CALL(*mockExpression, printToStream(_, _)).WillByDefault(Invoke(printObjectTypeExpression));
  ON_CALL(mockObjectType, isModel()).WillByDefault(Return(true));
  ON_CALL(mockObjectType, isReference()).WillByDefault(Return(true));
  ON_CALL(mockObjectType, findMethod("foo")).WillByDefault(Return(&mockMethodDescriptor));
  ON_CALL(mockObjectType, getTypeName()).WillByDefault(Return(modelFullName));
  ON_CALL(mockMethodDescriptor, getParentObject()).WillByDefault(Return(&mockObjectType));
  
  Mock::AllowLeak(mockExpression);
  Mock::AllowLeak(&mockObjectType);
  Mock::AllowLeak(&mockMethodDescriptor);
  IdentifierChain identifierChain(mockExpression, "foo", 3);
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(identifierChain.generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Method 'foo' of object systems.vos.wisey.compiler.tests.MObject is private\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}
