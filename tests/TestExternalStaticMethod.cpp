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

#include "TestFileSampleRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/ExternalStaticMethod.hpp"
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
    vector<Field*> fields;
    InjectionArgumentList fieldArguments;
    fields.push_back(new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, NULL, "foo", fieldArguments));
    fields.push_back(new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, NULL, "bar", fieldArguments));
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS, modelFullName, structType);
    mModel->setFields(fields, 0u);
    
    mStaticMethod = new ExternalStaticMethod(mModel,
                                             "mymethod",
                                             PrimitiveTypes::BOOLEAN_TYPE,
                                             arguments,
                                             thrownExceptions);

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
  EXPECT_EQ(FUNCTION_TYPE, mStaticMethod->getTypeKind());
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
                                    thrownExceptions);

  vector<Type*> argumentTypes;
  Controller* threadController = mContext.getController(Names::getThreadControllerFullName());
  argumentTypes.push_back(threadController->getLLVMType(mContext));
  argumentTypes.push_back(PrimitiveTypes::INT_TYPE->getLLVMType(mContext));
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  Type* llvmReturnType = PrimitiveTypes::FLOAT_TYPE->getLLVMType(mContext);
  FunctionType* expectedType = FunctionType::get(llvmReturnType, argTypesArray, false);
  
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
                                    thrownExceptions);
  Function* function = staticMethod.defineFunction(mContext);
  
  *mStringStream << *function;
  string expected = "\ndeclare float @systems.vos.wisey.compiler.tests.MObject.foo("
  "%wisey.lang.CThread*, i32)\n";
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
