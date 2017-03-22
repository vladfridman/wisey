//
//  TestInterface.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/1/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Interface}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/Interface.hpp"
#include "yazyk/MethodSignature.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

struct InterfaceTest : public Test {
  Interface* mInterface;
  MethodSignature* mMethodSignature;
  StructType* mShapeStructType;
  ModelField* mWidthField;
  ModelField* mHeightField;
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  
  InterfaceTest() : mLLVMContext(mContext.getLLVMContext()) {
    vector<Type*> objectTypes;
    StructType* objectStructType = StructType::create(mLLVMContext, "IObject");
    objectStructType->setBody(objectTypes);
    vector<MethodSignature*> objectMethodSignatures;
    vector<Interface*> objectParentInterfaces;
    Interface* shapeInterface = new Interface("IShape",
                                              mShapeStructType,
                                              objectParentInterfaces,
                                              objectMethodSignatures);

    vector<Type*> shapeTypes;
    mShapeStructType = StructType::create(mLLVMContext, "IShape");
    mShapeStructType->setBody(shapeTypes);
    vector<MethodArgument*> shapeMethodArguments;
    mMethodSignature = new MethodSignature("foo",
                                           PrimitiveTypes::INT_TYPE,
                                           shapeMethodArguments,
                                           0);
    vector<MethodSignature*> shapeMethodSignatures;
    shapeMethodSignatures.push_back(mMethodSignature);
    vector<Interface*> shapeParentInterfaces;
    shapeParentInterfaces.push_back(shapeInterface);
    mInterface = new Interface("IShape",
                               mShapeStructType,
                               shapeParentInterfaces,
                               shapeMethodSignatures);
  }
  
  ~InterfaceTest() { }
};

TEST_F(InterfaceTest, TestInterfaceInstantiation) {
  EXPECT_STREQ(mInterface->getName().c_str(), "IShape");
  EXPECT_EQ(mInterface->getTypeKind(), INTERFACE_TYPE);
  EXPECT_EQ(mInterface->getLLVMType(mLLVMContext), mShapeStructType->getPointerTo());
  EXPECT_EQ(mInterface->getParentInterfaces().size(), 1u);
}

TEST_F(InterfaceTest, TestFindMethod) {
  EXPECT_EQ(mInterface->findMethod("foo"), mMethodSignature);
  EXPECT_EQ(mInterface->findMethod("bar"), nullptr);
}

TEST_F(InterfaceTest, getObjectNameGlobalVariableNameTest) {
  EXPECT_STREQ(mInterface->getObjectNameGlobalVariableName().c_str(), "interface.IShape.name");
}

TEST_F(TestFileSampleRunner, InterfaceMethodNotImplmentedDeathTest) {
  expectFailIRGeneration("tests/samples/test_interface_method_not_implmented.yz",
                         1,
                         "Error: Method 'getArea' of interface 'IShape' is not "
                         "implemented by model 'MSquare'");
}

TEST_F(TestFileSampleRunner, InterfaceMethodDifferentReturnTypeDeathTest) {
  expectFailIRGeneration("tests/samples/test_interface_method_return_type_doesnot_match.yz",
                         1,
                         "Error: Method 'getArea' of interface 'IShape' has different "
                         "return type when implmeneted by model 'MSquare'");
}

TEST_F(TestFileSampleRunner, InterfaceMethodDifferentArgumentTypesDeathTest) {
  expectFailIRGeneration("tests/samples/test_interface_method_arguments_dont_match.yz",
                         1,
                         "Error: Method 'getArea' of interface 'IShape' has different "
                         "argument types when implmeneted by model 'MSquare'");
}

TEST_F(TestFileSampleRunner, ModelImplmenetingInterfaceDefinitionTest) {
  runFile("tests/samples/test_interface_implementation.yz", "90");
}

TEST_F(TestFileSampleRunner, InterfaceInheritanceTest) {
  runFile("tests/samples/test_level2_inheritance.yz", "235");
}

TEST_F(TestFileSampleRunner, InterfaceMultipleInheritanceTest) {
  runFile("tests/samples/test_interface_multiple_inheritance.yz", "5");
}

