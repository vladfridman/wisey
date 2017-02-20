//
//  TestModelMethodCall.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ModelMethodCall}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "yazyk/Identifier.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/ModelMethodCall.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

class MockExpression : public IExpression {
public:
  MOCK_CONST_METHOD1(generateIR, Value* (IRGenerationContext&));
  MOCK_CONST_METHOD1(getType, IType* (IRGenerationContext&));
};

struct ModelMethodCallTest : public Test {
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
  ModelField* mWidthField;
  ModelField* mHeightField;
  
public:
  
  ModelMethodCallTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mIntType(Type::getInt32Ty(mContext.getLLVMContext())) {
    vector<Type*> types;
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    mStructType = StructType::create(mLLVMContext, "Shape");
    mStructType->setBody(types);
    map<string, ModelField*>* fields = new map<string, ModelField*>();
    mWidthField = new ModelField(PrimitiveTypes::INT_TYPE, 0);
    mHeightField = new ModelField(PrimitiveTypes::INT_TYPE, 1);
    (*fields)["width"] = mWidthField;
    (*fields)["height"] = mHeightField;
    MethodArgument* methodArgument = new MethodArgument(PrimitiveTypes::FLOAT_TYPE, "argument");
    vector<MethodArgument*> methodArguments;
    methodArguments.push_back(methodArgument);
    mMethod = new Method("foo", PrimitiveTypes::INT_TYPE, methodArguments);
    map<string, Method*>* methods = new map<string, Method*>();
    (*methods)["foo"] = mMethod;
    (*methods)["bar"] = new Method("bar", PrimitiveTypes::INT_TYPE, methodArguments);;
    mModel = new Model("Shape", mStructType, fields, methods);

    mBasicBlock = BasicBlock::Create(mContext.getLLVMContext(), "entry");
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    Value* nullPointer = ConstantPointerNull::get(Type::getInt32PtrTy(mLLVMContext));
    Value* bitcast = new BitCastInst(nullPointer, mStructType->getPointerTo(), "test", mBasicBlock);
    ON_CALL(mExpression, generateIR(_)).WillByDefault(Return(bitcast));
    ON_CALL(mExpression, getType(_)).WillByDefault(Return(mModel));
  }
  
  ~ModelMethodCallTest() {
    delete mBasicBlock;
    delete mStringStream;
  }
};

TEST_F(ModelMethodCallTest, TranslateMethodToLLVMFunctionNameTest) {
  string functionName = ModelMethodCall::translateMethodToLLVMFunctionName(mModel, "foo");
  
  EXPECT_STREQ(functionName.c_str(), "model.Shape.foo");
}

TEST_F(ModelMethodCallTest, MethodDoesNotExistDeathTest) {
  ModelMethodCall methodCall(mExpression, "lorem", mArgumentList);
  Mock::AllowLeak(&mExpression);
  
  EXPECT_EXIT(methodCall.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Method 'lorem' is not found in model 'Shape'");
}

TEST_F(ModelMethodCallTest, MethodCallOnPrimitiveTypeDeathTest) {
  ON_CALL(mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::BOOLEAN_TYPE));
  ModelMethodCall methodCall(mExpression, "foo", mArgumentList);
  Mock::AllowLeak(&mExpression);
  
  EXPECT_EXIT(methodCall.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Attempt to call a method 'foo' on a primitive type expression");
}

TEST_F(ModelMethodCallTest, IncorrectNumberOfArgumentsDeathTest) {
  ModelMethodCall methodCall(mExpression, "foo", mArgumentList);
  Mock::AllowLeak(&mExpression);
  
  EXPECT_EXIT(methodCall.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Number of arguments for method call 'foo' of the model type 'Shape' "
              "is not correct");
}

TEST_F(ModelMethodCallTest, LLVMImplementationNotFoundDeathTest) {
  NiceMock<MockExpression> argumentExpression;
  ON_CALL(argumentExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));
  mArgumentList.push_back(&argumentExpression);
  ModelMethodCall methodCall(mExpression, "bar", mArgumentList);
  Mock::AllowLeak(&mExpression);
  Mock::AllowLeak(&argumentExpression);
  
  EXPECT_EXIT(methodCall.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: LLVM function implementing model 'Shape' method 'bar' was not found");
}

TEST_F(ModelMethodCallTest, IncorrectArgumentTypesDeathTest) {
  vector<Type*> argumentTypes;
  argumentTypes.push_back(mStructType->getPointerTo());
  argumentTypes.push_back(PrimitiveTypes::FLOAT_TYPE->getLLVMType(mLLVMContext));
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext),
                                                 argTypesArray,
                                                 false);
  Function::Create(functionType,
                   GlobalValue::InternalLinkage,
                   "model.Shape.foo",
                   mContext.getModule());

  NiceMock<MockExpression> argumentExpression;
  ON_CALL(argumentExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::LONG_TYPE));
  mArgumentList.push_back(&argumentExpression);
  ModelMethodCall methodCall(mExpression, "foo", mArgumentList);
  Mock::AllowLeak(&mExpression);
  Mock::AllowLeak(&argumentExpression);

  EXPECT_EXIT(methodCall.generateIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Call argument types do not match for a call to method 'foo' "
              "of the model type 'Shape");
}

TEST_F(ModelMethodCallTest, ModelMethodCallTest) {
  vector<Type*> argumentTypes;
  argumentTypes.push_back(mStructType->getPointerTo());
  argumentTypes.push_back(PrimitiveTypes::FLOAT_TYPE->getLLVMType(mLLVMContext));
  ArrayRef<Type*> argTypesArray = ArrayRef<Type*>(argumentTypes);
  FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext),
                                                 argTypesArray,
                                                 false);
  Function::Create(functionType,
                   GlobalValue::InternalLinkage,
                   "model.Shape.foo",
                   mContext.getModule());
  
  NiceMock<MockExpression> argumentExpression;
  Value* value = ConstantFP::get(Type::getFloatTy(mContext.getLLVMContext()), 5.2);
  ON_CALL(argumentExpression, generateIR(_)).WillByDefault(Return(value));
  ON_CALL(argumentExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));
  mArgumentList.push_back(&argumentExpression);
  ModelMethodCall methodCall(mExpression, "foo", mArgumentList);
  Mock::AllowLeak(&mExpression);
  Mock::AllowLeak(&argumentExpression);
  
  Value* irValue = methodCall.generateIR(mContext);

  *mStringStream << *irValue;
  EXPECT_STREQ("  %call = call i32 @model.Shape.foo(%Shape* %test, float 0x4014CCCCC0000000)",
               mStringStream->str().c_str());
  EXPECT_EQ(methodCall.getType(mContext), PrimitiveTypes::INT_TYPE);
}

TEST_F(TestFileSampleRunner, ModelMethodCallRunTest) {
  runFile("tests/samples/test_model_method_call.yz", "5");
}

TEST_F(TestFileSampleRunner, ModelMethodCallMultipleArgumentsRunTest) {
  runFile("tests/samples/test_model_method_call_multiple_parameters.yz", "8");
}

TEST_F(TestFileSampleRunner, ModelMethodCallAutoCastArgumentRunTest) {
  runFile("tests/samples/test_method_argument_autocast.yz", "1");
}

TEST_F(TestFileSampleRunner, ModelMethodCallInExpressionTest) {
  runFile("tests/samples/test_method_call_in_expression.yz", "30");
}

