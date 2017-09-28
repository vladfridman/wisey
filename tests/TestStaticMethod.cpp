//
//  TestStaticMethod.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link StaticMethod}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "wisey/CompoundStatement.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/StaticMethod.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::Test;

struct StaticMethodTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  StaticMethod* mStaticMethod;
  Model* mModel;
  Block* mBlock;
  CompoundStatement mCompoundStatement;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  StaticMethodTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mBlock(new Block()),
  mCompoundStatement(CompoundStatement(mBlock)) {
    
    MethodArgument* doubleArgument = new MethodArgument(PrimitiveTypes::DOUBLE_TYPE, "argDouble");
    MethodArgument* charArgument = new MethodArgument(PrimitiveTypes::CHAR_TYPE, "argChar");
    std::vector<MethodArgument*> arguments;
    arguments.push_back(doubleArgument);
    arguments.push_back(charArgument);
    vector<const Model*> thrownExceptions;
    mStaticMethod = new StaticMethod("mymethod",
                                     AccessLevel::PUBLIC_ACCESS,
                                     PrimitiveTypes::BOOLEAN_TYPE,
                                     arguments,
                                     thrownExceptions,
                                     NULL);
    
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
  
  ~StaticMethodTest() {
    delete mStringStream;
  }
};

TEST_F(StaticMethodTest, basicStaticMethodTest) {
  ASSERT_STREQ(mStaticMethod->getName().c_str(), "mymethod");
  ASSERT_EQ(mStaticMethod->getReturnType(), PrimitiveTypes::BOOLEAN_TYPE);
  ASSERT_EQ(mStaticMethod->getArguments().size(), 2u);
}

TEST_F(StaticMethodTest, definePublicFunctionTest) {
  MethodArgument* intArgument = new MethodArgument(PrimitiveTypes::INT_TYPE, "intargument");
  std::vector<MethodArgument*> arguments;
  arguments.push_back(intArgument);
  vector<const Model*> thrownExceptions;
  StaticMethod staticMethod("foo",
                            AccessLevel::PUBLIC_ACCESS,
                            PrimitiveTypes::FLOAT_TYPE,
                            arguments,
                            thrownExceptions,
                            &mCompoundStatement);
  Function* function = staticMethod.defineFunction(mContext, mModel);
  
  *mStringStream << *function;
  string expected = "\ndeclare float @systems.vos.wisey.compiler.tests.MObject.foo(i32)\n";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  EXPECT_EQ(mContext.getMainFunction(), nullptr);
}

TEST_F(StaticMethodTest, definePrivateFunctionTest) {
  MethodArgument* intArgument = new MethodArgument(PrimitiveTypes::INT_TYPE, "intargument");
  std::vector<MethodArgument*> arguments;
  arguments.push_back(intArgument);
  vector<const Model*> thrownExceptions;
  StaticMethod staticMethod("foo",
                            AccessLevel::PRIVATE_ACCESS,
                            PrimitiveTypes::FLOAT_TYPE,
                            arguments,
                            thrownExceptions,
                            &mCompoundStatement);
  Function* function = staticMethod.defineFunction(mContext, mModel);
  
  *mStringStream << *function;
  string expected = "\ndeclare internal float @systems.vos.wisey.compiler.tests.MObject.foo(i32)\n";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  EXPECT_EQ(mContext.getMainFunction(), nullptr);
}

TEST_F(StaticMethodTest, generateIRTest) {
  MethodArgument* intArgument = new MethodArgument(PrimitiveTypes::INT_TYPE, "intargument");
  std::vector<MethodArgument*> arguments;
  arguments.push_back(intArgument);
  vector<const Model*> thrownExceptions;
  StaticMethod staticMethod("foo",
                            AccessLevel::PUBLIC_ACCESS,
                            PrimitiveTypes::VOID_TYPE,
                            arguments,
                            thrownExceptions,
                            &mCompoundStatement);
  Function* function = staticMethod.defineFunction(mContext, mModel);
  staticMethod.generateIR(mContext, mModel);
  
  *mStringStream << *function;
  string expected =
  "\ndefine void @systems.vos.wisey.compiler.tests.MObject.foo(i32 %intargument) {"
  "\nentry:"
  "\n  %intargument.param = alloca i32"
  "\n  store i32 %intargument, i32* %intargument.param"
  "\n  ret void"
  "\n}"
  "\n";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  EXPECT_EQ(mContext.getMainFunction(), nullptr);
}

TEST_F(StaticMethodTest, printToStreamTest) {
  stringstream stringStream;
  mStaticMethod->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("  static boolean mymethod(\n"
               "    double argDouble,\n"
               "    char argChar);\n",
               stringStream.str().c_str());
}

TEST_F(TestFileSampleRunner, staticMethodDefinitionRunTest) {
  runFile("tests/samples/test_static_method_definition.yz", "2018");
}

TEST_F(TestFileSampleRunner, staticMethodMissingThrowsDeathRunTest) {
  expectFailCompile("tests/samples/test_static_method_missing_throws.yz",
                    1,
                    "Error: Static method doThrow neither handles the exception "
                    "systems.vos.wisey.compiler.tests.MException nor throws it");
}

TEST_F(TestFileSampleRunner, staticMethodReturnTypeIsNotVoidDeathRunTest) {
  expectFailCompile("tests/samples/test_static_method_return_type_is_not_void.yz",
                    1,
                    "Error: Static method foo must return a value of type int");
}

