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
#include "wisey/Argument.hpp"
#include "wisey/ExternalStaticMethod.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/IRGenerationContext.hpp"
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
    
    wisey::Argument* doubleArgument = new wisey::Argument(PrimitiveTypes::DOUBLE, "argDouble");
    wisey::Argument* charArgument = new wisey::Argument(PrimitiveTypes::CHAR, "argChar");
    std::vector<const wisey::Argument*> arguments;
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
    fields.push_back(new FixedField(PrimitiveTypes::INT, "foo", 0));
    fields.push_back(new FixedField(PrimitiveTypes::INT, "bar", 0));
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                             modelFullName,
                             structType,
                             mContext.getImportProfile(),
                             0);
    mModel->setFields(mContext, fields, 0u);
    
    mStaticMethod = new ExternalStaticMethod(mModel,
                                             "mymethod",
                                             PrimitiveTypes::BOOLEAN,
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
  EXPECT_EQ(PrimitiveTypes::BOOLEAN, mStaticMethod->getReturnType());
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
  wisey::Argument* intArgument = new wisey::Argument(PrimitiveTypes::INT, "intargument");
  std::vector<const wisey::Argument*> arguments;
  arguments.push_back(intArgument);
  vector<const Model*> thrownExceptions;
  ExternalStaticMethod staticMethod(mModel,
                                    "foo",
                                    PrimitiveTypes::FLOAT,
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
  argumentTypes.push_back(PrimitiveTypes::INT->getLLVMType(mContext));
  Type* llvmReturnType = PrimitiveTypes::FLOAT->getLLVMType(mContext);
  FunctionType* expectedType = FunctionType::get(llvmReturnType, argumentTypes, false);
  
  FunctionType* actualType = staticMethod.getLLVMType(mContext);
  
  EXPECT_EQ(expectedType, actualType);
}

TEST_F(ExternalStaticMethodTest, definePublicFunctionTest) {
  wisey::Argument* intArgument = new wisey::Argument(PrimitiveTypes::INT, "intargument");
  std::vector<const wisey::Argument*> arguments;
  arguments.push_back(intArgument);
  vector<const Model*> thrownExceptions;
  ExternalStaticMethod staticMethod(mModel,
                                    "foo",
                                    PrimitiveTypes::FLOAT,
                                    arguments,
                                    thrownExceptions,
                                    new MethodQualifiers(0),
                                    0);
  Function* function = staticMethod.defineFunction(mContext);
  
  *mStringStream << *function;
  string expected = "\ndeclare float @systems.vos.wisey.compiler.tests.MObject.foo(%wisey.threads.IThread*, %wisey.threads.CCallStack*, i32)\n";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  EXPECT_EQ(mContext.getMainFunction(), nullptr);
}

TEST_F(ExternalStaticMethodTest, isTypeKindTest) {
  EXPECT_FALSE(mStaticMethod->isPrimitive());
  EXPECT_FALSE(mStaticMethod->isOwner());
  EXPECT_FALSE(mStaticMethod->isReference());
  EXPECT_FALSE(mStaticMethod->isArray());
  EXPECT_TRUE(mStaticMethod->isFunction());
  EXPECT_FALSE(mStaticMethod->isPackage());
  EXPECT_FALSE(mStaticMethod->isNative());
  EXPECT_FALSE(mStaticMethod->isPointer());
  EXPECT_FALSE(mStaticMethod->isImmutable());
}
