//
//  TestExternalStaticMethod.cpp
//  runtests
//
//  Created by Vladimir Fridman on 9/29/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ExternalStaticMethod}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/ExternalStaticMethod.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct ExternalStaticMethodTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  ExternalStaticMethod* mStaticMethod;
  Model* mModel;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  ExternalStaticMethodTest() :
  mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    MethodArgument* doubleArgument = new MethodArgument(PrimitiveTypes::DOUBLE_TYPE, "argDouble");
    MethodArgument* charArgument = new MethodArgument(PrimitiveTypes::CHAR_TYPE, "argChar");
    std::vector<MethodArgument*> arguments;
    arguments.push_back(doubleArgument);
    arguments.push_back(charArgument);
    vector<const Model*> thrownExceptions;

    vector<Type*> types;
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MObject";
    StructType* structType = StructType::create(mLLVMContext, modelFullName);
    structType->setBody(types);
    vector<IField*> fields;
    fields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "foo", 0));
    fields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "bar", 0));
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS, modelFullName, structType);
    mModel->setFields(fields, 0u);
    
    mStaticMethod = new ExternalStaticMethod(mModel,
                                             "mymethod",
                                             PrimitiveTypes::BOOLEAN_TYPE,
                                             arguments,
                                             thrownExceptions,
                                             new MethodQualifiers(0),
                                             0);

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~ExternalStaticMethodTest() {
    delete mStringStream;
  }
};

TEST_F(ExternalStaticMethodTest, basicStaticMethodTest) {
  EXPECT_STREQ("mymethod", mStaticMethod->getName().c_str());
  EXPECT_EQ(PrimitiveTypes::BOOLEAN_TYPE, mStaticMethod->getReturnType());
  EXPECT_EQ(2u, mStaticMethod->getArguments().size());
  EXPECT_TRUE(mStaticMethod->isStatic());
  EXPECT_STREQ("systems.vos.wisey.compiler.tests.MObject.mymethod",
               mStaticMethod->getTypeName().c_str());
}

TEST_F(ExternalStaticMethodTest, elementTypeTest) {
  EXPECT_FALSE(mStaticMethod->isConstant());
  EXPECT_FALSE(mStaticMethod->isField());
  EXPECT_FALSE(mStaticMethod->isMethod());
  EXPECT_TRUE(mStaticMethod->isStaticMethod());
  EXPECT_FALSE(mStaticMethod->isMethodSignature());
  EXPECT_FALSE(mStaticMethod->isLLVMFunction());
}

TEST_F(ExternalStaticMethodTest, getLLVMTypeTest) {
  MethodArgument* intArgument = new MethodArgument(PrimitiveTypes::INT_TYPE, "intargument");
  std::vector<MethodArgument*> arguments;
  arguments.push_back(intArgument);
  vector<const Model*> thrownExceptions;
  ExternalStaticMethod staticMethod(mModel,
                                    "foo",
                                    PrimitiveTypes::FLOAT_TYPE,
                                    arguments,
                                    thrownExceptions,
                                    new MethodQualifiers(0),
                                    0);

  vector<Type*> argumentTypes;
  Interface* threadInterface = mContext.getInterface(Names::getThreadInterfaceFullName(), 0);
  argumentTypes.push_back(threadInterface->getLLVMType(mContext));
  Controller* callStackController = mContext.getController(Names::getCallStackControllerFullName(),
                                                           0);
  argumentTypes.push_back(callStackController->getLLVMType(mContext));
  argumentTypes.push_back(PrimitiveTypes::INT_TYPE->getLLVMType(mContext));
  Type* llvmReturnType = PrimitiveTypes::FLOAT_TYPE->getLLVMType(mContext);
  FunctionType* expectedType = FunctionType::get(llvmReturnType, argumentTypes, false);
  
  FunctionType* actualType = staticMethod.getLLVMType(mContext);
  
  EXPECT_EQ(expectedType, actualType);
}

TEST_F(ExternalStaticMethodTest, definePublicFunctionTest) {
  MethodArgument* intArgument = new MethodArgument(PrimitiveTypes::INT_TYPE, "intargument");
  std::vector<MethodArgument*> arguments;
  arguments.push_back(intArgument);
  vector<const Model*> thrownExceptions;
  ExternalStaticMethod staticMethod(mModel,
                                    "foo",
                                    PrimitiveTypes::FLOAT_TYPE,
                                    arguments,
                                    thrownExceptions,
                                    new MethodQualifiers(0),
                                    0);
  Function* function = staticMethod.defineFunction(mContext);
  
  *mStringStream << *function;
  string expected = "\ndeclare float @systems.vos.wisey.compiler.tests.MObject.foo(%wisey.lang.threads.IThread*, %wisey.lang.CCallStack*, i32)\n";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  EXPECT_EQ(mContext.getMainFunction(), nullptr);
}

TEST_F(ExternalStaticMethodTest, isOwnerTest) {
  EXPECT_FALSE(mStaticMethod->isOwner());
}

TEST_F(ExternalStaticMethodTest, isReferenceTest) {
  EXPECT_FALSE(mStaticMethod->isReference());
}

TEST_F(ExternalStaticMethodTest, isPrimitiveTest) {
  EXPECT_FALSE(mStaticMethod->isPrimitive());
}

TEST_F(ExternalStaticMethodTest, isArrayTest) {
  EXPECT_FALSE(mStaticMethod->isArray());
}

TEST_F(ExternalStaticMethodTest, isFunctionTest) {
  EXPECT_TRUE(mStaticMethod->isFunction());
}

TEST_F(ExternalStaticMethodTest, isPackageTest) {
  EXPECT_FALSE(mStaticMethod->isPackage());
}
