//
//  TestIMethodDescriptor.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IMethodDescriptor}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestPrefix.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/Method.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct IMethodDescriptorTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Method* mMethod;
  Model* mModel;
  Thread* mMainThread;
  
public:
  
  IMethodDescriptorTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    MethodArgument* doubleArgument = new MethodArgument(PrimitiveTypes::DOUBLE_TYPE, "argDouble");
    MethodArgument* charArgument = new MethodArgument(PrimitiveTypes::CHAR_TYPE, "argChar");
    std::vector<MethodArgument*> arguments;
    arguments.push_back(doubleArgument);
    arguments.push_back(charArgument);
    vector<const Model*> thrownExceptions;

    vector<Type*> types;
    types.push_back(Type::getInt64Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MObject";
    StructType* structType = StructType::create(mLLVMContext, "MObject");
    structType->setBody(types);
    vector<IField*> fields;
    fields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "foo"));
    fields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "bar"));
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS, modelFullName, structType);
    mModel->setFields(fields, 1u);
    
    mMethod = new Method(mModel,
                         "mymethod",
                         AccessLevel::PUBLIC_ACCESS,
                         PrimitiveTypes::BOOLEAN_TYPE,
                         arguments,
                         thrownExceptions,
                         NULL,
                         0);

    mMainThread = mContext.getThread(Names::getMainThreadFullName());
  }
};

TEST_F(IMethodDescriptorTest, compareTest) {
  MethodArgument* doubleArgument = new MethodArgument(PrimitiveTypes::DOUBLE_TYPE, "argDouble2");
  MethodArgument* charArgument = new MethodArgument(PrimitiveTypes::CHAR_TYPE, "argChar2");
  vector<MethodArgument*> arguments;
  arguments.push_back(doubleArgument);
  arguments.push_back(charArgument);
  vector<const Model*> thrownExceptions;
  Method method(mModel,
                "mymethod",
                AccessLevel::PUBLIC_ACCESS,
                PrimitiveTypes::BOOLEAN_TYPE,
                arguments,
                thrownExceptions,
                NULL,
                0);
  
  ASSERT_TRUE(IMethodDescriptor::compare(&method, mMethod));
}

TEST_F(IMethodDescriptorTest, nameNotEqualsTest) {
  MethodArgument* doubleArgument = new MethodArgument(PrimitiveTypes::DOUBLE_TYPE, "argDouble2");
  MethodArgument* charArgument = new MethodArgument(PrimitiveTypes::CHAR_TYPE, "argChar2");
  vector<MethodArgument*> arguments;
  arguments.push_back(doubleArgument);
  arguments.push_back(charArgument);
  vector<const Model*> thrownExceptions;
  Method method(mModel,
                "differentname",
                AccessLevel::PUBLIC_ACCESS,
                PrimitiveTypes::BOOLEAN_TYPE,
                arguments,
                thrownExceptions,
                NULL,
                0);
  
  ASSERT_FALSE(IMethodDescriptor::compare(&method, mMethod));
}

TEST_F(IMethodDescriptorTest, numberOfArgumentsNotEqualsTest) {
  MethodArgument* doubleArgument = new MethodArgument(PrimitiveTypes::DOUBLE_TYPE, "argDouble2");
  vector<MethodArgument*> arguments;
  arguments.push_back(doubleArgument);
  vector<const Model*> thrownExceptions;
  Method method(mModel,
                "mymethod",
                AccessLevel::PUBLIC_ACCESS,
                PrimitiveTypes::BOOLEAN_TYPE,
                arguments,
                thrownExceptions,
                NULL,
                0);
  
  ASSERT_FALSE(IMethodDescriptor::compare(&method, mMethod));
}

TEST_F(IMethodDescriptorTest, typeOfArgumentsNotEqualsTest) {
  MethodArgument* doubleArgument = new MethodArgument(PrimitiveTypes::DOUBLE_TYPE, "argDouble2");
  MethodArgument* charArgument = new MethodArgument(PrimitiveTypes::INT_TYPE, "argChar2");
  vector<MethodArgument*> arguments;
  arguments.push_back(doubleArgument);
  arguments.push_back(charArgument);
  vector<const Model*> thrownExceptions;
  Method method(mModel,
                "mymethod",
                AccessLevel::PUBLIC_ACCESS,
                PrimitiveTypes::BOOLEAN_TYPE,
                arguments,
                thrownExceptions,
                NULL,
                0);
  
  ASSERT_FALSE(IMethodDescriptor::compare(&method, mMethod));
}

TEST_F(IMethodDescriptorTest, getLLVMFunctionTypeTest) {
  MethodArgument* intArgument = new MethodArgument(PrimitiveTypes::INT_TYPE, "intargument");
  vector<MethodArgument*> arguments;
  vector<const Model*> thrownExceptions;
  arguments.push_back(intArgument);
  MethodSignature method(mModel,
                         "foo",
                         PrimitiveTypes::FLOAT_TYPE,
                         arguments,
                         thrownExceptions);
  FunctionType* functionType = IMethodDescriptor::getLLVMFunctionType(mContext, &method, mModel);
  
  EXPECT_EQ(functionType->getReturnType(), Type::getFloatTy(mLLVMContext));
  EXPECT_EQ(functionType->getNumParams(), 3u);
  EXPECT_EQ(functionType->getParamType(0), mModel->getLLVMType(mContext));
  EXPECT_EQ(functionType->getParamType(1), mMainThread->getLLVMType(mContext));
  EXPECT_EQ(functionType->getParamType(2), Type::getInt32Ty(mLLVMContext));
}
