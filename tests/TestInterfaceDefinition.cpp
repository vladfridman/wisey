//
//  TestInterfaceDefinition.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link InterfaceDefinition}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>

#include "TestFileSampleRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/AccessLevel.hpp"
#include "wisey/InterfaceDefinition.hpp"
#include "wisey/InterfaceTypeSpecifierFull.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/MethodSignatureDeclaration.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/PrimitiveTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Test;

struct InterfaceDefinitionTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  InterfaceDefinition* mInterfaceDefinition;
  InterfaceTypeSpecifierFull* mInterfaceTypeSpecifier;

  InterfaceDefinitionTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);

    PrimitiveTypeSpecifier* intTypeSpecifier = new PrimitiveTypeSpecifier(PrimitiveTypes::INT_TYPE);
    PrimitiveTypeSpecifier* floatTypeSpecifier =
      new PrimitiveTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
    Identifier* intArgumentIdentifier = new Identifier("intargument");
    VariableDeclaration* intArgument =
    VariableDeclaration::create(intTypeSpecifier, intArgumentIdentifier, 0);
    VariableList methodArguments;
    methodArguments.push_back(intArgument);
    vector<IModelTypeSpecifier*> thrownExceptions;
    IObjectElementDeclaration* methodSignatureDeclaration =
      new MethodSignatureDeclaration(floatTypeSpecifier, "foo", methodArguments, thrownExceptions);
    vector<IObjectElementDeclaration *> objectElements;
    objectElements.push_back(methodSignatureDeclaration);
    vector<IInterfaceTypeSpecifier*> parentInterfaces;
    
    mInterfaceTypeSpecifier = new InterfaceTypeSpecifierFull("systems.vos.wisey.compiler.tests",
                                                             "IMyInterface");
    vector<IObjectDefinition*> innerObjectDefinitions;
    mInterfaceDefinition = new InterfaceDefinition(mInterfaceTypeSpecifier,
                                                   parentInterfaces,
                                                   objectElements,
                                                   innerObjectDefinitions);
  }
  
  ~InterfaceDefinitionTest() {
    delete mInterfaceDefinition;
  }
};

TEST_F(InterfaceDefinitionTest, prototypeObjectTest) {
  mInterfaceDefinition->prototypeObject(mContext);
  
  ASSERT_NE(mContext.getInterface("systems.vos.wisey.compiler.tests.IMyInterface"), nullptr);
  
  Interface* interface = mContext.getInterface("systems.vos.wisey.compiler.tests.IMyInterface");
  
  EXPECT_STREQ(interface->getName().c_str(), "systems.vos.wisey.compiler.tests.IMyInterface");
  EXPECT_STREQ(interface->getShortName().c_str(), "IMyInterface");
  EXPECT_EQ(interface->findMethod("foo"), nullptr);
}

TEST_F(InterfaceDefinitionTest, prototypeMethodsTest) {
  mInterfaceDefinition->prototypeObject(mContext);
  mInterfaceDefinition->prototypeMethods(mContext);
  
  Interface* interface = mContext.getInterface("systems.vos.wisey.compiler.tests.IMyInterface");
  
  EXPECT_NE(interface->findMethod("foo"), nullptr);
}

TEST_F(InterfaceDefinitionTest, generateIRTest) {
  mInterfaceDefinition->prototypeObject(mContext);
  mInterfaceDefinition->prototypeMethods(mContext);
  mInterfaceDefinition->generateIR(mContext);

  ASSERT_NE(mContext.getInterface("systems.vos.wisey.compiler.tests.IMyInterface"), nullptr);

  Interface* interface = mContext.getInterface("systems.vos.wisey.compiler.tests.IMyInterface");
  Type* type = interface->getLLVMType(mLLVMContext);
  
  ASSERT_NE(type, nullptr);
  EXPECT_TRUE(type->isPointerTy());
  StructType* structType = (StructType*) type->getPointerElementType();
  EXPECT_TRUE(structType->getNumElements() == 1);
  Type* arrayOfFunctionsType = structType->getElementType(0);
  ASSERT_TRUE(arrayOfFunctionsType->isPointerTy());
  Type* pointerToFunctionType = arrayOfFunctionsType->getPointerElementType();
  ASSERT_TRUE(pointerToFunctionType->isPointerTy());
  Type* maybeFunctionType = pointerToFunctionType->getPointerElementType();
  ASSERT_TRUE(maybeFunctionType->isFunctionTy());
  FunctionType* functionType = (FunctionType *) maybeFunctionType;
  EXPECT_EQ(functionType->getReturnType(), Type::getInt32Ty(mLLVMContext));
  EXPECT_TRUE(functionType->isVarArg());
  
  EXPECT_STREQ(interface->getName().c_str(), "systems.vos.wisey.compiler.tests.IMyInterface");
  EXPECT_STREQ(interface->getShortName().c_str(), "IMyInterface");
  EXPECT_NE(interface->findMethod("foo"), nullptr);
}

TEST_F(TestFileSampleRunner, interfaceDefinitionRunTest) {
  runFile("tests/samples/test_interface_definition.yz", "0");
}
