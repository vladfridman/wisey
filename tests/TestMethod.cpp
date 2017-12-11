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
#include "TestPrefix.hpp"
#include "wisey/CompoundStatement.hpp"
#include "wisey/Method.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ProgramPrefix.hpp"

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
    mMethod = new Method("mymethod",
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
  
  ~MethodTest() {
    delete mStringStream;
  }
};

TEST_F(MethodTest, basicMethodTest) {
  ASSERT_STREQ(mMethod->getName().c_str(), "mymethod");
  ASSERT_EQ(mMethod->getReturnType(), PrimitiveTypes::BOOLEAN_TYPE);
  ASSERT_EQ(mMethod->getArguments().size(), 2u);
  ASSERT_FALSE(mMethod->isStatic());
}

TEST_F(MethodTest, definePublicFunctionTest) {
  MethodArgument* intArgument = new MethodArgument(PrimitiveTypes::INT_TYPE, "intargument");
  std::vector<MethodArgument*> arguments;
  arguments.push_back(intArgument);
  vector<const Model*> thrownExceptions;
  Method method("foo",
                AccessLevel::PUBLIC_ACCESS,
                PrimitiveTypes::FLOAT_TYPE,
                arguments,
                thrownExceptions,
                &mCompoundStatement,
                0);
  Function* function = method.defineFunction(mContext, mModel);
  
  *mStringStream << *function;
  string expected = "\ndeclare float @systems.vos.wisey.compiler.tests.MObject.foo("
  "%systems.vos.wisey.compiler.tests.MObject*, %wisey.lang.CThread*, i32)\n";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  EXPECT_EQ(mContext.getMainFunction(), nullptr);
}

TEST_F(MethodTest, definePrivateFunctionTest) {
  MethodArgument* intArgument = new MethodArgument(PrimitiveTypes::INT_TYPE, "intargument");
  std::vector<MethodArgument*> arguments;
  arguments.push_back(intArgument);
  vector<const Model*> thrownExceptions;
  Method method("foo",
                AccessLevel::PRIVATE_ACCESS,
                PrimitiveTypes::FLOAT_TYPE,
                arguments,
                thrownExceptions,
                &mCompoundStatement,
                0);
  Function* function = method.defineFunction(mContext, mModel);
  
  *mStringStream << *function;
  string expected = "\ndeclare internal float @systems.vos.wisey.compiler.tests.MObject.foo("
  "%systems.vos.wisey.compiler.tests.MObject*, %wisey.lang.CThread*, i32)\n";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  EXPECT_EQ(mContext.getMainFunction(), nullptr);
}

TEST_F(MethodTest, generateIRTest) {
  MethodArgument* intArgument = new MethodArgument(PrimitiveTypes::INT_TYPE, "intargument");
  std::vector<MethodArgument*> arguments;
  arguments.push_back(intArgument);
  vector<const Model*> thrownExceptions;
  Method method("foo",
                AccessLevel::PUBLIC_ACCESS,
                PrimitiveTypes::VOID_TYPE,
                arguments,
                thrownExceptions,
                &mCompoundStatement,
                0);
  Function* function = method.defineFunction(mContext, mModel);
  method.generateIR(mContext, mModel);
  
  *mStringStream << *function;
  string expected =
  "\ndefine void @systems.vos.wisey.compiler.tests.MObject.foo("
  "%systems.vos.wisey.compiler.tests.MObject* %this, %wisey.lang.CThread* %thread, "
  "i32 %intargument) {"
  "\nentry:"
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.MObject* %this to i64*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i64* %0, i64 1)"
  "\n  %1 = bitcast %wisey.lang.CThread* %thread to i64*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectUnsafely(i64* %1, i64 1)"
  "\n  %2 = bitcast %wisey.lang.CThread* %thread to i64*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectUnsafely(i64* %2, i64 -1)"
  "\n  %3 = bitcast %systems.vos.wisey.compiler.tests.MObject* %this to i64*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i64* %3, i64 -1)"
  "\n  ret void"
  "\n}"
  "\n";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  EXPECT_EQ(mContext.getMainFunction(), nullptr);
}

TEST_F(MethodTest, printToStreamTest) {
  stringstream stringStream;
  mMethod->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("  boolean mymethod(\n"
               "    double argDouble,\n"
               "    char argChar) throws wisey.lang.MReferenceCountException;\n",
               stringStream.str().c_str());
}

TEST_F(TestFileSampleRunner, methodMissingThrowsDeathRunTest) {
  expectFailCompile("tests/samples/test_method_missing_throws.yz",
                    1,
                    "Error: Method doThrow neither handles the exception "
                    "systems.vos.wisey.compiler.tests.MException nor throws it");
}

TEST_F(TestFileSampleRunner, methodReturnTypeIsNotVoidDeathRunTest) {
  expectFailCompile("tests/samples/test_method_return_type_is_not_void.yz",
                    1,
                    "Error: Method run must return a value of type int");
}

