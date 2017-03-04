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
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

struct InterfaceTest : public Test {
  Interface* mInterface;
  Method* mMethod;
  StructType* mStructType;
  ModelField* mWidthField;
  ModelField* mHeightField;
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  
  InterfaceTest() : mLLVMContext(mContext.getLLVMContext()) {
    vector<Type*> types;
    mStructType = StructType::create(mLLVMContext, "Shape");
    mStructType->setBody(types);
    vector<MethodArgument*> methodArguments;
    mMethod = new Method("foo", PrimitiveTypes::INT_TYPE, methodArguments);
    map<string, Method*>* methods = new map<string, Method*>();
    (*methods)["foo"] = mMethod;
    mInterface = new Interface("Shape", mStructType, methods);
  }
  
  ~InterfaceTest() { }
};

TEST_F(InterfaceTest, TestInterfaceInstantiation) {
  EXPECT_STREQ(mInterface->getName().c_str(), "Shape");
  EXPECT_EQ(mInterface->getTypeKind(), INTERFACE_TYPE);
  EXPECT_EQ(mInterface->getLLVMType(mLLVMContext), mStructType);
}

TEST_F(InterfaceTest, TestFindMethod) {
  EXPECT_EQ(mInterface->findMethod("foo"), mMethod);
  EXPECT_EQ(mInterface->findMethod("bar"), nullptr);
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
  runFile("tests/samples/test_interface_implementation.yz", "25");
}

