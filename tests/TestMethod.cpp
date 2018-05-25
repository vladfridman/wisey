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

#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/Argument.hpp"
#include "wisey/CompoundStatement.hpp"
#include "wisey/Method.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ReceivedField.hpp"

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

    wisey::Argument* doubleArgument = new wisey::Argument(PrimitiveTypes::DOUBLE, "argDouble");
    wisey::Argument* charArgument = new wisey::Argument(PrimitiveTypes::CHAR, "argChar");
    std::vector<const wisey::Argument*> arguments;
    arguments.push_back(doubleArgument);
    arguments.push_back(charArgument);
    vector<const Model*> thrownExceptions;
    string rceFullName = Names::getLangPackageName() + "." +
    Names::getReferenceCountExceptionName();
    thrownExceptions.push_back(mContext.getModel(rceFullName, 0));
    mMethod = new Method(mModel,
                         "mymethod",
                         AccessLevel::PUBLIC_ACCESS,
                         PrimitiveTypes::BOOLEAN,
                         arguments,
                         thrownExceptions,
                         new MethodQualifiers(0),
                         NULL,
                         0);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~MethodTest() {
    delete mStringStream;
  }
};

TEST_F(MethodTest, basicMethodTest) {
  EXPECT_STREQ("mymethod", mMethod->getName().c_str());
  EXPECT_EQ(PrimitiveTypes::BOOLEAN, mMethod->getReturnType());
  EXPECT_EQ(2u, mMethod->getArguments().size());
  EXPECT_FALSE(mMethod->isStatic());
  EXPECT_STREQ("systems.vos.wisey.compiler.tests.MObject.mymethod", mMethod->getTypeName().c_str());
}

TEST_F(MethodTest, elementTypeTest) {
  EXPECT_FALSE(mMethod->isConstant());
  EXPECT_FALSE(mMethod->isField());
  EXPECT_TRUE(mMethod->isMethod());
  EXPECT_FALSE(mMethod->isStaticMethod());
  EXPECT_FALSE(mMethod->isMethodSignature());
  EXPECT_FALSE(mMethod->isLLVMFunction());
}

TEST_F(MethodTest, getLLVMTypeTest) {
  wisey::Argument* intArgument = new wisey::Argument(PrimitiveTypes::INT, "intargument");
  std::vector<const wisey::Argument*> arguments;
  arguments.push_back(intArgument);
  vector<const Model*> thrownExceptions;
  Method method(mModel,
                "foo",
                AccessLevel::PUBLIC_ACCESS,
                PrimitiveTypes::FLOAT,
                arguments,
                thrownExceptions,
                new MethodQualifiers(0),
                &mCompoundStatement,
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

TEST_F(MethodTest, definePublicFunctionTest) {
  wisey::Argument* intArgument = new wisey::Argument(PrimitiveTypes::INT, "intargument");
  std::vector<const wisey::Argument*> arguments;
  arguments.push_back(intArgument);
  vector<const Model*> thrownExceptions;
  Method method(mModel,
                "foo",
                AccessLevel::PUBLIC_ACCESS,
                PrimitiveTypes::FLOAT,
                arguments,
                thrownExceptions,
                new MethodQualifiers(0),
                &mCompoundStatement,
                0);
  Function* function = method.declareFunction(mContext);
  
  *mStringStream << *function;
  string expected = "\ndeclare float @systems.vos.wisey.compiler.tests.MObject.foo(%systems.vos.wisey.compiler.tests.MObject*, %wisey.threads.IThread*, %wisey.threads.CCallStack*, i32)\n";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(MethodTest, definePrivateFunctionTest) {
  wisey::Argument* intArgument = new wisey::Argument(PrimitiveTypes::INT, "intargument");
  std::vector<const wisey::Argument*> arguments;
  arguments.push_back(intArgument);
  vector<const Model*> thrownExceptions;
  Method method(mModel,
                "foo",
                AccessLevel::PRIVATE_ACCESS,
                PrimitiveTypes::FLOAT,
                arguments,
                thrownExceptions,
                new MethodQualifiers(0),
                &mCompoundStatement,
                0);
  Function* function = method.declareFunction(mContext);
  
  *mStringStream << *function;
  string expected = "\ndeclare internal float @systems.vos.wisey.compiler.tests.MObject.foo(%systems.vos.wisey.compiler.tests.MObject*, %wisey.threads.IThread*, %wisey.threads.CCallStack*, i32)\n";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(MethodTest, generateIRTest) {
  wisey::Argument* intArgument = new wisey::Argument(PrimitiveTypes::INT, "intargument");
  std::vector<const wisey::Argument*> arguments;
  arguments.push_back(intArgument);
  vector<const Model*> thrownExceptions;
  Method method(mModel,
                "foo",
                AccessLevel::PUBLIC_ACCESS,
                PrimitiveTypes::VOID,
                arguments,
                thrownExceptions,
                new MethodQualifiers(0),
                &mCompoundStatement,
                0);
  Function* function = method.declareFunction(mContext);
  method.generateIR(mContext);
  
  *mStringStream << *function;
  string expected =
  "\ndefine void @systems.vos.wisey.compiler.tests.MObject.foo(%systems.vos.wisey.compiler.tests.MObject* %this, %wisey.threads.IThread* %thread, %wisey.threads.CCallStack* %callstack, i32 %intargument) {"
  "\nentry:"
  "\n  ret void"
  "\n}"
  "\n";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(MethodTest, isTypeKindTest) {
  EXPECT_FALSE(mMethod->isOwner());
  EXPECT_FALSE(mMethod->isReference());
  EXPECT_FALSE(mMethod->isPrimitive());
  EXPECT_FALSE(mMethod->isArray());
  EXPECT_TRUE(mMethod->isFunction());
  EXPECT_FALSE(mMethod->isPackage());
  EXPECT_FALSE(mMethod->isNative());
  EXPECT_FALSE(mMethod->isPointer());
  EXPECT_FALSE(mMethod->isImmutable());
}

TEST_F(MethodTest, printToStreamTest) {
  stringstream stringStream;
  mMethod->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("  boolean mymethod(\n"
               "    double argDouble,\n"
               "    char argChar) throws wisey.lang.MReferenceCountException;\n",
               stringStream.str().c_str());
}

TEST_F(TestFileRunner, freeOwnerReturnedFromMethodRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_free_owner_returned_from_method.yz",
                                        "5\n"
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "done destructing systems.vos.wisey.compiler.tests.MCar\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n"
                                        "done destructing systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}

TEST_F(TestFileRunner, methodMissingThrowsDeathRunTest) {
  expectFailCompile("tests/samples/test_method_missing_throws.yz",
                    1,
                    "Error: Method doThrow neither handles the exception "
                    "systems.vos.wisey.compiler.tests.MException nor throws it");
}

TEST_F(TestFileRunner, methodReturnTypeIsNotVoidDeathRunTest) {
  expectFailCompile("tests/samples/test_method_return_type_is_not_void.yz",
                    1,
                    "Error: Method run must return a value of type int");
}

