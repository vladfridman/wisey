//
//  TestIMethodDescriptor.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IMethodDescriptor}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "yazyk/Method.hpp"
#include "yazyk/MethodArgument.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

struct IMethodDescriptorTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Method* mMethod;
  Model* mModel;
  
public:
  
  IMethodDescriptorTest() : mLLVMContext(mContext.getLLVMContext()) {
    
    MethodArgument* doubleArgument = new MethodArgument(PrimitiveTypes::DOUBLE_TYPE, "argDouble");
    MethodArgument* charArgument = new MethodArgument(PrimitiveTypes::CHAR_TYPE, "argChar");
    std::vector<MethodArgument*> arguments;
    arguments.push_back(doubleArgument);
    arguments.push_back(charArgument);
    mMethod = new Method("mymethod", PrimitiveTypes::BOOLEAN_TYPE, arguments, 0, NULL);

    vector<Type*> types;
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    StructType* structType = StructType::create(mLLVMContext, "Object");
    structType->setBody(types);
    map<string, Field*> fields;
    fields["foo"] = new Field(PrimitiveTypes::INT_TYPE, 0);
    fields["bar"] = new Field(PrimitiveTypes::INT_TYPE, 1);
    vector<Method*> methods;
    vector<Interface*> interfaces;
    mModel = new Model("Object", structType, fields, methods, interfaces);
}
};

TEST_F(IMethodDescriptorTest, compareTest) {
  MethodArgument* doubleArgument = new MethodArgument(PrimitiveTypes::DOUBLE_TYPE, "argDouble2");
  MethodArgument* charArgument = new MethodArgument(PrimitiveTypes::CHAR_TYPE, "argChar2");
  std::vector<MethodArgument*> arguments;
  arguments.push_back(doubleArgument);
  arguments.push_back(charArgument);
  Method method("mymethod", PrimitiveTypes::BOOLEAN_TYPE, arguments, 0, NULL);
  
  ASSERT_TRUE(IMethodDescriptor::compare(&method, mMethod));
}

TEST_F(IMethodDescriptorTest, nameNotEqualsTest) {
  MethodArgument* doubleArgument = new MethodArgument(PrimitiveTypes::DOUBLE_TYPE, "argDouble2");
  MethodArgument* charArgument = new MethodArgument(PrimitiveTypes::CHAR_TYPE, "argChar2");
  std::vector<MethodArgument*> arguments;
  arguments.push_back(doubleArgument);
  arguments.push_back(charArgument);
  Method method("differentname", PrimitiveTypes::BOOLEAN_TYPE, arguments, 0, NULL);
  
  ASSERT_FALSE(IMethodDescriptor::compare(&method, mMethod));
}

TEST_F(IMethodDescriptorTest, numberOfArgumentsNotEqualsTest) {
  MethodArgument* doubleArgument = new MethodArgument(PrimitiveTypes::DOUBLE_TYPE, "argDouble2");
  std::vector<MethodArgument*> arguments;
  arguments.push_back(doubleArgument);
  Method method("mymethod", PrimitiveTypes::BOOLEAN_TYPE, arguments, 0, NULL);
  
  ASSERT_FALSE(IMethodDescriptor::compare(&method, mMethod));
}

TEST_F(IMethodDescriptorTest, typeOfArgumentsNotEqualsTest) {
  MethodArgument* doubleArgument = new MethodArgument(PrimitiveTypes::DOUBLE_TYPE, "argDouble2");
  MethodArgument* charArgument = new MethodArgument(PrimitiveTypes::INT_TYPE, "argChar2");
  std::vector<MethodArgument*> arguments;
  arguments.push_back(doubleArgument);
  arguments.push_back(charArgument);
  Method method("mymethod", PrimitiveTypes::BOOLEAN_TYPE, arguments, 0, NULL);
  
  ASSERT_FALSE(IMethodDescriptor::compare(&method, mMethod));
}

TEST_F(IMethodDescriptorTest, getLLVMFunctionTypeTest) {
  MethodArgument* intArgument = new MethodArgument(PrimitiveTypes::INT_TYPE, "intargument");
  std::vector<MethodArgument*> arguments;
  arguments.push_back(intArgument);
  MethodSignature method("foo", PrimitiveTypes::FLOAT_TYPE, arguments, 0);
  FunctionType* functionType = IMethodDescriptor::getLLVMFunctionType(&method, mContext, mModel);
  
  EXPECT_EQ(functionType->getReturnType(), Type::getFloatTy(mLLVMContext));
  EXPECT_EQ(functionType->getNumParams(), 2u);
  EXPECT_EQ(functionType->getParamType(0), mModel->getLLVMType(mLLVMContext));
  EXPECT_EQ(functionType->getParamType(1), Type::getInt32Ty(mLLVMContext));
}

