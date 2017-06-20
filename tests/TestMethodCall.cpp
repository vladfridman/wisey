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
#include "wisey/Identifier.hpp"
#include "wisey/Interface.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/MethodCall.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct MethodCallTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockExpression>* mExpression;
  NiceMock<MockType> mExceptionType;
  ExpressionList mArgumentList;
  Type* mIntType;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Model* mModel;
  Method* mMethod;
  StructType* mStructType;
  
public:
  
  MethodCallTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mExpression(new NiceMock<MockExpression>()),
  mIntType(Type::getInt32Ty(mContext.getLLVMContext())) {
    mContext.setPackage("systems.vos.wisey.compiler.tests");
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MSquare";
    mStructType = StructType::create(mLLVMContext, modelFullName);
    mStructType->setBody(types);
    map<string, Field*> fields;
    ExpressionList fieldArguments;
    fields["width"] = new Field(PrimitiveTypes::INT_TYPE, "width", 0, fieldArguments);
    fields["height"] = new Field(PrimitiveTypes::INT_TYPE, "height", 1, fieldArguments);
    MethodArgument* methodArgument = new MethodArgument(PrimitiveTypes::FLOAT_TYPE, "argument");
    vector<MethodArgument*> methodArguments;
    methodArguments.push_back(methodArgument);
    vector<const IType*> fooThrownExceptions;
    mMethod = new Method("foo",
                         AccessLevel::PUBLIC_ACCESS,
                         PrimitiveTypes::INT_TYPE,
                         methodArguments,
                         fooThrownExceptions,
                         NULL,
                         0);
    vector<Method*> methods;
    methods.push_back(mMethod);
    vector<const IType*> barThrownExceptions;
    barThrownExceptions.push_back(&mExceptionType);
    Method* barMethod = new Method("bar",
                                   AccessLevel::PUBLIC_ACCESS,
                                   PrimitiveTypes::INT_TYPE,
                                   methodArguments,
                                   barThrownExceptions,
                                   NULL,
                                   1);
    methods.push_back(barMethod);
    mModel = new Model(modelFullName, mStructType);
    mModel->setFields(fields);
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

    mStringStream = new raw_string_ostream(mStringBuffer);
    
    Value* nullPointer = ConstantPointerNull::get(Type::getInt32PtrTy(mLLVMContext));
    Value* bitcast = IRWriter::newBitCastInst(mContext, nullPointer, mStructType->getPointerTo());
    ON_CALL(*mExpression, generateIR(_)).WillByDefault(Return(bitcast));
    ON_CALL(*mExpression, getType(_)).WillByDefault(Return(mModel));
  }
  
  ~MethodCallTest() {
    delete mStringStream;
  }
};

TEST_F(MethodCallTest, translateObjectMethodToLLVMFunctionNameTest) {
  string functionName = MethodCall::translateObjectMethodToLLVMFunctionName(mModel, "foo");
  
  EXPECT_STREQ(functionName.c_str(), "systems.vos.wisey.compiler.tests.MSquare.foo");
  delete mExpression;
}

TEST_F(MethodCallTest, translateInterfaceMethodToLLVMFunctionNameTest) {
  string interfaceFullName = "systems.vos.wisey.compiler.tests.IShape";
  StructType* structType = StructType::create(mLLVMContext, interfaceFullName);
  Interface* interface = new Interface(interfaceFullName, structType);
  string functionName =
    MethodCall::translateInterfaceMethodToLLVMFunctionName(mModel, interface, "foo");
  
  EXPECT_STREQ(functionName.c_str(),
               "systems.vos.wisey.compiler.tests.MSquare.interface."
               "systems.vos.wisey.compiler.tests.IShape.foo");
  delete mExpression;
}

TEST_F(MethodCallTest, methodDoesNotExistDeathTest) {
  MethodCall methodCall(mExpression, "lorem", mArgumentList);
  Mock::AllowLeak(mExpression);
  
  EXPECT_EXIT(methodCall.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Method 'lorem' is not found in object "
              "'systems.vos.wisey.compiler.tests.MSquare'");
}

TEST_F(MethodCallTest, methodCallOnPrimitiveTypeDeathTest) {
  ON_CALL(*mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::BOOLEAN_TYPE));
  MethodCall methodCall(mExpression, "foo", mArgumentList);
  Mock::AllowLeak(mExpression);
  
  EXPECT_EXIT(methodCall.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Attempt to call a method 'foo' on a primitive type expression");
}

TEST_F(MethodCallTest, incorrectNumberOfArgumentsDeathTest) {
  MethodCall methodCall(mExpression, "foo", mArgumentList);
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
  MethodCall methodCall(mExpression, "bar", mArgumentList);
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
  MethodCall methodCall(mExpression, "foo", mArgumentList);
  Mock::AllowLeak(mExpression);
  Mock::AllowLeak(argumentExpression);

  EXPECT_EXIT(methodCall.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Call argument types do not match for a call to method 'foo' "
              "of the object type 'systems.vos.wisey.compiler.tests.MSquare");
}

TEST_F(MethodCallTest, modelMethodCallTest) {
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
  Value* value = ConstantFP::get(Type::getFloatTy(mContext.getLLVMContext()), 5.2);
  ON_CALL(*argumentExpression, generateIR(_)).WillByDefault(Return(value));
  ON_CALL(*argumentExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));
  mArgumentList.push_back(argumentExpression);
  MethodCall methodCall(mExpression, "foo", mArgumentList);
  
  Value* irValue = methodCall.generateIR(mContext);

  *mStringStream << *irValue;
  EXPECT_STREQ("  %call = call i32 @systems.vos.wisey.compiler.tests.MSquare.foo("
               "%systems.vos.wisey.compiler.tests.MSquare* %0, float 0x4014CCCCC0000000)",
               mStringStream->str().c_str());
  EXPECT_EQ(methodCall.getType(mContext), PrimitiveTypes::INT_TYPE);
}

TEST_F(MethodCallTest, modelMethodInvokeTest) {
  vector<Type*> argumentTypes;
  argumentTypes.push_back(mStructType->getPointerTo());
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
  MethodCall methodCall(mExpression, "bar", mArgumentList);
  mContext.getScopes().setLandingPadBlock(BasicBlock::Create(mLLVMContext, "eh.landing.pad"));
  mContext.getScopes().setExceptionContinueBlock(BasicBlock::Create(mLLVMContext, "eh.continue"));

  Value* irValue = methodCall.generateIR(mContext);
  
  *mStringStream << *irValue;
  EXPECT_STREQ("  %call = invoke i32 @systems.vos.wisey.compiler.tests.MSquare.bar("
               "%systems.vos.wisey.compiler.tests.MSquare* %0, float 0x4014CCCCC0000000)\n"
               "          to label %invoke.continue unwind label %eh.landing.pad",
               mStringStream->str().c_str());
  EXPECT_EQ(methodCall.getType(mContext), PrimitiveTypes::INT_TYPE);
}

TEST_F(MethodCallTest, existsInOuterScopeTest) {
  MethodCall methodCall(mExpression, "foo", mArgumentList);
  
  ON_CALL(*mExpression, existsInOuterScope(_)).WillByDefault(Return(false));
  EXPECT_FALSE(methodCall.existsInOuterScope(mContext));
  
  ON_CALL(*mExpression, existsInOuterScope(_)).WillByDefault(Return(true));
  EXPECT_TRUE(methodCall.existsInOuterScope(mContext));
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

