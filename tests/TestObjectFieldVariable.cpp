//
//  TestObjectFieldVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ObjectFieldVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/HeapReferenceVariable.hpp"
#include "wisey/ObjectFieldVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ObjectFieldVariableTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Model* mModel;
  Model* mSubModel;
  BasicBlock* mBasicBlock;
  Value* mObjectFieldValue;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  ObjectFieldVariable* mObjectFieldVariable;
  
  ObjectFieldVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    string subModelFullName = "systems.vos.wisey.compiler.tests.MSubObject";
    StructType* subModelStructType = StructType::create(mLLVMContext, subModelFullName);
    mSubModel = new Model(subModelFullName, subModelStructType);
   
    vector<Type*> types;
    types.push_back(mSubModel->getLLVMType(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MObject";
    StructType* modelStructType = StructType::create(mLLVMContext, modelFullName);
    modelStructType->setBody(types);
    map<string, Field*> fields;
    ExpressionList fieldArguments;
    fields["foo"] = new Field(mSubModel->getOwner(), "foo", 0, fieldArguments);
    fields["bar"] = new Field(PrimitiveTypes::INT_TYPE, "bar", 1, fieldArguments);
    mModel = new Model(modelFullName, modelStructType);
    mModel->setFields(fields);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    Value* thisPointer = ConstantPointerNull::get(mModel->getLLVMType(mLLVMContext));
    IVariable* thisVariable = new HeapReferenceVariable("this", mModel, thisPointer);
    mContext.getScopes().setVariable(thisVariable);
   
    mObjectFieldValue = ConstantPointerNull::get(mSubModel->getLLVMType(mLLVMContext));
    mObjectFieldVariable = new ObjectFieldVariable("foo", mObjectFieldValue, mModel);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~ObjectFieldVariableTest() {
    delete mStringStream;
    delete mModel;
  }
};

TEST_F(ObjectFieldVariableTest, basicFieldsTest) {
  EXPECT_STREQ(mObjectFieldVariable->getName().c_str(), "foo");
  EXPECT_EQ(mObjectFieldVariable->getType(), mSubModel->getOwner());
  EXPECT_EQ(mObjectFieldVariable->getValue(), mObjectFieldValue);
}

TEST_F(ObjectFieldVariableTest, objectFieldVariableGenerateIdentifierIRTest) {
  mObjectFieldVariable->generateIdentifierIR(mContext, "test");

  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.MObject, "
    "%systems.vos.wisey.compiler.tests.MObject* null, i32 0, i32 0"
  "\n  %1 = load %systems.vos.wisey.compiler.tests.MSubObject*, "
    "%systems.vos.wisey.compiler.tests.MSubObject** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ObjectFieldVariableTest, objectFieldVariableGenerateAssignmentIRTest) {
  NiceMock<MockExpression> assignToExpression;
  
  Value* assignToValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
  ON_CALL(assignToExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  ON_CALL(assignToExpression, generateIR(_)).WillByDefault(Return(assignToValue));
  
  Value* originalValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 1);
  ObjectFieldVariable* objectFieldVariable =
  new ObjectFieldVariable("bar", originalValue, mModel);
  objectFieldVariable->generateAssignmentIR(mContext, &assignToExpression);

  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.MObject, "
    "%systems.vos.wisey.compiler.tests.MObject* null, i32 0, i32 1"
  "\n  store i32 5, i32* %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ObjectFieldVariableTest, objectFieldVariableGenerateAssignmentWithCastIRTest) {
  NiceMock<MockExpression> assignToExpression;
  
  Value* assignToValue = ConstantInt::get(Type::getInt1Ty(mLLVMContext), 1);
  ON_CALL(assignToExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::BOOLEAN_TYPE));
  ON_CALL(assignToExpression, generateIR(_)).WillByDefault(Return(assignToValue));
  
  Value* originalValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
  ObjectFieldVariable* objectFieldVariable =
    new ObjectFieldVariable("bar", originalValue, mModel);
  objectFieldVariable->generateAssignmentIR(mContext, &assignToExpression);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %conv = zext i1 true to i32"
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.MObject, "
    "%systems.vos.wisey.compiler.tests.MObject* null, i32 0, i32 1"
  "\n  store i32 %conv, i32* %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ObjectFieldVariableTest, setToNullTest) {
  mObjectFieldVariable->setToNull(mContext);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.MObject, "
    "%systems.vos.wisey.compiler.tests.MObject* null, i32 0, i32 0"
  "\n  store %systems.vos.wisey.compiler.tests.MSubObject* null, "
    "%systems.vos.wisey.compiler.tests.MSubObject** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ObjectFieldVariableTest, existsInOuterScopeTest) {
  EXPECT_TRUE(mObjectFieldVariable->existsInOuterScope());
}

TEST_F(TestFileSampleRunner, modelFieldSetRunTest) {
  runFile("tests/samples/test_model_field_set.yz", "7");
}

TEST_F(TestFileSampleRunner, modelFieldSetWithAutocastRunTest) {
  runFile("tests/samples/test_model_field_set_with_autocast.yz", "1");
}

TEST_F(TestFileSampleRunner, objectFieldVariableSetToNullTest) {
  runFile("tests/samples/test_object_field_variable_set_to_null.yz", "5");
}

TEST_F(TestFileSampleRunner, objectFieldVariableSetToAnotherTest) {
  runFile("tests/samples/test_object_field_variable_set_to_another.yz", "7");
}

TEST_F(TestFileSampleRunner, objectFieldVariableNullComplicatedTest) {
  compileAndRunFile("tests/samples/test_object_field_variable_null_complicated.yz", 11);
}
