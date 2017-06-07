//
//  TestMethod.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/3/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link Method}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "wisey/CompoundStatement.hpp"
#include "wisey/Method.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct MethodTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Method* mMethod;
  Model* mModel;
  Block* mBlock;
  CompoundStatement mCompoundStatement;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  MethodTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mBlock(new Block()),
  mCompoundStatement(CompoundStatement(mBlock)) {

    MethodArgument* doubleArgument = new MethodArgument(PrimitiveTypes::DOUBLE_TYPE, "argDouble");
    MethodArgument* charArgument = new MethodArgument(PrimitiveTypes::CHAR_TYPE, "argChar");
    std::vector<MethodArgument*> arguments;
    arguments.push_back(doubleArgument);
    arguments.push_back(charArgument);
    vector<const IType*> thrownExceptions;
    mMethod = new Method("mymethod",
                         AccessLevel::PUBLIC_ACCESS,
                         PrimitiveTypes::BOOLEAN_TYPE,
                         arguments,
                         thrownExceptions,
                         NULL,
                         0);

    vector<Type*> types;
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MObject";
    StructType* structType = StructType::create(mLLVMContext, modelFullName);
    structType->setBody(types);
    map<string, Field*> fields;
    ExpressionList fieldArguments;
    fields["foo"] = new Field(PrimitiveTypes::INT_TYPE, "foo", 0, fieldArguments);
    fields["bar"] = new Field(PrimitiveTypes::INT_TYPE, "bar", 1, fieldArguments);
    mModel = new Model(modelFullName, structType);
    mModel->setFields(fields);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~MethodTest() {
    delete mStringStream;
  }
};

TEST_F(MethodTest, basicMethodsTest) {
  ASSERT_STREQ(mMethod->getName().c_str(), "mymethod");
  ASSERT_EQ(mMethod->getReturnType(), PrimitiveTypes::BOOLEAN_TYPE);
  ASSERT_EQ(mMethod->getArguments().size(), 2u);
}

TEST_F(MethodTest, defineFunctionTest) {
  MethodArgument* intArgument = new MethodArgument(PrimitiveTypes::INT_TYPE, "intargument");
  std::vector<MethodArgument*> arguments;
  arguments.push_back(intArgument);
  vector<const IType*> thrownExceptions;
  Method method("foo",
                AccessLevel::PUBLIC_ACCESS,
                PrimitiveTypes::FLOAT_TYPE,
                arguments,
                thrownExceptions,
                &mCompoundStatement,
                0);
  Function* function = method.defineFunction(mContext, mModel);
  
  *mStringStream << *function;
  string expected = "\ndeclare internal float @systems.vos.wisey.compiler.tests.MObject.foo("
    "%systems.vos.wisey.compiler.tests.MObject*, i32)\n";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  EXPECT_EQ(mContext.getMainFunction(), nullptr);
}

TEST_F(MethodTest, generateIRTest) {
  MethodArgument* intArgument = new MethodArgument(PrimitiveTypes::INT_TYPE, "intargument");
  std::vector<MethodArgument*> arguments;
  arguments.push_back(intArgument);
  vector<const IType*> thrownExceptions;
  Method method("foo",
                AccessLevel::PUBLIC_ACCESS,
                PrimitiveTypes::FLOAT_TYPE,
                arguments,
                thrownExceptions,
                &mCompoundStatement,
                0);
  Function* function = method.defineFunction(mContext, mModel);
  method.generateIR(mContext, mModel);
  
  *mStringStream << *function;
  string expected =
  "\ndefine internal float @systems.vos.wisey.compiler.tests.MObject.foo("
  "%systems.vos.wisey.compiler.tests.MObject* %this, i32 %intargument) {"
  "\nentry:"
  "\n  %this.param = alloca %systems.vos.wisey.compiler.tests.MObject*"
  "\n  store %systems.vos.wisey.compiler.tests.MObject* %this, "
  "%systems.vos.wisey.compiler.tests.MObject** %this.param"
  "\n  %intargument.param = alloca i32"
  "\n  store i32 %intargument, i32* %intargument.param"
  "\n  ret void"
  "\n}"
  "\n";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  EXPECT_EQ(mContext.getMainFunction(), nullptr);
}

TEST_F(TestFileSampleRunner, methodMissesThrowsQualifierDeathTest) {
  expectFailCompile("tests/samples/test_missing_throws.yz",
                    1,
                    "Error: Method doThrow neither handles the exception "
                    "systems.vos.wisey.compiler.tests.MException nor throws it");
}

