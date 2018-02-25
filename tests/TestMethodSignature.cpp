//
//  TestMethodSignature.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/15/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link MethodSignature}
//

#include <gtest/gtest.h>

#include "TestPrefix.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/MethodSignature.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct MethodSignatureTest : Test {
  IRGenerationContext mContext;
  MethodSignature* mMethodSignature;
  Interface* mInterface;
  
  MethodSignatureTest() {
    TestPrefix::generateIR(mContext);
    
    vector<IInterfaceTypeSpecifier*> parentInterfaces;
    vector<IObjectElementDeclaration*> interfaceElements;
    vector<Type*> types;
    string interfaceFullName = "systems.vos.wisey.compiler.tests.IInterface";
    StructType* structType = StructType::create(mContext.getLLVMContext(), "IInterface");
    structType->setBody(types);
    mInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                         "systems.vos.wisey.compiler.tests.IInterface",
                                         structType,
                                         parentInterfaces,
                                         interfaceElements);

    vector<MethodArgument*> arguments;
    vector<const Model*> thrownExceptions;
    mMethodSignature = new MethodSignature(mInterface,
                                           "foo",
                                           PrimitiveTypes::LONG_TYPE,
                                           arguments,
                                           thrownExceptions);
}
};

TEST_F(MethodSignatureTest, methodSignatureTest) {
  EXPECT_STREQ("foo", mMethodSignature->getName().c_str());
  EXPECT_EQ(PrimitiveTypes::LONG_TYPE, mMethodSignature->getReturnType());
  EXPECT_EQ(0u, mMethodSignature->getArguments().size());
  EXPECT_STREQ("systems.vos.wisey.compiler.tests.IInterface.foo",
               mMethodSignature->getTypeName().c_str());
  EXPECT_EQ(FUNCTION_TYPE, mMethodSignature->getTypeKind());
}

TEST_F(MethodSignatureTest, createCopyTest) {
  vector<IInterfaceTypeSpecifier*> parentInterfaces;
  vector<IObjectElementDeclaration*> interfaceElements;
  Interface* interface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                                 "systems.vos.wisey.compiler.tests.IAnother",
                                                 NULL,
                                                 parentInterfaces,
                                                 interfaceElements);
  MethodSignature* copy = mMethodSignature->createCopy(interface);
  
  EXPECT_STREQ(copy->getName().c_str(), "foo");
  EXPECT_EQ(copy->getReturnType(), PrimitiveTypes::LONG_TYPE);
  EXPECT_EQ(copy->getArguments().size(), 0u);
}

TEST_F(MethodSignatureTest, getLLVMTypeTest) {
  vector<Type*> argumentTypes;
  argumentTypes.push_back(mInterface->getLLVMType(mContext));
  Controller* threadController = mContext.getController(Names::getThreadControllerFullName());
  argumentTypes.push_back(threadController->getLLVMType(mContext));
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  Type* llvmReturnType = PrimitiveTypes::LONG_TYPE->getLLVMType(mContext);
  FunctionType* expectedType = FunctionType::get(llvmReturnType, argTypesArray, false);
  
  FunctionType* actualType = mMethodSignature->getLLVMType(mContext);
  
  EXPECT_EQ(expectedType, actualType);
}

TEST_F(MethodSignatureTest, isOwnerTest) {
  EXPECT_FALSE(mMethodSignature->isOwner());
}

TEST_F(MethodSignatureTest, isReferenceTest) {
  EXPECT_FALSE(mMethodSignature->isReference());
}

TEST_F(MethodSignatureTest, isPrimitiveTest) {
  EXPECT_FALSE(mMethodSignature->isPrimitive());
}

TEST_F(MethodSignatureTest, isArrayTest) {
  EXPECT_FALSE(mMethodSignature->isArray());
}

TEST_F(MethodSignatureTest, isFunctionTest) {
  EXPECT_TRUE(mMethodSignature->isFunction());
}

TEST_F(MethodSignatureTest, isPackageTest) {
  EXPECT_FALSE(mMethodSignature->isPackage());
}

TEST_F(MethodSignatureTest, printToStreamTest) {
  stringstream stringStream;
  mMethodSignature->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("  long foo();\n", stringStream.str().c_str());
}
