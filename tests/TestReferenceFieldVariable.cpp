//
//  TestReferenceFieldVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ReferenceFieldVariable}
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
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ReferenceFieldVariable.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ReferenceFieldVariableTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Model* mObject;
  Model* mModel;
  Interface* mInterface;
  BasicBlock* mBasicBlock;
  Value* mReferenceFieldValue;
  ReferenceFieldVariable* mReferenceFieldVariable;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  ReferenceFieldVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    string interfaceFullName = "systems.vos.wisey.compiler.tests.IInterface";
    StructType* interfaceStructType = StructType::create(mLLVMContext, interfaceFullName);
    mInterface = new Interface(interfaceFullName, interfaceStructType);
    vector<Interface*> interfaces;
    interfaces.push_back(mInterface);

    string modelFullName = "systems.vos.wisey.compiler.tests.MModel";
    StructType* modelStructType = StructType::create(mLLVMContext, modelFullName);
    mModel = new Model(modelFullName, modelStructType);
    mModel->setInterfaces(interfaces);
  
    vector<Type*> types;
    types.push_back(mModel->getLLVMType(mLLVMContext)->getPointerElementType());
    types.push_back(mInterface->getLLVMType(mLLVMContext)->getPointerElementType());
    string objectFullName = "systems.vos.wisey.compiler.tests.MObject";
    StructType* objectStructType = StructType::create(mLLVMContext, objectFullName);
    objectStructType->setBody(types);
    map<string, Field*> fields;
    ExpressionList fieldArguments;
    fields["foo"] = new Field(mModel, "foo", 0, fieldArguments);
    fields["bar"] = new Field(mInterface, "bar", 1, fieldArguments);
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
   
    mReferenceFieldValue = ConstantPointerNull::get(mModel->getLLVMType(mLLVMContext));
    mReferenceFieldVariable = new ReferenceFieldVariable("foo", mReferenceFieldValue, mObject);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~ReferenceFieldVariableTest() {
    delete mStringStream;
    delete mObject;
  }
};

TEST_F(ReferenceFieldVariableTest, basicFieldsTest) {
  EXPECT_STREQ(mReferenceFieldVariable->getName().c_str(), "foo");
  EXPECT_EQ(mReferenceFieldVariable->getType(), mModel);
  EXPECT_EQ(mReferenceFieldVariable->getValue(), mReferenceFieldValue);
}

TEST_F(ReferenceFieldVariableTest, referenceFieldVariableGenerateIdentifierIRTest) {
  mReferenceFieldVariable->generateIdentifierIR(mContext, "test");

  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = load %systems.vos.wisey.compiler.tests.MObject*, "
  "%systems.vos.wisey.compiler.tests.MObject** null"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.MObject, "
  "%systems.vos.wisey.compiler.tests.MObject* %0, i32 0, i32 0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ReferenceFieldVariableTest, referenceFieldVariableGenerateAssignmentIRTest) {
  NiceMock<MockExpression> assignToExpression;
  
  PointerType* llvmType = (PointerType*) mModel->getLLVMType(mLLVMContext);
  Value* assignToValue = ConstantPointerNull::get(llvmType);
  ON_CALL(assignToExpression, getType(_)).WillByDefault(Return(mModel));
  ON_CALL(assignToExpression, generateIR(_)).WillByDefault(Return(assignToValue));
  
  mReferenceFieldVariable->generateAssignmentIR(mContext, &assignToExpression);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = load %systems.vos.wisey.compiler.tests.MObject*, "
  "%systems.vos.wisey.compiler.tests.MObject** null"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.MObject, "
  "%systems.vos.wisey.compiler.tests.MObject* %0, i32 0, i32 0"
  "\n  %2 = load %systems.vos.wisey.compiler.tests.MModel*, "
  "%systems.vos.wisey.compiler.tests.MModel** null"
  "\n  store %systems.vos.wisey.compiler.tests.MModel* %2, "
  "%systems.vos.wisey.compiler.tests.MModel** %1\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ReferenceFieldVariableTest, referenceFieldVariableGenerateAssignmentWithCastIRTest) {
  NiceMock<MockExpression> assignToExpression;
  
  PointerType* llvmType = mModel->getLLVMType(mLLVMContext);
  Value* assignToValue = ConstantPointerNull::get(llvmType);
  ON_CALL(assignToExpression, getType(_)).WillByDefault(Return(mModel));
  ON_CALL(assignToExpression, generateIR(_)).WillByDefault(Return(assignToValue));
  
  Value* referenceFieldValue = ConstantPointerNull::get(mInterface->getLLVMType(mLLVMContext));
  ReferenceFieldVariable* referenceFieldVariable =
    new ReferenceFieldVariable("bar", referenceFieldValue, mObject);
  referenceFieldVariable->generateAssignmentIR(mContext, &assignToExpression);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.MModel** null "
  "to %systems.vos.wisey.compiler.tests.IInterface**"
  "\n  %1 = load %systems.vos.wisey.compiler.tests.MObject*, "
  "%systems.vos.wisey.compiler.tests.MObject** null"
  "\n  %2 = getelementptr %systems.vos.wisey.compiler.tests.MObject, "
  "%systems.vos.wisey.compiler.tests.MObject* %1, i32 0, i32 1"
  "\n  %3 = load %systems.vos.wisey.compiler.tests.IInterface*, "
  "%systems.vos.wisey.compiler.tests.IInterface** %0"
  "\n  store %systems.vos.wisey.compiler.tests.IInterface* %3, "
  "%systems.vos.wisey.compiler.tests.IInterface** %2\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ReferenceFieldVariableTest, existsInOuterScopeTest) {
  EXPECT_TRUE(mReferenceFieldVariable->existsInOuterScope());
}

TEST_F(TestFileSampleRunner, compareTwoNullsRunTest) {
  runFile("tests/samples/test_compare_two_nulls.yz", "1");
}

TEST_F(TestFileSampleRunner, compareToNullRunTest) {
  runFile("tests/samples/test_compare_to_null.yz", "1");
}
