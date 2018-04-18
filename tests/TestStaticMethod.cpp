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

#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/Argument.hpp"
#include "wisey/CompoundStatement.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/Names.hpp"
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
  mCompoundStatement(CompoundStatement(mBlock, 0)) {
    TestPrefix::generateIR(mContext);
    
    wisey::Argument* doubleArgument = new wisey::Argument(PrimitiveTypes::DOUBLE_TYPE, "argDouble");
    wisey::Argument* charArgument = new wisey::Argument(PrimitiveTypes::CHAR_TYPE, "argChar");
    std::vector<const wisey::Argument*> arguments;
    arguments.push_back(doubleArgument);
    arguments.push_back(charArgument);
    vector<const Model*> thrownExceptions;
    string rceFullName = Names::getLangPackageName() + "." +
    Names::getReferenceCountExceptionName();
    thrownExceptions.push_back(mContext.getModel(rceFullName, 0));
    string modelFullName = "systems.vos.wisey.compiler.tests.MObject";
    vector<Type*> types;
    types.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                    ->getPointerTo()->getPointerTo());
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    StructType* structType = StructType::create(mLLVMContext, modelFullName);
    structType->setBody(types);
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS, modelFullName, structType, 0);
    mStaticMethod = new StaticMethod(mModel,
                                     "mymethod",
                                     AccessLevel::PUBLIC_ACCESS,
                                     PrimitiveTypes::BOOLEAN_TYPE,
                                     arguments,
                                     thrownExceptions,
                                     new MethodQualifiers(0),
                                     NULL,
                                     0);
    
    vector<IField*> fields;
    fields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "foo", 0));
    fields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "bar", 0));
    mModel->setFields(fields, 1u);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~StaticMethodTest() {
    delete mStringStream;
  }
};

TEST_F(StaticMethodTest, basicStaticMethodTest) {
  EXPECT_STREQ(mStaticMethod->getName().c_str(), "mymethod");
  EXPECT_EQ(mStaticMethod->getReturnType(), PrimitiveTypes::BOOLEAN_TYPE);
  EXPECT_EQ(mStaticMethod->getArguments().size(), 2u);
  EXPECT_TRUE(mStaticMethod->isStatic());
  EXPECT_STREQ("systems.vos.wisey.compiler.tests.MObject.mymethod",
               mStaticMethod->getTypeName().c_str());
}

TEST_F(StaticMethodTest, elementTypeTest) {
  EXPECT_FALSE(mStaticMethod->isConstant());
  EXPECT_FALSE(mStaticMethod->isField());
  EXPECT_FALSE(mStaticMethod->isMethod());
  EXPECT_TRUE(mStaticMethod->isStaticMethod());
  EXPECT_FALSE(mStaticMethod->isMethodSignature());
  EXPECT_FALSE(mStaticMethod->isLLVMFunction());
}

TEST_F(StaticMethodTest, getLLVMTypeTest) {
  wisey::Argument* intArgument = new wisey::Argument(PrimitiveTypes::INT_TYPE, "intargument");
  std::vector<const wisey::Argument*> arguments;
  arguments.push_back(intArgument);
  vector<const Model*> thrownExceptions;
  StaticMethod staticMethod(mModel,
                            "foo",
                            AccessLevel::PUBLIC_ACCESS,
                            PrimitiveTypes::FLOAT_TYPE,
                            arguments,
                            thrownExceptions,
                            new MethodQualifiers(0),
                            &mCompoundStatement,
                            0);

  vector<Type*> argumentTypes;
  Interface* threadInterface = mContext.getInterface(Names::getThreadInterfaceFullName(), 0);
  argumentTypes.push_back(threadInterface->getLLVMType(mContext));
  Controller* callStack = mContext.getController(Names::getCallStackControllerFullName(), 0);
  argumentTypes.push_back(callStack->getLLVMType(mContext));
  argumentTypes.push_back(PrimitiveTypes::INT_TYPE->getLLVMType(mContext));
  Type* llvmReturnType = PrimitiveTypes::FLOAT_TYPE->getLLVMType(mContext);
  FunctionType* expectedType = FunctionType::get(llvmReturnType, argumentTypes, false);
  
  FunctionType* actualType = staticMethod.getLLVMType(mContext);
  
  EXPECT_EQ(expectedType, actualType);
}

TEST_F(StaticMethodTest, definePublicFunctionTest) {
  wisey::Argument* intArgument = new wisey::Argument(PrimitiveTypes::INT_TYPE, "intargument");
  std::vector<const wisey::Argument*> arguments;
  arguments.push_back(intArgument);
  vector<const Model*> thrownExceptions;
  StaticMethod staticMethod(mModel,
                            "foo",
                            AccessLevel::PUBLIC_ACCESS,
                            PrimitiveTypes::FLOAT_TYPE,
                            arguments,
                            thrownExceptions,
                            new MethodQualifiers(0),
                            &mCompoundStatement,
                            0);
  Function* function = staticMethod.defineFunction(mContext);
  
  *mStringStream << *function;
  string expected = "\ndeclare float @systems.vos.wisey.compiler.tests.MObject.foo(%wisey.lang.threads.IThread*, %wisey.lang.CCallStack*, i32)\n";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  EXPECT_EQ(mContext.getMainFunction(), nullptr);
}

TEST_F(StaticMethodTest, definePrivateFunctionTest) {
  wisey::Argument* intArgument = new wisey::Argument(PrimitiveTypes::INT_TYPE, "intargument");
  std::vector<const wisey::Argument*> arguments;
  arguments.push_back(intArgument);
  vector<const Model*> thrownExceptions;
  StaticMethod staticMethod(mModel,
                            "foo",
                            AccessLevel::PRIVATE_ACCESS,
                            PrimitiveTypes::FLOAT_TYPE,
                            arguments,
                            thrownExceptions,
                            new MethodQualifiers(0),
                            &mCompoundStatement,
                            0);
  Function* function = staticMethod.defineFunction(mContext);
  
  *mStringStream << *function;
  string expected = "\ndeclare internal float @systems.vos.wisey.compiler.tests.MObject.foo(%wisey.lang.threads.IThread*, %wisey.lang.CCallStack*, i32)\n";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  EXPECT_EQ(mContext.getMainFunction(), nullptr);
}

TEST_F(StaticMethodTest, generateIRTest) {
  wisey::Argument* intArgument = new wisey::Argument(PrimitiveTypes::INT_TYPE, "intargument");
  std::vector<const wisey::Argument*> arguments;
  arguments.push_back(intArgument);
  vector<const Model*> thrownExceptions;
  StaticMethod staticMethod(mModel,
                            "foo",
                            AccessLevel::PUBLIC_ACCESS,
                            PrimitiveTypes::VOID_TYPE,
                            arguments,
                            thrownExceptions,
                            new MethodQualifiers(0),
                            &mCompoundStatement,
                            0);
  Function* function = staticMethod.defineFunction(mContext);
  staticMethod.generateIR(mContext);
  
  *mStringStream << *function;
  string expected =
  "\ndefine void @systems.vos.wisey.compiler.tests.MObject.foo(%wisey.lang.threads.IThread* %thread, %wisey.lang.CCallStack* %callstack, i32 %intargument) {"
  "\nentry:"
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

TEST_F(StaticMethodTest, isOwnerTest) {
  EXPECT_FALSE(mStaticMethod->isOwner());
}

TEST_F(StaticMethodTest, isReferenceTest) {
  EXPECT_FALSE(mStaticMethod->isReference());
}

TEST_F(StaticMethodTest, isPrimitiveTest) {
  EXPECT_FALSE(mStaticMethod->isPrimitive());
}

TEST_F(StaticMethodTest, isArrayTest) {
  EXPECT_FALSE(mStaticMethod->isArray());
}

TEST_F(StaticMethodTest, isFunctionTest) {
  EXPECT_TRUE(mStaticMethod->isFunction());
}

TEST_F(StaticMethodTest, isPackageTest) {
  EXPECT_FALSE(mStaticMethod->isPackage());
}

TEST_F(TestFileRunner, staticMethodDefinitionRunTest) {
  runFile("tests/samples/test_static_method_definition.yz", "2018");
}

TEST_F(TestFileRunner, staticMethodMissingThrowsDeathRunTest) {
  expectFailCompile("tests/samples/test_static_method_missing_throws.yz",
                    1,
                    "Error: Static method doThrow neither handles the exception "
                    "systems.vos.wisey.compiler.tests.MException nor throws it");
}

TEST_F(TestFileRunner, staticMethodReturnTypeIsNotVoidDeathRunTest) {
  expectFailCompile("tests/samples/test_static_method_return_type_is_not_void.yz",
                    1,
                    "Error: Static method foo must return a value of type int");
}

