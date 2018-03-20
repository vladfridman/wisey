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
    vector<IObjectElementDefinition*> interfaceElements;
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
}

TEST_F(MethodSignatureTest, elementTypeTest) {
  EXPECT_FALSE(mMethodSignature->isConstant());
  EXPECT_FALSE(mMethodSignature->isField());
  EXPECT_FALSE(mMethodSignature->isMethod());
  EXPECT_FALSE(mMethodSignature->isStaticMethod());
  EXPECT_TRUE(mMethodSignature->isMethodSignature());
  EXPECT_FALSE(mMethodSignature->isLLVMFunction());
}

TEST_F(MethodSignatureTest, createCopyTest) {
  vector<IInterfaceTypeSpecifier*> parentInterfaces;
  vector<IObjectElementDefinition*> interfaceElements;
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
  Interface* threadInterface = mContext.getInterface(Names::getThreadInterfaceFullName());
  argumentTypes.push_back(threadInterface->getLLVMType(mContext));
  Controller* callStack = mContext.getController(Names::getCallStackControllerFullName());
  argumentTypes.push_back(callStack->getLLVMType(mContext));
  Type* llvmReturnType = PrimitiveTypes::LONG_TYPE->getLLVMType(mContext);
  FunctionType* expectedType = FunctionType::get(llvmReturnType, argumentTypes, false);
  
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
