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
#include "TestFileSampleRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/EmptyStatement.hpp"
#include "wisey/FinallyBlock.hpp"
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
#include "wisey/ProgramPrefix.hpp"
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
  StructType* mStructType;
  Controller* mThreadController;
  
public:
  
  MethodCallTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mExpression(new NiceMock<MockExpression>()),
  mIntType(Type::getInt32Ty(mContext.getLLVMContext())) {
    TestPrefix::run(mContext);
    
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);

    mContext.setPackage("systems.vos.wisey.compiler.tests");
    vector<Type*> types;
    types.push_back(Type::getInt64Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MSquare";
    mStructType = StructType::create(mLLVMContext, modelFullName);
    mStructType->setBody(types);
    vector<Field*> fields;
    ExpressionList fieldArguments;
    fields.push_back(new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "width", fieldArguments));
    fields.push_back(new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "height", fieldArguments));
    MethodArgument* methodArgument = new MethodArgument(PrimitiveTypes::FLOAT_TYPE, "argument");
    vector<MethodArgument*> methodArguments;
    methodArguments.push_back(methodArgument);
    vector<const Model*> fooThrownExceptions;
    IMethod* fooMethod = new Method("foo",
                                 AccessLevel::PUBLIC_ACCESS,
                                 PrimitiveTypes::INT_TYPE,
                                 methodArguments,
                                 fooThrownExceptions,
                                 NULL);
    vector<IMethod*> methods;
    methods.push_back(fooMethod);
    vector<const Model*> barThrownExceptions;
    StructType* exceptionModelStructType = StructType::create(mLLVMContext, "MException");
    Model* exceptionModel = Model::newModel("MException", exceptionModelStructType);
    barThrownExceptions.push_back(exceptionModel);
    IMethod* barMethod = new Method("bar",
                                    AccessLevel::PUBLIC_ACCESS,
                                    PrimitiveTypes::INT_TYPE,
                                    methodArguments,
                                    barThrownExceptions,
                                    NULL);
    methods.push_back(barMethod);
    mModel = Model::newModel(modelFullName, mStructType);
    mModel->setFields(fields, 1u);
    mModel->setMethods(methods);

    FunctionType* functionType = FunctionType::get(Type::getInt64Ty(mLLVMContext), false);
    Function* mainFunction = Function::Create(functionType,
                                              GlobalValue::InternalLinkage,
                                              "main",
                                              mContext.getModule());
    
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mainFunction);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    mContext.setMainFunction(mainFunction);

    BasicBlock* basicBlock = BasicBlock::Create(mLLVMContext);
    vector<Catch*> catchList;
    FinallyBlock* emptyBlock = new FinallyBlock();
    TryCatchInfo* tryCatchInfo = new TryCatchInfo(basicBlock, basicBlock, emptyBlock, catchList);
    mContext.getScopes().setTryCatchInfo(tryCatchInfo);

    mStringStream = new raw_string_ostream(mStringBuffer);
    
    Value* nullPointer = ConstantPointerNull::get(Type::getInt32PtrTy(mLLVMContext));
    Value* bitcast = IRWriter::newBitCastInst(mContext, nullPointer, mStructType->getPointerTo());
    ON_CALL(*mExpression, generateIR(_)).WillByDefault(Return(bitcast));
    ON_CALL(*mExpression, getType(_)).WillByDefault(Return(mModel));
    ON_CALL(*mExpression, printToStream(_, _)).WillByDefault(Invoke(printExpression));
    
    mThreadController = mContext.getController(Names::getThreadControllerFullName());
    Value* threadStore = IRWriter::newAllocaInst(mContext,
                                                 mThreadController->getLLVMType(mLLVMContext),
                                                 "threadStore");
    IVariable* threadVariable = new LocalReferenceVariable(ThreadExpression::THREAD,
                                                          mThreadController,
                                                          threadStore);
    threadVariable->setToNull(mContext);
    mContext.getScopes().setVariable(threadVariable);

    string objectName = mModel->getObjectNameGlobalVariableName();
    llvm::Constant* stringConstant = ConstantDataArray::getString(mLLVMContext, mModel->getName());
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
    stream << "object";
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
  vector<InterfaceTypeSpecifier*> parentInterfaces;
  vector<IObjectElementDeclaration*> interfaceElements;
  Interface* interface = Interface::newInterface(interfaceFullName,
                                                 structType,
                                                 parentInterfaces,
                                                 interfaceElements);
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
  argumentTypes.push_back(mThreadController->getLLVMType(mLLVMContext));
  argumentTypes.push_back(PrimitiveTypes::FLOAT_TYPE->getLLVMType(mLLVMContext));
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext),
                                                 argTypesArray,
                                                 false);
  Function::Create(functionType,
                   GlobalValue::InternalLinkage,
                   "systems.vos.wisey.compiler.tests.MSquare.foo",
                   mContext.getModule());
  
  NiceMock<MockExpression>* argumentExpression = new NiceMock<MockExpression>();
  Value* value = ConstantFP::get(Type::getFloatTy(mContext.getLLVMContext()), 5.2);
  ON_CALL(*argumentExpression, generateIR(_)).WillByDefault(Return(value));
  ON_CALL(*argumentExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));
  mArgumentList.push_back(argumentExpression);
  MethodCall methodCall(mExpression, "foo", mArgumentList, 0);
  
  Value* irValue = methodCall.generateIR(mContext);

  *mStringStream << *irValue;
  EXPECT_STREQ("  %3 = call i32 @systems.vos.wisey.compiler.tests.MSquare.foo("
               "%systems.vos.wisey.compiler.tests.MSquare* %0, "
               "%wisey.lang.CThread* %1, "
               "float 0x4014CCCCC0000000)",
               mStringStream->str().c_str());
  EXPECT_EQ(methodCall.getType(mContext), PrimitiveTypes::INT_TYPE);
}

TEST_F(MethodCallTest, modelMethodInvokeTest) {
  vector<Type*> argumentTypes;
  argumentTypes.push_back(mStructType->getPointerTo());
  argumentTypes.push_back(mThreadController->getLLVMType(mLLVMContext));
  argumentTypes.push_back(PrimitiveTypes::FLOAT_TYPE->getLLVMType(mLLVMContext));
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext),
                                                 argTypesArray,
                                                 false);
  Function::Create(functionType,
                   GlobalValue::InternalLinkage,
                   "systems.vos.wisey.compiler.tests.MSquare.bar",
                   mContext.getModule());
  
  NiceMock<MockExpression>* argumentExpression = new NiceMock<MockExpression>();
  Value* value = ConstantFP::get(Type::getFloatTy(mContext.getLLVMContext()), 5.2);
  ON_CALL(*argumentExpression, generateIR(_)).WillByDefault(Return(value));
  ON_CALL(*argumentExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));
  mArgumentList.push_back(argumentExpression);
  MethodCall methodCall(mExpression, "bar", mArgumentList, 0);
  BasicBlock* landingPadBlock = BasicBlock::Create(mLLVMContext, "eh.landing.pad");
  BasicBlock* continueBlock = BasicBlock::Create(mLLVMContext, "eh.continue");
  vector<Catch*> catchList;
  FinallyBlock* emptyBlock = new FinallyBlock();
  TryCatchInfo* tryCatchInfo = new TryCatchInfo(landingPadBlock,
                                                continueBlock,
                                                emptyBlock,
                                                catchList);
  mContext.getScopes().setTryCatchInfo(tryCatchInfo);

  Value* irValue = methodCall.generateIR(mContext);
  
  *mStringStream << *irValue;
  EXPECT_STREQ("  %3 = invoke i32 @systems.vos.wisey.compiler.tests.MSquare.bar("
               "%systems.vos.wisey.compiler.tests.MSquare* %0, "
               "%wisey.lang.CThread* %1, "
               "float 0x4014CCCCC0000000)\n"
               "          to label %invoke.continue1 unwind label %eh.landing.pad",
               mStringStream->str().c_str());
  EXPECT_EQ(methodCall.getType(mContext), PrimitiveTypes::INT_TYPE);
}

TEST_F(MethodCallTest, getVariableTest) {
  MethodCall methodCall(mExpression, "foo", mArgumentList, 0);
  
  EXPECT_EQ(methodCall.getVariable(mContext), nullptr);
}

TEST_F(MethodCallTest, existsInOuterScopeTest) {
  MethodCall methodCall(mExpression, "foo", mArgumentList, 0);
  
  ON_CALL(*mExpression, existsInOuterScope(_)).WillByDefault(Return(false));
  EXPECT_FALSE(methodCall.existsInOuterScope(mContext));
  
  ON_CALL(*mExpression, existsInOuterScope(_)).WillByDefault(Return(true));
  EXPECT_TRUE(methodCall.existsInOuterScope(mContext));
}

TEST_F(MethodCallTest, isConstantTest) {
  MethodCall methodCall(mExpression, "foo", mArgumentList, 0);

  EXPECT_FALSE(methodCall.isConstant());
}

TEST_F(MethodCallTest, printToStreamTest) {
  NiceMock<MockExpression>* argument1Expression = new NiceMock<MockExpression>();
  ON_CALL(*argument1Expression, printToStream(_, _)).WillByDefault(Invoke(printArgument1));
  mArgumentList.push_back(argument1Expression);
  NiceMock<MockExpression>* argument2Expression = new NiceMock<MockExpression>();
  ON_CALL(*argument2Expression, printToStream(_, _)).WillByDefault(Invoke(printArgument2));
  mArgumentList.push_back(argument2Expression);

  MethodCall methodCall(mExpression, "foo", mArgumentList, 0);

  stringstream stringStream;
  methodCall.printToStream(mContext, stringStream);
  
  EXPECT_STREQ("object.foo(argument1, argument2)", stringStream.str().c_str());
}

TEST_F(MethodCallTest, methodDoesNotExistDeathTest) {
  MethodCall methodCall(mExpression, "lorem", mArgumentList, 0);
  Mock::AllowLeak(mExpression);
  
  EXPECT_EXIT(methodCall.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Method 'lorem' is not found in object "
              "'systems.vos.wisey.compiler.tests.MSquare'");
}

TEST_F(MethodCallTest, methodCallOnPrimitiveTypeDeathTest) {
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::BOOLEAN_TYPE));
  MethodCall methodCall(mExpression, "foo", mArgumentList, 0);
  Mock::AllowLeak(mExpression);
  
  EXPECT_EXIT(methodCall.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Attempt to call a method 'foo' on a primitive type expression");
}

TEST_F(MethodCallTest, incorrectNumberOfArgumentsDeathTest) {
  MethodCall methodCall(mExpression, "foo", mArgumentList, 0);
  Mock::AllowLeak(mExpression);
  
  EXPECT_EXIT(methodCall.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Number of arguments for method call 'foo' of the object type "
              "'systems.vos.wisey.compiler.tests.MSquare' is not correct");
}

TEST_F(MethodCallTest, llvmImplementationNotFoundDeathTest) {
  NiceMock<MockExpression>* argumentExpression = new NiceMock<MockExpression>();
  ON_CALL(*argumentExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));
  mArgumentList.push_back(argumentExpression);
  MethodCall methodCall(mExpression, "bar", mArgumentList, 0);
  Mock::AllowLeak(mExpression);
  Mock::AllowLeak(argumentExpression);
  
  EXPECT_EXIT(methodCall.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: LLVM function implementing object 'systems.vos.wisey.compiler.tests.MSquare' "
              "method 'bar' was not found");
}

TEST_F(MethodCallTest, incorrectArgumentTypesDeathTest) {
  vector<Type*> argumentTypes;
  argumentTypes.push_back(mStructType->getPointerTo());
  argumentTypes.push_back(PrimitiveTypes::FLOAT_TYPE->getLLVMType(mLLVMContext));
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext),
                                                 argTypesArray,
                                                 false);
  Function::Create(functionType,
                   GlobalValue::InternalLinkage,
                   "systems.vos.wisey.compiler.tests.MSquare.foo",
                   mContext.getModule());
  
  NiceMock<MockExpression>* argumentExpression = new NiceMock<MockExpression>();
  ON_CALL(*argumentExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::LONG_TYPE));
  mArgumentList.push_back(argumentExpression);
  MethodCall methodCall(mExpression, "foo", mArgumentList, 0);
  Mock::AllowLeak(mExpression);
  Mock::AllowLeak(argumentExpression);
  
  EXPECT_EXIT(methodCall.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Call argument types do not match for a call to method 'foo' "
              "of the object type 'systems.vos.wisey.compiler.tests.MSquare");
}

TEST_F(TestFileSampleRunner, modelMethodCallRunTest) {
  runFile("tests/samples/test_model_method_call.yz", "5");
}

TEST_F(TestFileSampleRunner, modelMethodCallMultipleArgumentsRunTest) {
  runFile("tests/samples/test_model_method_call_multiple_parameters.yz", "8");
}

TEST_F(TestFileSampleRunner, modelMethodCallToSubModelRunTest) {
  runFile("tests/samples/test_model_method_call_to_submodel.yz", "7");
}

TEST_F(TestFileSampleRunner, modelMethodCallAutoCastArgumentRunTest) {
  runFile("tests/samples/test_method_argument_autocast.yz", "1");
}

TEST_F(TestFileSampleRunner, modelMethodCallInExpressionRunTest) {
  runFile("tests/samples/test_method_call_in_expression.yz", "30");
}

TEST_F(TestFileSampleRunner, modelMethodCallInExpressionWrappedIdentifierRunTest) {
  runFile("tests/samples/test_model_method_call_wrapped_identifier.yz", "5");
}

TEST_F(TestFileSampleRunner, methodCallToMethodCallRunTest) {
  runFile("tests/samples/test_method_call_to_method_call.yz", "10");
}

TEST_F(TestFileSampleRunner, methodCallToPrivateMethodViaPublicMethodRunTest) {
  runFile("tests/samples/test_call_to_private_method_via_public_method.yz", "20");
}

TEST_F(TestFileSampleRunner, returnOwnerAndAssignToReferenceRunTest) {
  runFile("tests/samples/test_method_call_return_owner_and_assign_to_reference.yz", "3");
}

TEST_F(TestFileSampleRunner, passOwnerAsParameterToMethodRunTest) {
  runFile("tests/samples/test_pass_owner_as_parameter_to_method.yz", "3");
}

TEST_F(TestFileSampleRunner, callToPrivateStaticMethodFromAnotherMethodRunTest) {
  runFile("tests/samples/test_call_to_private_static_method_from_another_method.yz", "10");
}

TEST_F(TestFileSampleRunner, callToPrivateMethodOnExpressionRunTest) {
  runFile("tests/samples/test_call_to_private_method_on_expression.yz", "2017");
}

TEST_F(TestFileSampleRunner, methodCallToPrivateMethodRunDeathTest) {
  expectFailCompile("tests/samples/test_private_method_call.yz",
                    1,
                    "Error: Method 'getDouble\\(\\)' of object "
                    "'systems.vos.wisey.compiler.tests.CService' is private");
}

TEST_F(TestFileSampleRunner, methodExceptionNotHandledDeathTest) {
  expectFailCompile("tests/samples/test_method_exception_not_handled.yz",
                    1,
                    "Error: Method doSomething neither handles the exception "
                    "systems.vos.wisey.compiler.tests.MException nor throws it");
}

TEST_F(TestFileSampleRunner, methodIdentifierChainDeathTest) {
  expectFailCompile("tests/samples/test_identifier_chain.yz",
                    1,
                    "Error: Incorrect method call format");
}

TEST_F(TestFileSampleRunner, passOwnerAsParameterToMethodAndThenUseItRunDeathTest) {
  expectFailCompile("tests/samples/test_pass_owner_as_parameter_to_method_and_then_use_it.yz",
                    1,
                    "Error: Variable 'data' was previously cleared and can not be used");
}
