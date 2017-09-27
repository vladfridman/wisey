//
//  TestExternalModelDefinition.cpp
//  runtests
//
//  Created by Vladimir Fridman on 9/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ExternalModelDefinition}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>

#include "TestFileSampleRunner.hpp"
#include "wisey/ExternalModelDefinition.hpp"
#include "wisey/FloatConstant.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/ProgramPrefix.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct ExternalModelDefinitionTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  MethodSignatureDeclaration *mMethodSignatureDeclaration;
  vector<FieldDeclaration*> mFields;
  vector<MethodSignatureDeclaration*> mMethodSignatures;
  
  ExternalModelDefinitionTest() : mLLVMContext(mContext.getLLVMContext()) {
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    
    mContext.setPackage("systems.vos.wisey.compiler.tests");
    PrimitiveTypeSpecifier* intTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
    PrimitiveTypeSpecifier* floatTypeSpecifier =
    new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
    Identifier* intArgumentIdentifier = new Identifier("intargument");
    VariableDeclaration* intArgument =
    new VariableDeclaration(intTypeSpecifier, intArgumentIdentifier);
    VariableList methodArguments;
    methodArguments.push_back(intArgument);
    vector<ModelTypeSpecifier*> thrownExceptions;
    mMethodSignatureDeclaration = new MethodSignatureDeclaration(floatTypeSpecifier,
                                                                 "foo",
                                                                 methodArguments,
                                                                 thrownExceptions);
    mMethodSignatures.push_back(mMethodSignatureDeclaration);
  }
};

// TODO: add tests once ExternalModelDefinition is implemented

TEST_F(TestFileSampleRunner, externalModelDefinitionsRunTest) {
  compileFile("tests/samples/test_external_model_definitions.yz");
}
