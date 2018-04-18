//
//  TestMethodSignatureDeclaration.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/1/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link MethodSignatureDeclaration}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>

#include "TestPrefix.hpp"
#include "wisey/AccessLevel.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodSignature.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/VariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct MethodSignatureDeclarationTest : Test {
  IRGenerationContext mContext;
  const PrimitiveTypeSpecifier* mFloatTypeSpecifier;
  const PrimitiveTypeSpecifier* mIntTypeSpecifier;
  Identifier* mIntArgumentIdentifier;
  Identifier* mFloatArgumentIdentifier;
  VariableDeclaration* mIntArgument;
  VariableDeclaration* mFloatArgument;
  VariableList mArguments;
  Interface* mInterface;
  
  MethodSignatureDeclarationTest() :
  mFloatTypeSpecifier(PrimitiveTypes::FLOAT_TYPE->newTypeSpecifier()),
  mIntTypeSpecifier(PrimitiveTypes::INT_TYPE->newTypeSpecifier()),
  mIntArgumentIdentifier(new Identifier("intargument", 0)),
  mFloatArgumentIdentifier(new Identifier("floatargument", 0)),
  mIntArgument(VariableDeclaration::create(mIntTypeSpecifier, mIntArgumentIdentifier, 0)),
  mFloatArgument(VariableDeclaration::create(mFloatTypeSpecifier, mFloatArgumentIdentifier, 0)) {
    TestPrefix::generateIR(mContext);

    LLVMContext& llvmContext = mContext.getLLVMContext();
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    string interfaceFullName = "systems.vos.wisey.compiler.tests.IInterface";
    StructType* structType = StructType::create(llvmContext, interfaceFullName);
    structType->setBody(types);
    vector<IInterfaceTypeSpecifier*> parentInterfaces;
    vector<IObjectElementDefinition*> interfaceElements;
    mInterface = Interface::newPublicInterface(interfaceFullName,
                                               structType,
                                               parentInterfaces,
                                               interfaceElements,
                                               0);
  }
  
  ~MethodSignatureDeclarationTest() {
  }
};

TEST_F(MethodSignatureDeclarationTest, methodDescriptorExtractTest) {
  mArguments.push_back(mIntArgument);
  mArguments.push_back(mFloatArgument);
  vector<IModelTypeSpecifier*> thrownExceptions;
  const PrimitiveTypeSpecifier* floatTypeSpecifier = PrimitiveTypes::FLOAT_TYPE->newTypeSpecifier();
  MethodSignatureDeclaration methodSignatureDeclaration(floatTypeSpecifier,
                                                        "foo",
                                                        mArguments,
                                                        thrownExceptions,
                                                        new MethodQualifiers(0),
                                                        0);
  MethodSignature* methodSignature = methodSignatureDeclaration.define(mContext, mInterface);
  vector<MethodArgument*> arguments = methodSignature->getArguments();
  
  EXPECT_FALSE(methodSignatureDeclaration.isConstant());
  EXPECT_FALSE(methodSignatureDeclaration.isField());
  EXPECT_FALSE(methodSignatureDeclaration.isMethod());
  EXPECT_FALSE(methodSignatureDeclaration.isStaticMethod());
  EXPECT_TRUE(methodSignatureDeclaration.isMethodSignature());
  EXPECT_FALSE(methodSignatureDeclaration.isLLVMFunction());

  EXPECT_STREQ(methodSignature->getName().c_str(), "foo");
  EXPECT_EQ(methodSignature->getReturnType(), PrimitiveTypes::FLOAT_TYPE);
  EXPECT_EQ(arguments.size(), 2ul);
  EXPECT_EQ(arguments.at(0)->getName(), "intargument");
  EXPECT_EQ(arguments.at(0)->getType(), PrimitiveTypes::INT_TYPE);
  EXPECT_EQ(arguments.at(1)->getName(), "floatargument");
  EXPECT_EQ(arguments.at(1)->getType(), PrimitiveTypes::FLOAT_TYPE);
}
