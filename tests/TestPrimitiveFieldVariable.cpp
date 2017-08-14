//
//  TestPrimitiveFieldVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link PrimitiveFieldVariable}
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
#include "wisey/IRWriter.hpp"
#include "wisey/HeapReferenceVariable.hpp"
#include "wisey/PrimitiveFieldVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct PrimitiveFieldVariableTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Model* mObject;
  BasicBlock* mBasicBlock;
  Value* mPrimitiveFieldValue;
  PrimitiveFieldVariable* mPrimitiveFieldVariable;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  PrimitiveFieldVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    vector<Type*> types;
    types.push_back(PrimitiveTypes::INT_TYPE->getLLVMType(mLLVMContext));
    string objectFullName = "systems.vos.wisey.compiler.tests.MObject";
    StructType* objectStructType = StructType::create(mLLVMContext, objectFullName);
    objectStructType->setBody(types);
    map<string, Field*> fields;
    ExpressionList fieldArguments;
    fields["foo"] = new Field(PrimitiveTypes::INT_TYPE, "foo", 0, fieldArguments);
    mObject = new Model(objectFullName, objectStructType);
    mObject->setFields(fields);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    Value* thisPointer = ConstantPointerNull::get(mObject->getLLVMType(mLLVMContext));
    IVariable* thisVariable = new HeapReferenceVariable("this", mObject, thisPointer);
    mContext.getScopes().setVariable(thisVariable);
    
    mPrimitiveFieldValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 5);
    mPrimitiveFieldVariable = new PrimitiveFieldVariable("foo", mPrimitiveFieldValue, mObject);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~PrimitiveFieldVariableTest() {
    delete mStringStream;
    delete mObject;
  }
};

TEST_F(PrimitiveFieldVariableTest, basicFieldsTest) {
  EXPECT_STREQ(mPrimitiveFieldVariable->getName().c_str(), "foo");
  EXPECT_EQ(mPrimitiveFieldVariable->getType(), PrimitiveTypes::INT_TYPE);
  EXPECT_EQ(mPrimitiveFieldVariable->getValue(), mPrimitiveFieldValue);
}

TEST_F(PrimitiveFieldVariableTest, primitiveFieldVariableGenerateIdentifierIRTest) {
  mPrimitiveFieldVariable->generateIdentifierIR(mContext, "test");
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = load %systems.vos.wisey.compiler.tests.MObject*, "
  "%systems.vos.wisey.compiler.tests.MObject** null"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.MObject, "
  "%systems.vos.wisey.compiler.tests.MObject* %0, i32 0, i32 0"
  "\n  %2 = load i32, i32* %1\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(PrimitiveFieldVariableTest, primitiveFieldVariableGenerateAssignmentIRTest) {
  NiceMock<MockExpression> assignToExpression;
  
  Value* assignToValue = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
  ON_CALL(assignToExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  ON_CALL(assignToExpression, generateIR(_)).WillByDefault(Return(assignToValue));
  
  mPrimitiveFieldVariable->generateAssignmentIR(mContext, &assignToExpression);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = load %systems.vos.wisey.compiler.tests.MObject*, "
  "%systems.vos.wisey.compiler.tests.MObject** null"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.MObject, "
  "%systems.vos.wisey.compiler.tests.MObject* %0, i32 0, i32 0"
  "\n  store i32 3, i32* %1\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(PrimitiveFieldVariableTest, primitiveFieldVariableGenerateAssignmentWithCastIRTest) {
  NiceMock<MockExpression> assignToExpression;
  
  Value* assignToValue = ConstantInt::get(PrimitiveTypes::CHAR_TYPE->getLLVMType(mLLVMContext), 3);
  ON_CALL(assignToExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::CHAR_TYPE));
  ON_CALL(assignToExpression, generateIR(_)).WillByDefault(Return(assignToValue));
  
  mPrimitiveFieldVariable->generateAssignmentIR(mContext, &assignToExpression);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %conv = zext i16 3 to i32"
  "\n  %0 = load %systems.vos.wisey.compiler.tests.MObject*, "
  "%systems.vos.wisey.compiler.tests.MObject** null"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.MObject, "
  "%systems.vos.wisey.compiler.tests.MObject* %0, i32 0, i32 0"
  "\n  store i32 %conv, i32* %1\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(PrimitiveFieldVariableTest, existsInOuterScopeTest) {
  EXPECT_TRUE(mPrimitiveFieldVariable->existsInOuterScope());
}

TEST_F(TestFileSampleRunner, modelFieldSetRunTest) {
  runFile("tests/samples/test_model_field_set.yz", "7");
}

TEST_F(TestFileSampleRunner, modelFieldSetWithAutocastRunTest) {
  runFile("tests/samples/test_model_field_set_with_autocast.yz", "1");
}
