//
//  TestMethodCall.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link MethodCall}
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
#include "wisey/EmptyStatement.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/Interface.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalReferenceVariable.hpp"
#include "wisey/Method.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodCall.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ThreadExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct MethodCallTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockExpression>* mExpression;
  ExpressionList mArgumentList;
  Type* mIntType;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Model* mModel;
  Model* mReturnedModel;
  StructType* mStructType;
  Interface* mThreadInterface;
  Controller* mCallStack;
  Function* mMainFunction;
  Method* mFooMethod;
  Method* mBarMethod;
  
public:
  
  MethodCallTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mExpression(new NiceMock<MockExpression>()),
  mIntType(Type::getInt32Ty(mContext.getLLVMContext())) {
    TestPrefix::generateIR(mContext);
    
    vector<Type*> returnedModelTypes;
    string returnedModelFullName = "systems.vos.wisey.compiler.tests.MReturnedModel";
    StructType* returnedModelStructType = StructType::create(mLLVMContext, returnedModelFullName);
    mReturnedModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                                     returnedModelFullName,
                                     returnedModelStructType,
                                     0);
    mContext.addModel(mReturnedModel);
    
    vector<Type*> types;
    types.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                    ->getPointerTo()->getPointerTo());
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MSquare";
    mStructType = StructType::create(mLLVMContext, modelFullName);
    mStructType->setBody(types);
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS, modelFullName, mStructType, 0);
    vector<IField*> fields;
    fields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "width", 0));
    fields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "height", 0));
    MethodArgument* methodArgument = new MethodArgument(PrimitiveTypes::FLOAT_TYPE, "argument");
    vector<MethodArgument*> methodArguments;
    methodArguments.push_back(methodArgument);
    vector<const Model*> fooThrownExceptions;
    mFooMethod = new Method(mModel,
                            "foo",
                            AccessLevel::PUBLIC_ACCESS,
                            mReturnedModel,
                            methodArguments,
                            fooThrownExceptions,
                            new MethodQualifiers(0),
                            NULL,
                            0);
    vector<IMethod*> methods;
    methods.push_back(mFooMethod);
    vector<const Model*> barThrownExceptions;
    StructType* exceptionModelStructType = StructType::create(mLLVMContext, "MException");
    Model* exceptionModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                                            "MException",
                                            exceptionModelStructType,
                                            0);
    barThrownExceptions.push_back(exceptionModel);
    mBarMethod = new Method(mModel,
                            "bar",
                            AccessLevel::PUBLIC_ACCESS,
                            PrimitiveTypes::INT_TYPE,
                            methodArguments,
                            barThrownExceptions,
                            new MethodQualifiers(0),
                            NULL,
                            0);
    methods.push_back(mBarMethod);
    mModel->setFields(fields, 1u);
    mModel->setMethods(methods);

    FunctionType* functionType = FunctionType::get(Type::getInt64Ty(mLLVMContext), false);
    mMainFunction = Function::Create(functionType,
                                     GlobalValue::InternalLinkage,
                                     "main",
                                     mContext.getModule());

    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mMainFunction);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    mContext.setMainFunction(mMainFunction);

    mStringStream = new raw_string_ostream(mStringBuffer);
    
    Value* nullPointer = ConstantPointerNull::get(Type::getInt32PtrTy(mLLVMContext));
    Value* bitcast = IRWriter::newBitCastInst(mContext, nullPointer, mStructType->getPointerTo());
    ON_CALL(*mExpression, generateIR(_, _)).WillByDefault(Return(bitcast));
    ON_CALL(*mExpression, getType(_)).WillByDefault(Return(mFooMethod));
    ON_CALL(*mExpression, printToStream(_, _)).WillByDefault(Invoke(printExpression));
    
    mThreadInterface = mContext.getInterface(Names::getThreadInterfaceFullName(), 0);
    llvm::PointerType* llvmType = mThreadInterface->getLLVMType(mContext);
    Value* threadStore = IRWriter::newAllocaInst(mContext, llvmType, "threadStore");
    llvm::Constant* null = ConstantPointerNull::get(llvmType);
    IRWriter::newStoreInst(mContext, null, threadStore);
    IVariable* threadVariable = new LocalReferenceVariable(ThreadExpression::THREAD,
                                                           mThreadInterface,
                                                           threadStore);
    FakeExpression* fakeExpression = new FakeExpression(null, mThreadInterface);
    vector<const IExpression*> arrayIndices;
    threadVariable->generateAssignmentIR(mContext, fakeExpression, arrayIndices, 0);
    mContext.getScopes().setVariable(threadVariable);
    
    mCallStack = mContext.getController(Names::getCallStackControllerFullName(), 0);
    llvm::PointerType* callStackLLVMType = mCallStack->getLLVMType(mContext);
    Value* callStackStore = IRWriter::newAllocaInst(mContext, callStackLLVMType, "");
    null = ConstantPointerNull::get(callStackLLVMType);
    IRWriter::newStoreInst(mContext, null, callStackStore);
    IVariable* callStackVariable = new LocalReferenceVariable(ThreadExpression::CALL_STACK,
                                                              mCallStack,
                                                              callStackStore);
    fakeExpression = new FakeExpression(null, mCallStack);
    callStackVariable->generateAssignmentIR(mContext, fakeExpression, arrayIndices, 0);
    mContext.getScopes().setVariable(callStackVariable);

    string objectName = mModel->getObjectNameGlobalVariableName();
    llvm::Constant* stringConstant = ConstantDataArray::getString(mLLVMContext,
                                                                  mModel->getTypeName());
    new GlobalVariable(*mContext.getModule(),
                       stringConstant->getType(),
                       true,
                       GlobalValue::InternalLinkage,
                       stringConstant,
                       objectName);

    string functionName = MethodCall::getMethodNameConstantName("foo");
    stringConstant = ConstantDataArray::getString(mLLVMContext, "foo");
    new GlobalVariable(*mContext.getModule(),
                       stringConstant->getType(),
                       true,
                       GlobalValue::InternalLinkage,
                       stringConstant,
                       functionName);

    functionName = MethodCall::getMethodNameConstantName("bar");
    stringConstant = ConstantDataArray::getString(mLLVMContext, "bar");
    new GlobalVariable(*mContext.getModule(),
                       stringConstant->getType(),
                       true,
                       GlobalValue::InternalLinkage,
                       stringConstant,
                       functionName);
}
  
  ~MethodCallTest() {
    delete mStringStream;
  }
  
  static void printExpression(IRGenerationContext& context, iostream& stream) {
    stream << "object.foo";
  }

  static void printArgument1(IRGenerationContext& context, iostream& stream) {
    stream << "argument1";
  }

  static void printArgument2(IRGenerationContext& context, iostream& stream) {
    stream << "argument2";
  }
};

TEST_F(MethodCallTest, translateObjectMethodToLLVMFunctionNameTest) {
  string functionName = IMethodCall::translateObjectMethodToLLVMFunctionName(mModel, "foo");
  
  EXPECT_STREQ(functionName.c_str(), "systems.vos.wisey.compiler.tests.MSquare.foo");
  delete mExpression;
}

TEST_F(MethodCallTest, translateInterfaceMethodToLLVMFunctionNameTest) {
  string interfaceFullName = "systems.vos.wisey.compiler.tests.IShape";
  StructType* structType = StructType::create(mLLVMContext, interfaceFullName);
  vector<IInterfaceTypeSpecifier*> parentInterfaces;
  vector<IObjectElementDefinition*> interfaceElements;
  Interface* interface = Interface::newPublicInterface(interfaceFullName,
                                                       structType,
                                                       parentInterfaces,
                                                       interfaceElements,
                                                       0);
  string functionName =
    MethodCall::translateInterfaceMethodToLLVMFunctionName(mModel, interface, "foo");
  
  EXPECT_STREQ(functionName.c_str(),
               "systems.vos.wisey.compiler.tests.MSquare.interface."
               "systems.vos.wisey.compiler.tests.IShape.foo");
  delete mExpression;
}

TEST_F(MethodCallTest, modelMethodCallTest) {
  vector<Type*> argumentTypes;
  argumentTypes.push_back(mStructType->getPointerTo());
  argumentTypes.push_back(mThreadInterface->getLLVMType(mContext));
  argumentTypes.push_back(mCallStack->getLLVMType(mContext));
  argumentTypes.push_back(PrimitiveTypes::FLOAT_TYPE->getLLVMType(mContext));
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
  ON_CALL(*argumentExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));
  mArgumentList.push_back(argumentExpression);
  MethodCall methodCall(mExpression, mArgumentList, 0);
  
  Value* irValue = methodCall.generateIR(mContext, PrimitiveTypes::VOID_TYPE);

  *mStringStream << *irValue;
  string expected =
  "  %16 = invoke %systems.vos.wisey.compiler.tests.MReturnedModel* "
  "@systems.vos.wisey.compiler.tests.MSquare.foo(%systems.vos.wisey.compiler.tests.MSquare* %0, %wisey.lang.threads.IThread* %14, %wisey.lang.CCallStack* %15, float 0x4014CCCCC0000000)"
  "\n          to label %invoke.continue1 unwind label %cleanup";
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  EXPECT_EQ(methodCall.getType(mContext), mReturnedModel);
}

TEST_F(MethodCallTest, modelMethodCallWithTryCatchTest) {
  vector<Type*> argumentTypes;
  argumentTypes.push_back(mStructType->getPointerTo());
  argumentTypes.push_back(mThreadInterface->getLLVMType(mContext));
  argumentTypes.push_back(mCallStack->getLLVMType(mContext));
  argumentTypes.push_back(PrimitiveTypes::FLOAT_TYPE->getLLVMType(mContext));
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
  ON_CALL(*argumentExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(mBarMethod));
  mArgumentList.push_back(argumentExpression);
  MethodCall methodCall(mExpression, mArgumentList, 0);
  BasicBlock* continueBlock = BasicBlock::Create(mLLVMContext, "eh.continue");
  vector<Catch*> catchList;
  TryCatchInfo* tryCatchInfo = new TryCatchInfo(catchList, continueBlock);
  mContext.getScopes().beginTryCatch(tryCatchInfo);

  Value* irValue = methodCall.generateIR(mContext, PrimitiveTypes::VOID_TYPE);
  
  *mStringStream << *irValue;
  EXPECT_STREQ("  %11 = invoke i32 @systems.vos.wisey.compiler.tests.MSquare.bar(%systems.vos.wisey.compiler.tests.MSquare* %0, %wisey.lang.threads.IThread* %9, %wisey.lang.CCallStack* %10, float 0x4014CCCCC0000000)\n"
               "          to label %invoke.continue1 unwind label %eh.landing.pad",
               mStringStream->str().c_str());
  EXPECT_EQ(methodCall.getType(mContext), PrimitiveTypes::INT_TYPE);
}

TEST_F(MethodCallTest, getVariableTest) {
  MethodCall methodCall(mExpression, mArgumentList, 0);
  vector<const IExpression*> arrayIndices;

  EXPECT_EQ(methodCall.getVariable(mContext, arrayIndices), nullptr);
}

TEST_F(MethodCallTest, isConstantTest) {
  MethodCall methodCall(mExpression, mArgumentList, 0);

  EXPECT_FALSE(methodCall.isConstant());
}

TEST_F(MethodCallTest, printToStreamTest) {
  NiceMock<MockExpression>* argument1Expression = new NiceMock<MockExpression>();
  ON_CALL(*argument1Expression, printToStream(_, _)).WillByDefault(Invoke(printArgument1));
  mArgumentList.push_back(argument1Expression);
  NiceMock<MockExpression>* argument2Expression = new NiceMock<MockExpression>();
  ON_CALL(*argument2Expression, printToStream(_, _)).WillByDefault(Invoke(printArgument2));
  mArgumentList.push_back(argument2Expression);

  MethodCall methodCall(mExpression, mArgumentList, 0);

  stringstream stringStream;
  methodCall.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("object.foo(argument1, argument2)", stringStream.str().c_str());
}

TEST_F(MethodCallTest, incorrectNumberOfArgumentsDeathTest) {
  MethodCall methodCall(mExpression, mArgumentList, 0);
  Mock::AllowLeak(mExpression);
  
  EXPECT_EXIT(methodCall.generateIR(mContext, PrimitiveTypes::VOID_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Number of arguments for method call 'foo' of the object type "
              "'systems.vos.wisey.compiler.tests.MSquare' is not correct");
}

TEST_F(MethodCallTest, llvmImplementationNotFoundDeathTest) {
  NiceMock<MockExpression>* argumentExpression = new NiceMock<MockExpression>();
  ON_CALL(*argumentExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(mBarMethod));
  mArgumentList.push_back(argumentExpression);
  MethodCall methodCall(mExpression, mArgumentList, 0);
  Mock::AllowLeak(mExpression);
  Mock::AllowLeak(argumentExpression);
  
  EXPECT_EXIT(methodCall.generateIR(mContext, PrimitiveTypes::VOID_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: LLVM function implementing object 'systems.vos.wisey.compiler.tests.MSquare' "
              "method 'bar' was not found");
}

TEST_F(MethodCallTest, incorrectArgumentTypesDeathTest) {
  vector<Type*> argumentTypes;
  argumentTypes.push_back(mStructType->getPointerTo());
  argumentTypes.push_back(PrimitiveTypes::FLOAT_TYPE->getLLVMType(mContext));
  FunctionType* functionType = FunctionType::get(mReturnedModel->getLLVMType(mContext),
                                                 argumentTypes,
                                                 false);
  Function::Create(functionType,
                   GlobalValue::InternalLinkage,
                   "systems.vos.wisey.compiler.tests.MSquare.foo",
                   mContext.getModule());
  
  NiceMock<MockExpression>* argumentExpression = new NiceMock<MockExpression>();
  ON_CALL(*argumentExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::LONG_TYPE));
  mArgumentList.push_back(argumentExpression);
  MethodCall methodCall(mExpression, mArgumentList, 0);
  Mock::AllowLeak(mExpression);
  Mock::AllowLeak(argumentExpression);
  
  EXPECT_EXIT(methodCall.generateIR(mContext, PrimitiveTypes::VOID_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Call argument types do not match for a call to method 'foo' "
              "of the object type 'systems.vos.wisey.compiler.tests.MSquare");
}

TEST_F(TestFileRunner, modelMethodCallRunTest) {
  runFile("tests/samples/test_model_method_call.yz", "5");
}

TEST_F(TestFileRunner, modelMethodCallMultipleArgumentsRunTest) {
  runFile("tests/samples/test_model_method_call_multiple_parameters.yz", "8");
}

TEST_F(TestFileRunner, modelMethodCallToSubModelRunTest) {
  runFile("tests/samples/test_model_method_call_to_submodel.yz", "7");
}

TEST_F(TestFileRunner, modelMethodCallAutoCastArgumentRunTest) {
  runFile("tests/samples/test_method_argument_autocast.yz", "1");
}

TEST_F(TestFileRunner, modelMethodCallInExpressionRunTest) {
  runFile("tests/samples/test_method_call_in_expression.yz", "30");
}

TEST_F(TestFileRunner, modelMethodCallInExpressionWrappedIdentifierRunTest) {
  runFile("tests/samples/test_model_method_call_wrapped_identifier.yz", "5");
}

TEST_F(TestFileRunner, methodCallToMethodCallRunTest) {
  runFile("tests/samples/test_method_call_to_method_call.yz", "10");
}

TEST_F(TestFileRunner, methodCallToPrivateMethodViaPublicMethodRunTest) {
  runFile("tests/samples/test_call_to_private_method_via_public_method.yz", "20");
}

TEST_F(TestFileRunner, returnOwnerAndAssignToReferenceRunTest) {
  runFile("tests/samples/test_method_call_return_owner_and_assign_to_reference.yz", "3");
}

TEST_F(TestFileRunner, passOwnerAsParameterToMethodRunTest) {
  runFile("tests/samples/test_pass_owner_as_parameter_to_method.yz", "3");
}

TEST_F(TestFileRunner, callToPrivateStaticMethodFromAnotherMethodRunTest) {
  runFile("tests/samples/test_call_to_private_static_method_from_another_method.yz", "10");
}

TEST_F(TestFileRunner, callToPrivateMethodOnExpressionRunTest) {
  runFile("tests/samples/test_call_to_private_method_on_expression.yz", "2017");
}

TEST_F(TestFileRunner, returnArrayReferenceRunTest) {
  runFile("tests/samples/test_return_array_reference.yz", "5");
}

TEST_F(TestFileRunner, returnArrayOwnerRunTest) {
  runFile("tests/samples/test_return_array_owner.yz", "5");
}

TEST_F(TestFileRunner, methodCallToPrivateMethodRunDeathTest) {
  expectFailCompile("tests/samples/test_private_method_call.yz",
                    1,
                    "Error: Method 'getDouble' of object "
                    "systems.vos.wisey.compiler.tests.CService is private");
}

TEST_F(TestFileRunner, methodExceptionNotHandledDeathTest) {
  expectFailCompile("tests/samples/test_method_exception_not_handled.yz",
                    1,
                    "Error: Method doSomething neither handles the exception "
                    "systems.vos.wisey.compiler.tests.MException nor throws it");
}

TEST_F(TestFileRunner, methodIdentifierChainDeathTest) {
  expectFailCompile("tests/samples/test_identifier_chain.yz",
                    1,
                    "Error: Method 'vos' is not found in object "
                    "systems.vos.wisey.compiler.tests.MSystem");
}

TEST_F(TestFileRunner, passOwnerAsParameterToMethodAndThenUseItRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_pass_owner_as_parameter_to_method_and_then_use_it.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_pass_owner_as_parameter_to_method_and_then_use_it.yz:24)\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, returnArrayReferenceRceRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_return_array_reference_rce.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.getArray(tests/samples/test_return_array_reference_rce.yz:10)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_return_array_reference_rce.yz:14)\n"
                               "Details: Object referenced by expression still has 1 active reference\n"
                               "Main thread ended without a result\n");
}
