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
#include "wisey/VariableDeclaration.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct MethodSignatureDeclarationTest : Test {
  IRGenerationContext mContext;
  PrimitiveTypeSpecifier mFloatTypeSpecifier;
  PrimitiveTypeSpecifier mIntTypeSpecifier;
  Identifier mIntArgumentIdentifier;
  Identifier mFloatArgumentIdentifier;
  VariableDeclaration mIntArgument;
  VariableDeclaration mFloatArgument;
  VariableList mArguments;
  Interface* mInterface;
  
  MethodSignatureDeclarationTest() :
  mFloatTypeSpecifier(PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE)),
  mIntTypeSpecifier(PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE)),
  mIntArgumentIdentifier(Identifier("intargument")),
  mFloatArgumentIdentifier(Identifier("floatargument")),
  mIntArgument(VariableDeclaration(mIntTypeSpecifier, mIntArgumentIdentifier)),
  mFloatArgument(VariableDeclaration(mFloatTypeSpecifier, mFloatArgumentIdentifier)) {
    LLVMContext& llvmContext = mContext.getLLVMContext();
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    StructType* structType = StructType::create(llvmContext, "Interface");
    structType->setBody(types);
    vector<MethodSignature*> methodSignatures;
    vector<Interface*> parentInterfaces;
    mInterface = new Interface("Interface", structType, parentInterfaces, methodSignatures);
  }
  
  ~MethodSignatureDeclarationTest() {
  }
};

TEST_F(MethodSignatureDeclarationTest, methodDescriptorExtractTest) {
  mArguments.push_back(&mIntArgument);
  mArguments.push_back(&mFloatArgument);
  vector<ITypeSpecifier*> thrownExceptions;
  MethodSignatureDeclaration methodSignatureDeclaration(mFloatTypeSpecifier,
                                                        "foo",
                                                        mArguments,
                                                        thrownExceptions);
  MethodSignature* methodSignature = methodSignatureDeclaration.createMethodSignature(mContext, 0);
  vector<MethodArgument*> arguments = methodSignature->getArguments();
  
  EXPECT_STREQ(methodSignature->getName().c_str(), "foo");
  EXPECT_EQ(methodSignature->getReturnType(), PrimitiveTypes::FLOAT_TYPE);
  EXPECT_EQ(arguments.size(), 2ul);
  EXPECT_EQ(arguments.at(0)->getName(), "intargument");
  EXPECT_EQ(arguments.at(0)->getType(), PrimitiveTypes::INT_TYPE);
  EXPECT_EQ(arguments.at(1)->getName(), "floatargument");
  EXPECT_EQ(arguments.at(1)->getType(), PrimitiveTypes::FLOAT_TYPE);
}
