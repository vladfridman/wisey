//
//  TestExternalControllerDefinition.cpp
//  runtests
//
//  Created by Vladimir Fridman on 9/27/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ExternalControllerDefinition}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "wisey/AccessLevel.hpp"
#include "wisey/ExternalControllerDefinition.hpp"
#include "wisey/FloatConstant.hpp"
#include "wisey/Interface.hpp"
#include "wisey/InterfaceTypeSpecifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"
#include "wisey/ProgramPrefix.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct ExternalControllerDefinitionTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  vector<FieldDeclaration*> mReceivedFields;
  vector<FieldDeclaration*> mInjectedFields;
  vector<FieldDeclaration*> mStateFields;
  vector<MethodSignatureDeclaration*> mMethodSignatures;
  vector<InterfaceTypeSpecifier*> mInterfaces;
  
  ExternalControllerDefinitionTest() : mLLVMContext(mContext.getLLVMContext()) {
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    
    MethodSignatureDeclaration* methodSignature;
    
    mContext.setPackage("systems.vos.wisey.compiler.tests");
    PrimitiveTypeSpecifier* intTypeSpecifier =
    new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
    PrimitiveTypeSpecifier* floatTypeSpecifier =
    new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
    Identifier* intArgumentIdentifier = new Identifier("intargument");
    VariableDeclaration* intArgument =
    new VariableDeclaration(intTypeSpecifier, intArgumentIdentifier);
    VariableList methodArguments;
    methodArguments.push_back(intArgument);
    vector<ModelTypeSpecifier*> thrownExceptions;
    methodSignature = new MethodSignatureDeclaration(floatTypeSpecifier,
                                                     "foo",
                                                     methodArguments,
                                                     thrownExceptions);
    mMethodSignatures.push_back(methodSignature);
    
    PrimitiveTypeSpecifier* longType = new PrimitiveTypeSpecifier(PrimitiveTypes::LONG_TYPE);
    PrimitiveTypeSpecifier* floatType = new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
    ExpressionList arguments;
    FieldDeclaration* field1 =
    new FieldDeclaration(RECEIVED_FIELD, longType, "mField1", arguments);
    FieldDeclaration* field2 =
    new FieldDeclaration(RECEIVED_FIELD, floatType, "mField2", arguments);
    mReceivedFields.push_back(field1);
    mReceivedFields.push_back(field2);
  }
  
  ~ExternalControllerDefinitionTest() {
  }
};

// TODO: add tests once ExternalControllerDefinition is implemented

TEST_F(TestFileSampleRunner, externalControllerDefinitionsRunTest) {
  compileFile("tests/samples/test_external_controller_definitions.yz");
}

