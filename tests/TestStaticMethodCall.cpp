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
#include "TestFileSampleRunner.hpp"
#include "wisey/EmptyStatement.hpp"
#include "wisey/FinallyBlock.hpp"
#include "wisey/Identifier.hpp"
#include "wisey/Interface.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/MethodArgument.hpp"
#include "wisey/ModelTypeSpecifier.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ProgramPrefix.hpp"
#include "wisey/StaticMethod.hpp"
#include "wisey/StaticMethodCall.hpp"

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
  ModelTypeSpecifier* mModelSpecifier;
  StructType* mStructType;
  
public:
  
  StaticMethodCallTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mIntType(Type::getInt32Ty(mContext.getLLVMContext())) {
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    
    mContext.setPackage("systems.vos.wisey.compiler.tests");
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MSquare";
    mStructType = StructType::create(mLLVMContext, modelFullName);
    mStructType->setBody(types);
    map<string, IField*> fields;
    ExpressionList fieldArguments;
    fields["width"] = new FieldFixed(PrimitiveTypes::INT_TYPE, "width", 0, fieldArguments);
    fields["height"] = new FieldFixed(PrimitiveTypes::INT_TYPE, "height", 1, fieldArguments);
    MethodArgument* fooMethodArgument = new MethodArgument(PrimitiveTypes::FLOAT_TYPE, "argument");
    vector<MethodArgument*> fooMethodArguments;
    fooMethodArguments.push_back(fooMethodArgument);
    vector<const Model*> fooThrownExceptions;
    IMethod* fooMethod = new StaticMethod("foo",
                                          AccessLevel::PUBLIC_ACCESS,
                                          PrimitiveTypes::INT_TYPE,
                                          fooMethodArguments,
                                          fooThrownExceptions,
                                          NULL);
    vector<IMethod*> methods;
    methods.push_back(fooMethod);
    vector<const Model*> barThrownExceptions;
    StructType* exceptionModelStructType = StructType::create(mLLVMContext, "MException");
    MethodArgument* barMethodArgument = new MethodArgument(PrimitiveTypes::FLOAT_TYPE, "argument");
    vector<MethodArgument*> barMethodArguments;
    barMethodArguments.push_back(barMethodArgument);
    Model* exceptionModel = new Model("MException", exceptionModelStructType);
    barThrownExceptions.push_back(exceptionModel);
    IMethod* barMethod = new StaticMethod("bar",
                                          AccessLevel::PUBLIC_ACCESS,
                                          PrimitiveTypes::INT_TYPE,
                                          barMethodArguments,
                                          barThrownExceptions,
                                          NULL);
    methods.push_back(barMethod);
    mModel = new Model(modelFullName, mStructType);
    mModel->setFields(fields);
    mModel->setMethods(methods);
    mContext.addModel(mModel);
    vector<string> package;
    mModelSpecifier = new ModelTypeSpecifier(package, "MSquare");
    
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
  }
  
  ~StaticMethodCallTest() {
    delete mStringStream;
  }

  static void printArgument1(iostream& stream) {
    stream << "argument1";
  }
  
  static void printArgument2(iostream& stream) {
    stream << "argument2";
  }
};

TEST_F(StaticMethodCallTest, modelStaticMethodCallTest) {
  vector<Type*> argumentTypes;
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
  StaticMethodCall staticMethodCall(mModelSpecifier, "foo", mArgumentList);
  
  Value* irValue = staticMethodCall.generateIR(mContext);
  
  *mStringStream << *irValue;
  EXPECT_STREQ("  %call = call i32 @systems.vos.wisey.compiler.tests.MSquare.foo("
               "float 0x4014CCCCC0000000)",
               mStringStream->str().c_str());
  EXPECT_EQ(staticMethodCall.getType(mContext), PrimitiveTypes::INT_TYPE);
}

TEST_F(StaticMethodCallTest, modelStaticMethodInvokeTest) {
  vector<Type*> argumentTypes;
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
  StaticMethodCall staticMethodCall(mModelSpecifier, "bar", mArgumentList);
  BasicBlock* landingPadBlock = BasicBlock::Create(mLLVMContext, "eh.landing.pad");
  BasicBlock* continueBlock = BasicBlock::Create(mLLVMContext, "eh.continue");
  vector<Catch*> catchList;
  FinallyBlock* emptyBlock = new FinallyBlock();
  TryCatchInfo* tryCatchInfo = new TryCatchInfo(landingPadBlock,
                                                continueBlock,
                                                emptyBlock,
                                                catchList);
  mContext.getScopes().setTryCatchInfo(tryCatchInfo);
  
  Value* irValue = staticMethodCall.generateIR(mContext);
  
  *mStringStream << *irValue;
  EXPECT_STREQ("  %call = invoke i32 @systems.vos.wisey.compiler.tests.MSquare.bar("
               "float 0x4014CCCCC0000000)\n"
               "          to label %invoke.continue unwind label %eh.landing.pad",
               mStringStream->str().c_str());
  EXPECT_EQ(staticMethodCall.getType(mContext), PrimitiveTypes::INT_TYPE);
}

TEST_F(StaticMethodCallTest, getVariableTest) {
  StaticMethodCall staticMethodCall(mModelSpecifier, "foo", mArgumentList);
  
  EXPECT_EQ(staticMethodCall.getVariable(mContext), nullptr);
}

TEST_F(StaticMethodCallTest, existsInOuterScopeTest) {
  StaticMethodCall staticMethodCall(mModelSpecifier, "foo", mArgumentList);
  
  EXPECT_TRUE(staticMethodCall.existsInOuterScope(mContext));
}

TEST_F(StaticMethodCallTest, printToStreamTest) {
  NiceMock<MockExpression>* argument1Expression = new NiceMock<MockExpression>();
  ON_CALL(*argument1Expression, printToStream(_)).WillByDefault(Invoke(printArgument1));
  mArgumentList.push_back(argument1Expression);
  NiceMock<MockExpression>* argument2Expression = new NiceMock<MockExpression>();
  ON_CALL(*argument2Expression, printToStream(_)).WillByDefault(Invoke(printArgument2));
  mArgumentList.push_back(argument2Expression);
  
  StaticMethodCall staticMethodCall(mModelSpecifier, "foo", mArgumentList);

  stringstream stringStream;
  staticMethodCall.printToStream(stringStream);
  
  EXPECT_STREQ("MSquare.foo(argument1, argument2)", stringStream.str().c_str());
}

TEST_F(StaticMethodCallTest, methodDoesNotExistDeathTest) {
  StaticMethodCall staticMethodCall(mModelSpecifier, "lorem", mArgumentList);
  
  EXPECT_EXIT(staticMethodCall.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Static method 'lorem' is not found in object "
              "'systems.vos.wisey.compiler.tests.MSquare'");
}

TEST_F(StaticMethodCallTest, incorrectNumberOfArgumentsDeathTest) {
  StaticMethodCall staticMethodCall(mModelSpecifier, "foo", mArgumentList);
  
  EXPECT_EXIT(staticMethodCall.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Number of arguments for static method call 'foo' of the object type "
              "'systems.vos.wisey.compiler.tests.MSquare' is not correct");
}

TEST_F(StaticMethodCallTest, llvmImplementationNotFoundDeathTest) {
  NiceMock<MockExpression>* argumentExpression = new NiceMock<MockExpression>();
  ON_CALL(*argumentExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));
  mArgumentList.push_back(argumentExpression);
  StaticMethodCall staticMethodCall(mModelSpecifier, "bar", mArgumentList);
  Mock::AllowLeak(argumentExpression);
  
  EXPECT_EXIT(staticMethodCall.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: LLVM function implementing object 'systems.vos.wisey.compiler.tests.MSquare' "
              "method 'bar' was not found");
}

TEST_F(StaticMethodCallTest, incorrectArgumentTypesDeathTest) {
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
  StaticMethodCall staticMethodCall(mModelSpecifier, "foo", mArgumentList);
  Mock::AllowLeak(argumentExpression);
  
  EXPECT_EXIT(staticMethodCall.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Call argument types do not match for a call to method 'foo' "
              "of the object type 'systems.vos.wisey.compiler.tests.MSquare");
}

TEST_F(TestFileSampleRunner, modelStaticMethodCallRunTest) {
  runFile("tests/samples/test_model_static_method_call.yz", "2018");
}

TEST_F(TestFileSampleRunner, nodeStaticMethodCallRunTest) {
  runFile("tests/samples/test_node_static_method_call.yz", "5");
}

TEST_F(TestFileSampleRunner, controllerStaticMethodCallRunTest) {
  runFile("tests/samples/test_controller_static_method_call.yz", "7");
}

TEST_F(TestFileSampleRunner, modelStaticMethodCallMultipleArgumentsRunTest) {
  runFile("tests/samples/test_model_static_method_call_multiple_parameters.yz", "6");
}

TEST_F(TestFileSampleRunner, modelStaticMethodCallToSubModelRunTest) {
  runFile("tests/samples/test_model_static_method_call_to_submodel.yz", "3");
}

TEST_F(TestFileSampleRunner, modelStaticMethodCallAutoCastArgumentRunTest) {
  runFile("tests/samples/test_static_method_argument_autocast.yz", "1");
}

TEST_F(TestFileSampleRunner, modelStaticMethodCallInExpressionRunTest) {
  runFile("tests/samples/test_static_method_call_in_expression.yz", "8");
}

TEST_F(TestFileSampleRunner, staticMethodCallToMethodCallRunTest) {
  runFile("tests/samples/test_static_method_call_to_method_call.yz", "10");
}

TEST_F(TestFileSampleRunner, staticMethodCallToPrivateMethodViaPublicMethodRunTest) {
  runFile("tests/samples/test_call_to_private_static_method_via_public_static_method.yz", "10");
}

TEST_F(TestFileSampleRunner, staticMethodReturnOwnerAndAssignToReferenceRunTest) {
  runFile("tests/samples/test_static_method_call_return_owner_and_assign_to_reference.yz", "3");
}

TEST_F(TestFileSampleRunner, passOwnerAsParameterToStaticMethodRunTest) {
  runFile("tests/samples/test_pass_owner_as_parameter_to_static_method.yz", "3");
}

TEST_F(TestFileSampleRunner, staticMethodCallToPrivateMethodRunDeathTest) {
  expectFailCompile("tests/samples/test_private_static_method_call.yz",
                    1,
                    "Error: Static method 'getDouble\\(\\)' of object "
                    "'systems.vos.wisey.compiler.tests.CService' is private");
}

TEST_F(TestFileSampleRunner, staticMethodExceptionNotHandledDeathTest) {
  expectFailCompile("tests/samples/test_static_method_exception_not_handled.yz",
                    1,
                    "Error: Static method doSomething neither handles the exception "
                    "systems.vos.wisey.compiler.tests.MException nor throws it");
}

TEST_F(TestFileSampleRunner, passOwnerAsParameterToStaticMethodAndThenUseItRunDeathTest) {
  expectFailCompile("tests/samples/test_pass_owner_as_parameter_to_static_method_and_then_use_it.yz",
                    1,
                    "Error: Variable 'data' was previously cleared and can not be used");
}