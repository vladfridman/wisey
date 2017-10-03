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
#include "wisey/ExternalStaticMethod.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/MethodArgument.hpp"
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
    
    MethodArgument* doubleArgument = new MethodArgument(PrimitiveTypes::DOUBLE_TYPE, "argDouble");
    MethodArgument* charArgument = new MethodArgument(PrimitiveTypes::CHAR_TYPE, "argChar");
    std::vector<MethodArgument*> arguments;
    arguments.push_back(doubleArgument);
    arguments.push_back(charArgument);
    vector<const Model*> thrownExceptions;
    mStaticMethod = new ExternalStaticMethod("mymethod",
                                             PrimitiveTypes::BOOLEAN_TYPE,
                                             arguments,
                                             thrownExceptions);

    vector<Type*> types;
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MObject";
    StructType* structType = StructType::create(mLLVMContext, modelFullName);
    structType->setBody(types);
    vector<Field*> fields;
    ExpressionList fieldArguments;
    fields.push_back(new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "foo", 0, fieldArguments));
    fields.push_back(new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "bar", 1, fieldArguments));
    mModel = new Model(modelFullName, structType);
    mModel->setFields(fields);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~ExternalStaticMethodTest() {
    delete mStringStream;
  }
};

TEST_F(ExternalStaticMethodTest, basicStaticMethodTest) {
  ASSERT_STREQ(mStaticMethod->getName().c_str(), "mymethod");
  ASSERT_EQ(mStaticMethod->getReturnType(), PrimitiveTypes::BOOLEAN_TYPE);
  ASSERT_EQ(mStaticMethod->getArguments().size(), 2u);
  ASSERT_TRUE(mStaticMethod->isStatic());
}

TEST_F(ExternalStaticMethodTest, definePublicFunctionTest) {
  MethodArgument* intArgument = new MethodArgument(PrimitiveTypes::INT_TYPE, "intargument");
  std::vector<MethodArgument*> arguments;
  arguments.push_back(intArgument);
  vector<const Model*> thrownExceptions;
  ExternalStaticMethod staticMethod("foo", PrimitiveTypes::FLOAT_TYPE, arguments, thrownExceptions);
  Function* function = staticMethod.defineFunction(mContext, mModel);
  
  *mStringStream << *function;
  string expected = "\ndeclare float @systems.vos.wisey.compiler.tests.MObject.foo(i32)\n";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  EXPECT_EQ(mContext.getMainFunction(), nullptr);
}
