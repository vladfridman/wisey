//
//  TestExternalMethod.cpp
//  runtests
//
//  Created by Vladimir Fridman on 9/29/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ExternalMethod}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestPrefix.hpp"
#include "Argument.hpp"
#include "ExternalMethod.hpp"
#include "IRGenerationContext.hpp"
#include "Names.hpp"
#include "PrimitiveTypes.hpp"
#include "ReceivedField.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct ExternalMethodTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  ExternalMethod* mMethod;
  Model* mModel;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  ExternalMethodTest() :
  mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    wisey::Argument* doubleArgument = new wisey::Argument(PrimitiveTypes::DOUBLE, "argDouble");
    wisey::Argument* charArgument = new wisey::Argument(PrimitiveTypes::CHAR, "argChar");
    std::vector<const wisey::Argument*> arguments;
    arguments.push_back(doubleArgument);
    arguments.push_back(charArgument);
    vector<const Model*> thrownExceptions;

    vector<Type*> types;
    types.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                    ->getPointerTo()->getPointerTo());
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MObject";
    StructType* structType = StructType::create(mLLVMContext, modelFullName);
    structType->setBody(types);
    vector<IField*> fields;
    fields.push_back(new ReceivedField(PrimitiveTypes::INT, "foo", 0));
    fields.push_back(new ReceivedField(PrimitiveTypes::INT, "bar", 0));
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                             modelFullName,
                             structType,
                             mContext.getImportProfile(),
                             0);
    mModel->setFields(mContext, fields, 1u);

    mMethod = new ExternalMethod(mModel,
                                 "mymethod",
                                 PrimitiveTypes::BOOLEAN,
                                 arguments,
                                 thrownExceptions,
                                 new MethodQualifiers(0),
                                 0);

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~ExternalMethodTest() {
    delete mStringStream;
  }
};

TEST_F(ExternalMethodTest, basicMethodTest) {
  EXPECT_STREQ("mymethod", mMethod->getName().c_str());
  EXPECT_EQ(PrimitiveTypes::BOOLEAN, mMethod->getReturnType());
  EXPECT_EQ(2u, mMethod->getArguments().size());
  EXPECT_FALSE(mMethod->isStatic());
  EXPECT_STREQ("systems.vos.wisey.compiler.tests.MObject.method.mymethod",
               mMethod->getTypeName().c_str());
}

TEST_F(ExternalMethodTest, elementTypeTest) {
  EXPECT_FALSE(mMethod->isConstant());
  EXPECT_FALSE(mMethod->isField());
  EXPECT_TRUE(mMethod->isMethod());
  EXPECT_FALSE(mMethod->isStaticMethod());
  EXPECT_FALSE(mMethod->isMethodSignature());
  EXPECT_FALSE(mMethod->isLLVMFunction());
}

TEST_F(ExternalMethodTest, getLLVMTypeTest) {
  wisey::Argument* intArgument = new wisey::Argument(PrimitiveTypes::INT, "intargument");
  std::vector<const wisey::Argument*> arguments;
  arguments.push_back(intArgument);
  vector<const Model*> thrownExceptions;
  ExternalMethod method(mModel,
                        "foo",
                        PrimitiveTypes::FLOAT,
                        arguments,
                        thrownExceptions,
                        new MethodQualifiers(0),
                        0);

  vector<Type*> argumentTypes;
  argumentTypes.push_back(mModel->getLLVMType(mContext));

  Interface* threadInterface = mContext.getInterface(Names::getThreadInterfaceFullName(), 0);
  argumentTypes.push_back(threadInterface->getLLVMType(mContext));
  Controller* callStack = mContext.getController(Names::getCallStackControllerFullName(), 0);
  argumentTypes.push_back(callStack->getLLVMType(mContext));
  argumentTypes.push_back(PrimitiveTypes::INT->getLLVMType(mContext));
  Type* llvmReturnType = PrimitiveTypes::FLOAT->getLLVMType(mContext);
  FunctionType* expectedType = FunctionType::get(llvmReturnType, argumentTypes, false);
  
  FunctionType* actualType = method.getLLVMType(mContext);
  
  EXPECT_EQ(expectedType, actualType);
}

TEST_F(ExternalMethodTest, declareFunctionTest) {
  wisey::Argument* intArgument = new wisey::Argument(PrimitiveTypes::INT, "intargument");
  std::vector<const wisey::Argument*> arguments;
  arguments.push_back(intArgument);
  vector<const Model*> thrownExceptions;
  ExternalMethod method(mModel,
                        "foo",
                        PrimitiveTypes::FLOAT,
                        arguments,
                        thrownExceptions,
                        new MethodQualifiers(0),
                        0);
  Function* function = method.declareFunction(mContext);
  
  *mStringStream << *function;
  string expected = "\ndeclare float @systems.vos.wisey.compiler.tests.MObject.method.foo(%systems.vos.wisey.compiler.tests.MObject*, %wisey.threads.IThread*, %wisey.threads.CCallStack*, i32)\n";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ExternalMethodTest, isTypeKindTest) {
  EXPECT_FALSE(mMethod->isPrimitive());
  EXPECT_FALSE(mMethod->isOwner());
  EXPECT_FALSE(mMethod->isReference());
  EXPECT_FALSE(mMethod->isArray());
  EXPECT_TRUE(mMethod->isFunction());
  EXPECT_FALSE(mMethod->isPackage());
  EXPECT_FALSE(mMethod->isNative());
  EXPECT_FALSE(mMethod->isPointer());
  EXPECT_FALSE(mMethod->isImmutable());
}
