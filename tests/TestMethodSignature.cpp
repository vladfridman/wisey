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
#include "IRGenerationContext.hpp"
#include "MethodSignature.hpp"
#include "Names.hpp"
#include "PrimitiveTypes.hpp"

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
                                         interfaceElements,
                                         mContext.getImportProfile(),
                                         0);

    vector<const wisey::Argument*> arguments;
    vector<const Model*> thrownExceptions;
    mMethodSignature = new MethodSignature(mInterface,
                                           "foo",
                                           PrimitiveTypes::LONG,
                                           arguments,
                                           thrownExceptions,
                                           new MethodQualifiers(0),
                                           interfaceFullName,
                                           0);
  }
};

TEST_F(MethodSignatureTest, methodSignatureTest) {
  EXPECT_STREQ("foo", mMethodSignature->getName().c_str());
  EXPECT_EQ(PrimitiveTypes::LONG, mMethodSignature->getReturnType());
  EXPECT_EQ(0u, mMethodSignature->getArguments().size());
  EXPECT_STREQ("systems.vos.wisey.compiler.tests.IInterface.foo",
               mMethodSignature->getTypeName().c_str());
  EXPECT_STREQ("systems.vos.wisey.compiler.tests.IInterface",
               mMethodSignature->getOriginalParentName().c_str());
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
                                                 interfaceElements,
                                                 mContext.getImportProfile(),
                                                 0);
  MethodSignature* copy = mMethodSignature->createCopy(interface);
  
  EXPECT_STREQ(copy->getName().c_str(), "foo");
  EXPECT_EQ(copy->getReturnType(), PrimitiveTypes::LONG);
  EXPECT_EQ(copy->getArguments().size(), 0u);
}

TEST_F(MethodSignatureTest, getLLVMTypeTest) {
  vector<Type*> argumentTypes;
  argumentTypes.push_back(mInterface->getLLVMType(mContext));
  Interface* threadInterface = mContext.getInterface(Names::getThreadInterfaceFullName(), 0);
  argumentTypes.push_back(threadInterface->getLLVMType(mContext));
  Controller* callStack = mContext.getController(Names::getCallStackControllerFullName(), 0);
  argumentTypes.push_back(callStack->getLLVMType(mContext));
  Type* llvmReturnType = PrimitiveTypes::LONG->getLLVMType(mContext);
  FunctionType* expectedType = FunctionType::get(llvmReturnType, argumentTypes, false);
  
  FunctionType* actualType = mMethodSignature->getLLVMType(mContext);
  
  EXPECT_EQ(expectedType, actualType);
}

TEST_F(MethodSignatureTest, isTypeKindTest) {
  EXPECT_FALSE(mMethodSignature->isOwner());
  EXPECT_FALSE(mMethodSignature->isReference());
  EXPECT_FALSE(mMethodSignature->isPrimitive());
  EXPECT_FALSE(mMethodSignature->isArray());
  EXPECT_TRUE(mMethodSignature->isFunction());
  EXPECT_FALSE(mMethodSignature->isPackage());
  EXPECT_FALSE(mMethodSignature->isNative());
  EXPECT_FALSE(mMethodSignature->isPointer());
  EXPECT_FALSE(mMethodSignature->isImmutable());
}

TEST_F(MethodSignatureTest, printToStreamTest) {
  stringstream stringStream;
  mMethodSignature->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("  long foo();\n", stringStream.str().c_str());
}
