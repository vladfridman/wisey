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
#include "wisey/ExternalMethod.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypes.hpp"

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
    StructType* structType = StructType::create(mLLVMContext, modelFullName);
    structType->setBody(types);
    vector<Field*> fields;
    InjectionArgumentList fieldArguments;
    fields.push_back(new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "foo", fieldArguments));
    fields.push_back(new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "bar", fieldArguments));
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS, modelFullName, structType);
    mModel->setFields(fields, 1u);
    
    mMethod = new ExternalMethod(mModel,
                                 "mymethod",
                                 PrimitiveTypes::BOOLEAN_TYPE,
                                 arguments,
                                 thrownExceptions);

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~ExternalMethodTest() {
    delete mStringStream;
  }
};

TEST_F(ExternalMethodTest, basicMethodTest) {
  EXPECT_STREQ("mymethod", mMethod->getName().c_str());
  EXPECT_EQ(PrimitiveTypes::BOOLEAN_TYPE, mMethod->getReturnType());
  EXPECT_EQ(2u, mMethod->getArguments().size());
  EXPECT_FALSE(mMethod->isStatic());
  EXPECT_STREQ("systems.vos.wisey.compiler.tests.MObject.mymethod", mMethod->getTypeName().c_str());
  EXPECT_EQ(FUNCTION_TYPE, mMethod->getTypeKind());
}

TEST_F(ExternalMethodTest, getLLVMTypeTest) {
  MethodArgument* intArgument = new MethodArgument(PrimitiveTypes::INT_TYPE, "intargument");
  std::vector<MethodArgument*> arguments;
  arguments.push_back(intArgument);
  vector<const Model*> thrownExceptions;
  ExternalMethod method(mModel, "foo", PrimitiveTypes::FLOAT_TYPE, arguments, thrownExceptions);

  vector<Type*> argumentTypes;
  argumentTypes.push_back(mModel->getLLVMType(mContext));
  Controller* threadController = mContext.getController(Names::getThreadControllerFullName());
  argumentTypes.push_back(threadController->getLLVMType(mContext));
  argumentTypes.push_back(PrimitiveTypes::INT_TYPE->getLLVMType(mContext));
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  Type* llvmReturnType = PrimitiveTypes::FLOAT_TYPE->getLLVMType(mContext);
  FunctionType* expectedType = FunctionType::get(llvmReturnType, argTypesArray, false);
  
  FunctionType* actualType = method.getLLVMType(mContext);
  
  EXPECT_EQ(expectedType, actualType);
}

TEST_F(ExternalMethodTest, defineFunctionTest) {
  MethodArgument* intArgument = new MethodArgument(PrimitiveTypes::INT_TYPE, "intargument");
  std::vector<MethodArgument*> arguments;
  arguments.push_back(intArgument);
  vector<const Model*> thrownExceptions;
  ExternalMethod method(mModel, "foo", PrimitiveTypes::FLOAT_TYPE, arguments, thrownExceptions);
  Function* function = method.defineFunction(mContext);
  
  *mStringStream << *function;
  string expected = "\ndeclare float @systems.vos.wisey.compiler.tests.MObject.foo("
  "%systems.vos.wisey.compiler.tests.MObject*, %wisey.lang.CThread*, i32)\n";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  EXPECT_EQ(mContext.getMainFunction(), nullptr);
}
