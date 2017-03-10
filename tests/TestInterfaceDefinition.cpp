//
//  TestInterfaceDefinition.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/28/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link InterfaceDefinition}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/AccessSpecifiers.hpp"
#include "yazyk/InterfaceDefinition.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/MethodSignatureDeclaration.hpp"
#include "yazyk/PrimitiveTypes.hpp"
#include "yazyk/PrimitiveTypeSpecifier.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;

TEST(InterfaceDefinitionTest, TestSimpleDefinition) {
  IRGenerationContext context;
  LLVMContext& llvmContext = context.getLLVMContext();
  
  PrimitiveTypeSpecifier intTypeSpecifier(PrimitiveTypes::INT_TYPE);
  PrimitiveTypeSpecifier floatTypeSpecifier(PrimitiveTypes::FLOAT_TYPE);
  Identifier intArgumentIdentifier("intargument");
  VariableDeclaration intArgument(intTypeSpecifier, intArgumentIdentifier);
  VariableList methodArguments;
  methodArguments.push_back(&intArgument);
  MethodSignatureDeclaration methodSignatureDeclaration(floatTypeSpecifier, "foo", methodArguments);
  vector<MethodSignatureDeclaration *> methods;
  methods.push_back(&methodSignatureDeclaration);
  
  InterfaceDefinition interfaceDefinition("myinterface", methods);
  
  interfaceDefinition.generateIR(context);
  Interface* interface = context.getInterface("myinterface");
  Type* pointerType = interface->getLLVMType(llvmContext);
  
  ASSERT_NE(pointerType, nullptr);
  EXPECT_TRUE(pointerType->isPointerTy());
  StructType* structType = (StructType*) pointerType->getPointerElementType();
  EXPECT_TRUE(structType->getNumElements() == 1);
  Type* arrayOfFunctionsType = structType->getElementType(0);
  ASSERT_TRUE(arrayOfFunctionsType->isPointerTy());
  Type* pointerToFunctionType = arrayOfFunctionsType->getPointerElementType();
  ASSERT_TRUE(pointerToFunctionType->isPointerTy());
  Type* maybeFunctionType = pointerToFunctionType->getPointerElementType();
  ASSERT_TRUE(maybeFunctionType->isFunctionTy());
  FunctionType* functionType = (FunctionType *) maybeFunctionType;
  EXPECT_EQ(functionType->getReturnType(), Type::getInt32Ty(llvmContext));
  EXPECT_TRUE(functionType->isVarArg());
  
  EXPECT_STREQ(interface->getName().c_str(), "myinterface");
}

TEST_F(TestFileSampleRunner, InterfaceDefinitionTest) {
  runFile("tests/samples/test_interface_definition.yz", "0");
}
