//
//  TestMethodCall.cpp
//  Yazyk
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
#include "TestFileSampleRunner.hpp"
#include "yazyk/Identifier.hpp"
#include "yazyk/Interface.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/MethodArgument.hpp"
#include "yazyk/MethodCall.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct MethodCallTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockExpression> mExpression;
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
  mIntType(Type::getInt32Ty(mContext.getLLVMContext())) {
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    mStructType = StructType::create(mLLVMContext, "MSquare");
    mStructType->setBody(types);
    map<string, Field*> fields;
    ExpressionList fieldArguments;
    fields["width"] = new Field(PrimitiveTypes::INT_TYPE, "width", 0, fieldArguments);
    fields["height"] = new Field(PrimitiveTypes::INT_TYPE, "height", 1, fieldArguments);
    MethodArgument* methodArgument = new MethodArgument(PrimitiveTypes::FLOAT_TYPE, "argument");
    vector<MethodArgument*> methodArguments;
    methodArguments.push_back(methodArgument);
    mMethod = new Method("foo",
                         AccessLevel::PUBLIC_ACCESS,
                         PrimitiveTypes::INT_TYPE,
                         methodArguments,
                         0,
                         NULL);
    vector<Method*> methods;
    methods.push_back(mMethod);
    Method* barMethod = new Method("bar",
                                   AccessLevel::PUBLIC_ACCESS,
                                   PrimitiveTypes::INT_TYPE,
                                   methodArguments,
                                   1,
                                   NULL);
    methods.push_back(barMethod);
    vector<Interface*> interfaces;
    mModel = new Model("MSquare", mStructType, fields, methods, interfaces);

    mBasicBlock = BasicBlock::Create(mContext.getLLVMContext(), "entry");
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    Value* nullPointer = ConstantPointerNull::get(Type::getInt32PtrTy(mLLVMContext));
    Value* bitcast = new BitCastInst(nullPointer, mStructType->getPointerTo(), "test", mBasicBlock);
    ON_CALL(mExpression, generateIR(_)).WillByDefault(Return(bitcast));
    ON_CALL(mExpression, getType(_)).WillByDefault(Return(mModel));
  }
  
  ~MethodCallTest() {
    delete mBasicBlock;
    delete mStringStream;
  }
};

TEST_F(MethodCallTest, translateObjectMethodToLLVMFunctionNameTest) {
  string functionName = MethodCall::translateObjectMethodToLLVMFunctionName(mModel, "foo");
  
  EXPECT_STREQ(functionName.c_str(), "object.MSquare.foo");
}

TEST_F(MethodCallTest, translateInterfaceMethodToLLVMFunctionNameTest) {
  StructType* structType = StructType::create(mLLVMContext, "IShape");
  vector<MethodSignature*> interfaceMethods;
  vector<Interface*> methodInterface;
  Interface* interface = new Interface("IShape", structType, methodInterface, interfaceMethods);
  string functionName =
    MethodCall::translateInterfaceMethodToLLVMFunctionName(mModel, interface, "foo");
  
  EXPECT_STREQ(functionName.c_str(), "object.MSquare.interface.IShape.foo");
}

TEST_F(MethodCallTest, methodDoesNotExistDeathTest) {
  MethodCall methodCall(mExpression, "lorem", mArgumentList);
  Mock::AllowLeak(&mExpression);
  
  EXPECT_EXIT(methodCall.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Method 'lorem' is not found in object 'MSquare'");
}

TEST_F(MethodCallTest, methodCallOnPrimitiveTypeDeathTest) {
  ON_CALL(mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::BOOLEAN_TYPE));
  MethodCall methodCall(mExpression, "foo", mArgumentList);
  Mock::AllowLeak(&mExpression);
  
  EXPECT_EXIT(methodCall.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Attempt to call a method 'foo' on a primitive type expression");
}

TEST_F(MethodCallTest, incorrectNumberOfArgumentsDeathTest) {
  MethodCall methodCall(mExpression, "foo", mArgumentList);
  Mock::AllowLeak(&mExpression);
  
  EXPECT_EXIT(methodCall.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Number of arguments for method call 'foo' of the object type 'MSquare' "
              "is not correct");
}

TEST_F(MethodCallTest, llvmImplementationNotFoundDeathTest) {
  NiceMock<MockExpression> argumentExpression;
  ON_CALL(argumentExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));
  mArgumentList.push_back(&argumentExpression);
  MethodCall methodCall(mExpression, "bar", mArgumentList);
  Mock::AllowLeak(&mExpression);
  Mock::AllowLeak(&argumentExpression);
  
  EXPECT_EXIT(methodCall.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: LLVM function implementing object 'MSquare' method 'bar' was not found");
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
                   "model.Square.foo",
                   mContext.getModule());

  NiceMock<MockExpression> argumentExpression;
  ON_CALL(argumentExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::LONG_TYPE));
  mArgumentList.push_back(&argumentExpression);
  MethodCall methodCall(mExpression, "foo", mArgumentList);
  Mock::AllowLeak(&mExpression);
  Mock::AllowLeak(&argumentExpression);

  EXPECT_EXIT(methodCall.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Call argument types do not match for a call to method 'foo' "
              "of the object type 'MSquare");
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
                   "object.MSquare.foo",
                   mContext.getModule());
  
  NiceMock<MockExpression> argumentExpression;
  Value* value = ConstantFP::get(Type::getFloatTy(mContext.getLLVMContext()), 5.2);
  ON_CALL(argumentExpression, generateIR(_)).WillByDefault(Return(value));
  ON_CALL(argumentExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));
  mArgumentList.push_back(&argumentExpression);
  MethodCall methodCall(mExpression, "foo", mArgumentList);
  Mock::AllowLeak(&mExpression);
  Mock::AllowLeak(&argumentExpression);
  
  Value* irValue = methodCall.generateIR(mContext);

  *mStringStream << *irValue;
  EXPECT_STREQ("  %call = call i32 @object.MSquare.foo(%MSquare* %test, float 0x4014CCCCC0000000)",
               mStringStream->str().c_str());
  EXPECT_EQ(methodCall.getType(mContext), PrimitiveTypes::INT_TYPE);
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
  expectFailIRGeneration("tests/samples/test_private_method_call.yz",
                         1,
                         "Error: Method 'getDouble\\(\\)' of object 'CService' is private");
}

