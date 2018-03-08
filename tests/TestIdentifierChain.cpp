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
#include "TestFileSampleRunner.hpp"
#include "wisey/IInterfaceTypeSpecifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IdentifierChain.hpp"
#include "wisey/LocalPrimitiveVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/UndefinedType.hpp"

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
  IdentifierChain identifierChain(mockExpression, "test");
  
  EXPECT_FALSE(identifierChain.isConstant());
}

TEST_F(IdentifierChainTest, printToStreamTest) {
  NiceMock<MockExpression>* mockExpression = new NiceMock<MockExpression>();
  ON_CALL(*mockExpression, printToStream(_, _)).WillByDefault(Invoke(printPackageTypeExpression));
  IdentifierChain identifierChain(mockExpression, "test");
  
  stringstream stringStream;
  identifierChain.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("systems.vos.wisey.compiler.test", stringStream.str().c_str());
}

TEST_F(IdentifierChainTest, getTypeForUndefinedBaseTypeTest) {
  NiceMock<MockExpression>* mockExpression = new NiceMock<MockExpression>();
  ON_CALL(*mockExpression, getType(_)).WillByDefault(Return(UndefinedType::UNDEFINED_TYPE));
  ON_CALL(*mockExpression, printToStream(_, _)).WillByDefault(Invoke(printUndefinedTypeExpression));
  
  IdentifierChain identifierChain(mockExpression, "lang");
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
  
  IdentifierChain identifierChain(mockExpression, "tests");
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
  ON_CALL(mockObjectType, getObjectType()).WillByDefault(Return(&mockObjectType));
  ON_CALL(mockObjectType, findMethod("foo")).WillByDefault(Return(&mockMethodDescriptor));
  ON_CALL(mockMethodDescriptor, getObjectType()).WillByDefault(Return(&mockObjectType));

  IdentifierChain identifierChain(mockExpression, "foo");
  const IType* type = identifierChain.getType(mContext);
  
  EXPECT_EQ(&mockMethodDescriptor, type);
}

TEST_F(IdentifierChainTest, getTypeForPrimitiveBaseTypeDeathTest) {
  NiceMock<MockExpression>* mockExpression = new NiceMock<MockExpression>();
  NiceMock<MockObjectType> mockObjectType;
  ON_CALL(*mockExpression, getType(_)).WillByDefault(Return(&mockObjectType));
  ON_CALL(*mockExpression, printToStream(_, _)).WillByDefault(Invoke(printObjectTypeExpression));
  ON_CALL(mockObjectType, isPrimitive()).WillByDefault(Return(true));
  
  IdentifierChain identifierChain(mockExpression, "foo");
  
  Mock::AllowLeak(mockExpression);
  Mock::AllowLeak(&mockObjectType);
  EXPECT_EXIT(identifierChain.getType(mContext),
              ::testing::ExitedWithCode(1),
              "Attempt to call a method 'foo' on an identifier that is not an object");
}

TEST_F(IdentifierChainTest, getTypeForObjectBaseTypeMethodNotFoundDeathTest) {
  NiceMock<MockExpression>* mockExpression = new NiceMock<MockExpression>();
  NiceMock<MockObjectType> mockObjectType;
  string modelFullName = "systems.vos.wisey.compiler.tests.MObject";
  ON_CALL(*mockExpression, getType(_)).WillByDefault(Return(&mockObjectType));
  ON_CALL(*mockExpression, printToStream(_, _)).WillByDefault(Invoke(printObjectTypeExpression));
  ON_CALL(mockObjectType, isModel()).WillByDefault(Return(true));
  ON_CALL(mockObjectType, isReference()).WillByDefault(Return(true));
  ON_CALL(mockObjectType, getObjectType()).WillByDefault(Return(&mockObjectType));
  ON_CALL(mockObjectType, getTypeName()).WillByDefault(Return(modelFullName));

  IdentifierChain identifierChain(mockExpression, "foo");

  Mock::AllowLeak(mockExpression);
  Mock::AllowLeak(&mockObjectType);
  EXPECT_EXIT(identifierChain.getType(mContext),
              ::testing::ExitedWithCode(1),
              "Method 'foo' is not found in object systems.vos.wisey.compiler.tests.MObject");
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
  ON_CALL(mockObjectType, getObjectType()).WillByDefault(Return(&mockObjectType));
  ON_CALL(mockObjectType, findMethod("foo")).WillByDefault(Return(&mockMethodDescriptor));
  ON_CALL(mockObjectType, getTypeName()).WillByDefault(Return(modelFullName));
  ON_CALL(mockMethodDescriptor, getObjectType()).WillByDefault(Return(&mockObjectType));
  ON_CALL(mockMethodDescriptor, getAccessLevel()).WillByDefault(Return(AccessLevel::PUBLIC_ACCESS));
  ON_CALL(*mockExpression, generateIR(_, _)).WillByDefault(Return(objectPointer));

  IdentifierChain identifierChain(mockExpression, "foo");
  Value* result = identifierChain.generateIR(mContext, PrimitiveTypes::VOID_TYPE);
  
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
  ON_CALL(mockObjectType, getObjectType()).WillByDefault(Return(&mockObjectType));
  ON_CALL(mockObjectType, findMethod("foo")).WillByDefault(Return(&mockMethodDescriptor));
  ON_CALL(mockObjectType, getTypeName()).WillByDefault(Return(modelFullName));
  ON_CALL(mockMethodDescriptor, getObjectType()).WillByDefault(Return(&mockObjectType));
  
  Mock::AllowLeak(mockExpression);
  Mock::AllowLeak(&mockObjectType);
  Mock::AllowLeak(&mockMethodDescriptor);
  IdentifierChain identifierChain(mockExpression, "foo");
  EXPECT_EXIT(identifierChain.generateIR(mContext, PrimitiveTypes::VOID_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Method 'foo' of object systems.vos.wisey.compiler.tests.MObject is private");
}
