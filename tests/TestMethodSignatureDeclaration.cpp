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

#include "wisey/AccessLevel.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodSignature.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/ProgramPrefix.hpp"
#include "wisey/VariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct MethodSignatureDeclarationTest : Test {
  IRGenerationContext mContext;
  PrimitiveTypeSpecifier* mFloatTypeSpecifier;
  PrimitiveTypeSpecifier* mIntTypeSpecifier;
  Identifier* mIntArgumentIdentifier;
  Identifier* mFloatArgumentIdentifier;
  VariableDeclaration* mIntArgument;
  VariableDeclaration* mFloatArgument;
  VariableList mArguments;
  Interface* mInterface;
  
  MethodSignatureDeclarationTest() :
  mFloatTypeSpecifier(new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE)),
  mIntTypeSpecifier(new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE)),
  mIntArgumentIdentifier(new Identifier("intargument")),
  mFloatArgumentIdentifier(new Identifier("floatargument")),
  mIntArgument(new VariableDeclaration(mIntTypeSpecifier, mIntArgumentIdentifier)),
  mFloatArgument(new VariableDeclaration(mFloatTypeSpecifier, mFloatArgumentIdentifier)) {
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);

    LLVMContext& llvmContext = mContext.getLLVMContext();
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    string interfaceFullName = "systems.vos.wisey.compiler.tests.IInterface";
    StructType* structType = StructType::create(llvmContext, interfaceFullName);
    structType->setBody(types);
    vector<InterfaceTypeSpecifier*> parentInterfaces;
    vector<MethodSignatureDeclaration*> interfaceMethods;
    mInterface = new Interface(interfaceFullName, structType, parentInterfaces, interfaceMethods);
  }
  
  ~MethodSignatureDeclarationTest() {
  }
};

TEST_F(MethodSignatureDeclarationTest, methodDescriptorExtractTest) {
  mArguments.push_back(mIntArgument);
  mArguments.push_back(mFloatArgument);
  vector<ModelTypeSpecifier*> thrownExceptions;
  PrimitiveTypeSpecifier* floatTypeSpecifier =
    new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
  MethodSignatureDeclaration methodSignatureDeclaration(floatTypeSpecifier,
                                                        "foo",
                                                        mArguments,
                                                        thrownExceptions);
  MethodSignature* methodSignature = methodSignatureDeclaration.createMethodSignature(mContext);
  vector<MethodArgument*> arguments = methodSignature->getArguments();
  
  EXPECT_STREQ(methodSignature->getName().c_str(), "foo");
  EXPECT_EQ(methodSignature->getReturnType(), PrimitiveTypes::FLOAT_TYPE);
  EXPECT_EQ(arguments.size(), 2ul);
  EXPECT_EQ(arguments.at(0)->getName(), "intargument");
  EXPECT_EQ(arguments.at(0)->getType(), PrimitiveTypes::INT_TYPE);
  EXPECT_EQ(arguments.at(1)->getName(), "floatargument");
  EXPECT_EQ(arguments.at(1)->getType(), PrimitiveTypes::FLOAT_TYPE);
}
