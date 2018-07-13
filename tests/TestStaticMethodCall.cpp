//
//  TestStaticMethodCall.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link StaticMethodCall}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "MockType.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/Argument.hpp"
#include "wisey/EmptyStatement.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/Interface.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalReferenceVariable.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/StaticMethod.hpp"
#include "wisey/StaticMethodCall.hpp"
#include "wisey/ThreadExpression.hpp"
#include "wisey/ReceivedField.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct StaticMethodCallTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  ExpressionList mArgumentList;
  Type* mIntType;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Model* mModel;
  Model* mReturnedModel;
  ModelTypeSpecifier* mModelSpecifier;
  StructType* mStructType;
  Interface* mThreadInterface;
  Controller* mCallStack;
  string mPackage = "systems.vos.wisey.compiler.tests";
  ImportProfile* mImportProfile;

public:
  
  StaticMethodCallTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mIntType(Type::getInt32Ty(mContext.getLLVMContext())) {
    TestPrefix::generateIR(mContext);
    
    vector<Type*> returnedModelTypes;
    string returnedModelFullName = "systems.vos.wisey.compiler.tests.MReturnedModel";
    StructType* returnedModelStructType = StructType::create(mLLVMContext, returnedModelFullName);
    mReturnedModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                                     returnedModelFullName,
                                     returnedModelStructType,
                                     mContext.getImportProfile(),
                                     0);
    mContext.addModel(mReturnedModel, 0);

    vector<Type*> types;
    types.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                    ->getPointerTo()->getPointerTo());
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MSquare";
    mStructType = StructType::create(mLLVMContext, modelFullName);
    mStructType->setBody(types);
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                             modelFullName,
                             mStructType,
                             mContext.getImportProfile(),
                             0);
    vector<IField*> fields;
    fields.push_back(new ReceivedField(PrimitiveTypes::INT, "width", 0));
    fields.push_back(new ReceivedField(PrimitiveTypes::INT, "height", 0));
    wisey::Argument* fooArgument = new wisey::Argument(PrimitiveTypes::FLOAT, "argument");
    vector<const wisey::Argument*> fooArguments;
    fooArguments.push_back(fooArgument);
    vector<const Model*> fooThrownExceptions;
    IMethod* fooMethod = new StaticMethod(mModel,
                                          "foo",
                                          AccessLevel::PUBLIC_ACCESS,
                                          mReturnedModel,
                                          fooArguments,
                                          fooThrownExceptions,
                                          new MethodQualifiers(0),
                                          NULL,
                                          0);
    vector<IMethod*> methods;
    methods.push_back(fooMethod);
    vector<const Model*> barThrownExceptions;
    StructType* exceptionModelStructType = StructType::create(mLLVMContext, "MException");
    wisey::Argument* barArgument = new wisey::Argument(PrimitiveTypes::FLOAT, "argument");
    vector<const wisey::Argument*> barArguments;
    barArguments.push_back(barArgument);
    Model* exceptionModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                                            "MException",
                                            exceptionModelStructType,
                                            mContext.getImportProfile(),
                                            0);
    barThrownExceptions.push_back(exceptionModel);
    IMethod* barMethod = new StaticMethod(mModel,
                                          "bar",
                                          AccessLevel::PUBLIC_ACCESS,
                                          PrimitiveTypes::INT,
                                          barArguments,
                                          barThrownExceptions,
                                          new MethodQualifiers(0),
                                          NULL,
                                          0);
    methods.push_back(barMethod);
    mModel->setFields(mContext, fields, 1u);
    mModel->setMethods(methods);
    mContext.addModel(mModel, 0);
    mModelSpecifier = new ModelTypeSpecifier(NULL, "MSquare", 0);
    
    FunctionType* functionType = FunctionType::get(Type::getInt64Ty(mLLVMContext), false);
    Function* mainFunction = Function::Create(functionType,
                                              GlobalValue::InternalLinkage,
                                              "main",
                                              mContext.getModule());
    
    BasicBlock* declareBlock = BasicBlock::Create(mLLVMContext, "declare", mainFunction);
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mainFunction);
    mContext.setDeclarationsBlock(declareBlock);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mThreadInterface = mContext.getInterface(Names::getThreadInterfaceFullName(), 0);
    mCallStack = mContext.getController(Names::getCallStackControllerFullName(), 0);

    string objectName = mModel->getObjectNameGlobalVariableName();
    llvm::Constant* stringConstant = ConstantDataArray::getString(mLLVMContext,
                                                                  mModel->getTypeName());
    new GlobalVariable(*mContext.getModule(),
                       stringConstant->getType(),
                       true,
                       GlobalValue::InternalLinkage,
                       stringConstant,
                       objectName);
  
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~StaticMethodCallTest() {
    delete mStringStream;
  }

  static void printArgument1(IRGenerationContext& context, iostream& stream) {
    stream << "argument1";
  }
  
  static void printArgument2(IRGenerationContext& context, iostream& stream) {
    stream << "argument2";
  }
};

TEST_F(StaticMethodCallTest, modelStaticMethodCallTest) {
  vector<Type*> argumentTypes;
  argumentTypes.push_back(mThreadInterface->getLLVMType(mContext));
  argumentTypes.push_back(mCallStack->getLLVMType(mContext));
  argumentTypes.push_back(PrimitiveTypes::FLOAT->getLLVMType(mContext));
  FunctionType* functionType = FunctionType::get(mReturnedModel->getLLVMType(mContext),
                                                 argumentTypes,
                                                 false);
  Function::Create(functionType,
                   GlobalValue::InternalLinkage,
                   "systems.vos.wisey.compiler.tests.MSquare.foo",
                   mContext.getModule());
  
  NiceMock<MockExpression>* argumentExpression = new NiceMock<MockExpression>();
  Value* value = ConstantFP::get(Type::getFloatTy(mContext.getLLVMContext()), 5.2);
  ON_CALL(*argumentExpression, generateIR(_, _)).WillByDefault(Return(value));
  ON_CALL(*argumentExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT));
  mArgumentList.push_back(argumentExpression);
  StaticMethodCall* staticMethodCall = StaticMethodCall::create(mModelSpecifier,
                                                                "foo",
                                                                mArgumentList,
                                                                0);
  
  Value* irValue = staticMethodCall->generateIR(mContext, PrimitiveTypes::VOID);
  
  *mStringStream << *irValue;
  string expected =
  "  %call = invoke %systems.vos.wisey.compiler.tests.MReturnedModel* @systems.vos.wisey.compiler.tests.MSquare.foo(%wisey.threads.IThread* null, %wisey.threads.CCallStack* null, float 0x4014CCCCC0000000)"
  "\n          to label %invoke.continue unwind label %cleanup";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  EXPECT_EQ(staticMethodCall->getType(mContext), mReturnedModel);
  
  delete staticMethodCall;
}

TEST_F(StaticMethodCallTest, staticMethodCallCantThrowTest) {
  vector<Type*> argumentTypes;
  argumentTypes.push_back(mThreadInterface->getLLVMType(mContext));
  argumentTypes.push_back(mCallStack->getLLVMType(mContext));
  argumentTypes.push_back(PrimitiveTypes::FLOAT->getLLVMType(mContext));
  FunctionType* functionType = FunctionType::get(mReturnedModel->getLLVMType(mContext),
                                                 argumentTypes,
                                                 false);
  Function::Create(functionType,
                   GlobalValue::InternalLinkage,
                   "systems.vos.wisey.compiler.tests.MSquare.foo",
                   mContext.getModule());
  
  NiceMock<MockExpression>* argumentExpression = new NiceMock<MockExpression>();
  Value* value = ConstantFP::get(Type::getFloatTy(mContext.getLLVMContext()), 5.2);
  ON_CALL(*argumentExpression, generateIR(_, _)).WillByDefault(Return(value));
  ON_CALL(*argumentExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT));
  mArgumentList.push_back(argumentExpression);
  StaticMethodCall* staticMethodCall = StaticMethodCall::createCantThrow(mModelSpecifier,
                                                                         "foo",
                                                                         mArgumentList,
                                                                         0);

  Value* irValue = staticMethodCall->generateIR(mContext, PrimitiveTypes::VOID);
  
  *mStringStream << *irValue;
  string expected =
  "  %call = call %systems.vos.wisey.compiler.tests.MReturnedModel* @systems.vos.wisey.compiler.tests.MSquare.foo(%wisey.threads.IThread* null, %wisey.threads.CCallStack* null, float 0x4014CCCCC0000000)";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  EXPECT_EQ(staticMethodCall->getType(mContext), mReturnedModel);
  
  delete staticMethodCall;
}

TEST_F(StaticMethodCallTest, modelStaticMethodCallWithTryCatchTest) {
  vector<Type*> argumentTypes;
  argumentTypes.push_back(mThreadInterface->getLLVMType(mContext));
  argumentTypes.push_back(mCallStack->getLLVMType(mContext));
  argumentTypes.push_back(PrimitiveTypes::FLOAT->getLLVMType(mContext));
  FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext),
                                                 argumentTypes,
                                                 false);
  Function::Create(functionType,
                   GlobalValue::InternalLinkage,
                   "systems.vos.wisey.compiler.tests.MSquare.bar",
                   mContext.getModule());
  
  NiceMock<MockExpression>* argumentExpression = new NiceMock<MockExpression>();
  Value* value = ConstantFP::get(Type::getFloatTy(mContext.getLLVMContext()), 5.2);
  ON_CALL(*argumentExpression, generateIR(_, _)).WillByDefault(Return(value));
  ON_CALL(*argumentExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT));
  mArgumentList.push_back(argumentExpression);
  StaticMethodCall* staticMethodCall = StaticMethodCall::create(mModelSpecifier,
                                                                "bar",
                                                                mArgumentList,
                                                                0);
  BasicBlock* continueBlock = BasicBlock::Create(mLLVMContext, "eh.continue");
  vector<Catch*> catchList;
  TryCatchInfo* tryCatchInfo = new TryCatchInfo(catchList, continueBlock);
  mContext.getScopes().beginTryCatch(tryCatchInfo);
  
  Value* irValue = staticMethodCall->generateIR(mContext, PrimitiveTypes::VOID);
  
  *mStringStream << *irValue;
  EXPECT_STREQ("  %call = invoke i32 @systems.vos.wisey.compiler.tests.MSquare.bar(%wisey.threads.IThread* null, %wisey.threads.CCallStack* null, float 0x4014CCCCC0000000)\n"
               "          to label %invoke.continue unwind label %eh.landing.pad",
               mStringStream->str().c_str());
  EXPECT_EQ(staticMethodCall->getType(mContext), PrimitiveTypes::INT);
  
  delete staticMethodCall;
}

TEST_F(StaticMethodCallTest, isConstantTest) {
  StaticMethodCall* staticMethodCall = StaticMethodCall::create(mModelSpecifier,
                                                                "foo",
                                                                mArgumentList,
                                                                0);

  EXPECT_FALSE(staticMethodCall->isConstant());

  delete staticMethodCall;
}

TEST_F(StaticMethodCallTest, isAssignableTest) {
  StaticMethodCall* staticMethodCall = StaticMethodCall::create(mModelSpecifier,
                                                                "foo",
                                                                mArgumentList,
                                                                0);

  EXPECT_FALSE(staticMethodCall->isAssignable());
  
  delete staticMethodCall;
}

TEST_F(StaticMethodCallTest, printToStreamTest) {
  NiceMock<MockExpression>* argument1Expression = new NiceMock<MockExpression>();
  ON_CALL(*argument1Expression, printToStream(_, _)).WillByDefault(Invoke(printArgument1));
  mArgumentList.push_back(argument1Expression);
  NiceMock<MockExpression>* argument2Expression = new NiceMock<MockExpression>();
  ON_CALL(*argument2Expression, printToStream(_, _)).WillByDefault(Invoke(printArgument2));
  mArgumentList.push_back(argument2Expression);
  
  StaticMethodCall* staticMethodCall = StaticMethodCall::create(mModelSpecifier,
                                                                "foo",
                                                                mArgumentList,
                                                                0);

  stringstream stringStream;
  staticMethodCall->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("systems.vos.wisey.compiler.tests.MSquare.foo(argument1, argument2)",
               stringStream.str().c_str());
  
  delete staticMethodCall;
}

TEST_F(StaticMethodCallTest, methodDoesNotExistDeathTest) {
  StaticMethodCall* staticMethodCall = StaticMethodCall::create(mModelSpecifier,
                                                                "lorem",
                                                                mArgumentList,
                                                                1);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(staticMethodCall->generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(1): Error: Static method 'lorem' is not found in object systems.vos.wisey.compiler.tests.MSquare\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
  
  delete staticMethodCall;
}

TEST_F(StaticMethodCallTest, incorrectNumberOfArgumentsDeathTest) {
  StaticMethodCall* staticMethodCall = StaticMethodCall::create(mModelSpecifier,
                                                                "foo",
                                                                mArgumentList,
                                                                3);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(staticMethodCall->generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Number of arguments for static method call 'foo' of the object type systems.vos.wisey.compiler.tests.MSquare is not correct\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
  
  delete staticMethodCall;
}

TEST_F(StaticMethodCallTest, llvmImplementationNotFoundDeathTest) {
  NiceMock<MockExpression>* argumentExpression = new NiceMock<MockExpression>();
  ON_CALL(*argumentExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT));
  mArgumentList.push_back(argumentExpression);
  StaticMethodCall* staticMethodCall = StaticMethodCall::create(mModelSpecifier,
                                                                "bar",
                                                                mArgumentList,
                                                                5);
  Mock::AllowLeak(argumentExpression);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(staticMethodCall->generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(5): Error: LLVM function implementing object systems.vos.wisey.compiler.tests.MSquare method 'bar' was not found\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
  
  delete staticMethodCall;
}

TEST_F(StaticMethodCallTest, incorrectArgumentTypesDeathTest) {
  vector<Type*> argumentTypes;
  argumentTypes.push_back(mStructType->getPointerTo());
  argumentTypes.push_back(PrimitiveTypes::FLOAT->getLLVMType(mContext));
  FunctionType* functionType = FunctionType::get(mReturnedModel->getLLVMType(mContext),
                                                 argumentTypes,
                                                 false);
  Function::Create(functionType,
                   GlobalValue::InternalLinkage,
                   "systems.vos.wisey.compiler.tests.MSquare.foo",
                   mContext.getModule());
  
  NiceMock<MockExpression>* argumentExpression = new NiceMock<MockExpression>();
  ON_CALL(*argumentExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::LONG));
  mArgumentList.push_back(argumentExpression);
  StaticMethodCall* staticMethodCall = StaticMethodCall::create(mModelSpecifier,
                                                                "foo",
                                                                mArgumentList,
                                                                7);
  Mock::AllowLeak(argumentExpression);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(staticMethodCall->generateIR(mContext, PrimitiveTypes::VOID));
  EXPECT_STREQ("/tmp/source.yz(7): Error: Call argument types do not match for a call to method 'foo' of the object type systems.vos.wisey.compiler.tests.MSquare\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
  
  delete staticMethodCall;
}

TEST_F(TestFileRunner, modelStaticMethodCallRunTest) {
  runFile("tests/samples/test_model_static_method_call.yz", 2018);
}

TEST_F(TestFileRunner, nodeStaticMethodCallRunTest) {
  runFile("tests/samples/test_node_static_method_call.yz", 5);
}

TEST_F(TestFileRunner, controllerStaticMethodCallRunTest) {
  runFile("tests/samples/test_controller_static_method_call.yz", 7);
}

TEST_F(TestFileRunner, modelStaticMethodCallMultipleArgumentsRunTest) {
  runFile("tests/samples/test_model_static_method_call_multiple_parameters.yz", 6);
}

TEST_F(TestFileRunner, modelStaticMethodCallToSubModelRunTest) {
  runFile("tests/samples/test_model_static_method_call_to_submodel.yz", 3);
}

TEST_F(TestFileRunner, modelStaticMethodCallAutoCastArgumentRunTest) {
  runFile("tests/samples/test_static_method_argument_autocast.yz", 1);
}

TEST_F(TestFileRunner, modelStaticMethodCallInExpressionRunTest) {
  runFile("tests/samples/test_static_method_call_in_expression.yz", 8);
}

TEST_F(TestFileRunner, staticMethodCallToMethodCallRunTest) {
  runFile("tests/samples/test_static_method_call_to_method_call.yz", 10);
}

TEST_F(TestFileRunner, staticMethodCallToPrivateMethodViaPublicMethodRunTest) {
  runFile("tests/samples/test_call_to_private_static_method_via_public_static_method.yz", 10);
}

TEST_F(TestFileRunner, staticMethodReturnOwnerAndAssignToReferenceRunTest) {
  runFile("tests/samples/test_static_method_call_return_owner_and_assign_to_reference.yz", 3);
}

TEST_F(TestFileRunner, passOwnerAsParameterToStaticMethodRunTest) {
  runFile("tests/samples/test_pass_owner_as_parameter_to_static_method.yz", 3);
}

TEST_F(TestFileRunner, staticReturnArrayReferenceRunTest) {
  runFile("tests/samples/test_static_return_array_reference.yz", 5);
}

TEST_F(TestFileRunner, staticReturnArrayOwnerRunTest) {
  runFile("tests/samples/test_static_return_array_owner.yz", 5);
}

TEST_F(TestFileRunner, staticMethodCallToPrivateMethodRunDeathTest) {
  expectFailCompile("tests/samples/test_private_static_method_call.yz",
                    1,
                    "Error: Static method 'getDouble' of object "
                    "systems.vos.wisey.compiler.tests.CService is private");
}

TEST_F(TestFileRunner, staticMethodExceptionNotHandledDeathTest) {
  expectFailCompile("tests/samples/test_static_method_exception_not_handled.yz",
                    1,
                    "Error: Static method doSomething neither handles the exception "
                    "systems.vos.wisey.compiler.tests.MException nor throws it");
}

TEST_F(TestFileRunner, passOwnerAsParameterToStaticMethodAndThenUseItRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_pass_owner_as_parameter_to_static_method_and_then_use_it.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_pass_owner_as_parameter_to_static_method_and_then_use_it.yz:22)\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, staticReturnArrayReferenceRceRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_static_return_array_reference_rce.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.getArray(tests/samples/test_static_return_array_reference_rce.yz:8)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_static_return_array_reference_rce.yz:14)\n"
                               "Details: Object referenced by expression of type int[][]* still has 1 active reference\n"
                               "Main thread ended without a result\n");
}
