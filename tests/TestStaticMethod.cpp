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
#include "TestPrefix.hpp"
#include "wisey/CompoundStatement.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ProgramPrefix.hpp"
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
  mCompoundStatement(CompoundStatement(mBlock, 0)) {
    TestPrefix::generateIR(mContext);
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    
    MethodArgument* doubleArgument = new MethodArgument(PrimitiveTypes::DOUBLE_TYPE, "argDouble");
    MethodArgument* charArgument = new MethodArgument(PrimitiveTypes::CHAR_TYPE, "argChar");
    std::vector<MethodArgument*> arguments;
    arguments.push_back(doubleArgument);
    arguments.push_back(charArgument);
    vector<const Model*> thrownExceptions;
    string rceFullName = Names::getLangPackageName() + "." +
    Names::getReferenceCountExceptionName();
    thrownExceptions.push_back(mContext.getModel(rceFullName));
    mStaticMethod = new StaticMethod(mModel,
                                     "mymethod",
                                     AccessLevel::PUBLIC_ACCESS,
                                     PrimitiveTypes::BOOLEAN_TYPE,
                                     arguments,
                                     thrownExceptions,
                                     NULL,
                                     0);
    
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
  ASSERT_TRUE(mStaticMethod->isStatic());
}

TEST_F(StaticMethodTest, definePublicFunctionTest) {
  MethodArgument* intArgument = new MethodArgument(PrimitiveTypes::INT_TYPE, "intargument");
  std::vector<MethodArgument*> arguments;
  arguments.push_back(intArgument);
  vector<const Model*> thrownExceptions;
  StaticMethod staticMethod(mModel,
                            "foo",
                            AccessLevel::PUBLIC_ACCESS,
                            PrimitiveTypes::FLOAT_TYPE,
                            arguments,
                            thrownExceptions,
                            &mCompoundStatement,
                            0);
  Function* function = staticMethod.defineFunction(mContext);
  
  *mStringStream << *function;
  string expected = "\ndeclare float @systems.vos.wisey.compiler.tests.MObject.foo("
  "%wisey.lang.CThread*, i32)\n";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  EXPECT_EQ(mContext.getMainFunction(), nullptr);
}

TEST_F(StaticMethodTest, definePrivateFunctionTest) {
  MethodArgument* intArgument = new MethodArgument(PrimitiveTypes::INT_TYPE, "intargument");
  std::vector<MethodArgument*> arguments;
  arguments.push_back(intArgument);
  vector<const Model*> thrownExceptions;
  StaticMethod staticMethod(mModel,
                            "foo",
                            AccessLevel::PRIVATE_ACCESS,
                            PrimitiveTypes::FLOAT_TYPE,
                            arguments,
                            thrownExceptions,
                            &mCompoundStatement,
                            0);
  Function* function = staticMethod.defineFunction(mContext);
  
  *mStringStream << *function;
  string expected = "\ndeclare internal float @systems.vos.wisey.compiler.tests.MObject.foo("
  "%wisey.lang.CThread*, i32)\n";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  EXPECT_EQ(mContext.getMainFunction(), nullptr);
}

TEST_F(StaticMethodTest, generateIRTest) {
  MethodArgument* intArgument = new MethodArgument(PrimitiveTypes::INT_TYPE, "intargument");
  std::vector<MethodArgument*> arguments;
  arguments.push_back(intArgument);
  vector<const Model*> thrownExceptions;
  StaticMethod staticMethod(mModel,
                            "foo",
                            AccessLevel::PUBLIC_ACCESS,
                            PrimitiveTypes::VOID_TYPE,
                            arguments,
                            thrownExceptions,
                            &mCompoundStatement,
                            0);
  Function* function = staticMethod.defineFunction(mContext);
  staticMethod.generateIR(mContext);
  
  *mStringStream << *function;
  string expected =
  "\ndefine void @systems.vos.wisey.compiler.tests.MObject.foo("
  "%wisey.lang.CThread* %thread, i32 %intargument) {"
  "\nentry:"
  "\n  %0 = bitcast %wisey.lang.CThread* %thread to i64*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectUnsafely(i64* %0, i64 1)"
  "\n  %1 = bitcast %wisey.lang.CThread* %thread to i64*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectUnsafely(i64* %1, i64 -1)"
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
               "    char argChar) throws wisey.lang.MReferenceCountException;\n",
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

