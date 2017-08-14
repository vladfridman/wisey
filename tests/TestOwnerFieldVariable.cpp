//
//  TestOwnerFieldVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link OwnerFieldVariable}
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
#include "wisey/OwnerFieldVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ProgramPrefix.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct OwnerFieldVariableTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Model* mObject;
  Model* mModel;
  Interface* mInterface;
  BasicBlock* mBasicBlock;
  Value* mOwnerFieldValue;
  OwnerFieldVariable* mOwnerFieldVariable;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  OwnerFieldVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);

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
    types.push_back(mModel->getOwner()->getLLVMType(mLLVMContext));
    types.push_back(mInterface->getOwner()->getLLVMType(mLLVMContext));
    string objectFullName = "systems.vos.wisey.compiler.tests.MObject";
    StructType* objectStructType = StructType::create(mLLVMContext, objectFullName);
    objectStructType->setBody(types);
    map<string, Field*> fields;
    ExpressionList fieldArguments;
    fields["foo"] = new Field(mModel->getOwner(), "foo", 0, fieldArguments);
    fields["bar"] = new Field(mInterface->getOwner(), "bar", 1, fieldArguments);
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
    
    mOwnerFieldValue = ConstantPointerNull::get(mModel->getLLVMType(mLLVMContext));
    mOwnerFieldVariable = new OwnerFieldVariable("foo", mOwnerFieldValue, mObject);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~OwnerFieldVariableTest() {
    delete mStringStream;
    delete mObject;
  }
};

TEST_F(OwnerFieldVariableTest, basicFieldsTest) {
  EXPECT_STREQ(mOwnerFieldVariable->getName().c_str(), "foo");
  EXPECT_EQ(mOwnerFieldVariable->getType(), mModel->getOwner());
  EXPECT_EQ(mOwnerFieldVariable->getValue(), mOwnerFieldValue);
}

TEST_F(OwnerFieldVariableTest, ownerFieldVariableGenerateIdentifierIRTest) {
  mOwnerFieldVariable->generateIdentifierIR(mContext, "test");
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = load %systems.vos.wisey.compiler.tests.MObject*, "
  "%systems.vos.wisey.compiler.tests.MObject** null"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.MObject, "
  "%systems.vos.wisey.compiler.tests.MObject* %0, i32 0, i32 0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(OwnerFieldVariableTest, ownerFieldVariableGenerateAssignmentIRTest) {
  NiceMock<MockExpression> assignToExpression;
  
  PointerType* llvmType = (PointerType*) mModel->getOwner()->getLLVMType(mLLVMContext)
    ->getPointerTo();
  Value* assignToValue = ConstantPointerNull::get(llvmType);
  ON_CALL(assignToExpression, getType(_)).WillByDefault(Return(mModel->getOwner()));
  ON_CALL(assignToExpression, generateIR(_)).WillByDefault(Return(assignToValue));
  
  mOwnerFieldVariable->generateAssignmentIR(mContext, &assignToExpression);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = load %systems.vos.wisey.compiler.tests.MObject*, "
  "%systems.vos.wisey.compiler.tests.MObject** null"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.MObject, "
  "%systems.vos.wisey.compiler.tests.MObject* %0, i32 0, i32 0"
  "\n  %ownerFieldToFree = load %systems.vos.wisey.compiler.tests.MModel*, "
  "%systems.vos.wisey.compiler.tests.MModel** %1"
  "\n  %2 = bitcast %systems.vos.wisey.compiler.tests.MModel* %ownerFieldToFree to i8*"
  "\n  call void @__freeIfNotNull(i8* %2)"
  "\n  %3 = load %systems.vos.wisey.compiler.tests.MModel*, "
  "%systems.vos.wisey.compiler.tests.MModel** null"
  "\n  store %systems.vos.wisey.compiler.tests.MModel* %3, "
  "%systems.vos.wisey.compiler.tests.MModel** %1\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(OwnerFieldVariableTest, ownerFieldVariableGenerateAssignmentWithCastIRTest) {
  NiceMock<MockExpression> assignToExpression;
  
  PointerType* llvmType = mModel->getOwner()->getLLVMType(mLLVMContext)->getPointerTo();
  Value* assignToValue = ConstantPointerNull::get(llvmType);
  ON_CALL(assignToExpression, getType(_)).WillByDefault(Return(mModel->getOwner()));
  ON_CALL(assignToExpression, generateIR(_)).WillByDefault(Return(assignToValue));
  
  Value* ownerFieldValue =
    ConstantPointerNull::get(mInterface->getOwner()->getLLVMType(mLLVMContext));
  OwnerFieldVariable* ownerFieldVariable =
    new OwnerFieldVariable("bar", ownerFieldValue, mObject);
  ownerFieldVariable->generateAssignmentIR(mContext, &assignToExpression);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.MModel** null "
  "to %systems.vos.wisey.compiler.tests.IInterface**"
  "\n  %1 = load %systems.vos.wisey.compiler.tests.MObject*, "
  "%systems.vos.wisey.compiler.tests.MObject** null"
  "\n  %2 = getelementptr %systems.vos.wisey.compiler.tests.MObject, "
  "%systems.vos.wisey.compiler.tests.MObject* %1, i32 0, i32 1"
  "\n  %ownerFieldToFree = load %systems.vos.wisey.compiler.tests.IInterface*, "
  "%systems.vos.wisey.compiler.tests.IInterface** %2"
  "\n  %3 = bitcast %systems.vos.wisey.compiler.tests.IInterface* %ownerFieldToFree to i8*"
  "\n  call void @__freeIfNotNull(i8* %3)"
  "\n  %4 = load %systems.vos.wisey.compiler.tests.IInterface*, "
  "%systems.vos.wisey.compiler.tests.IInterface** %0"
  "\n  store %systems.vos.wisey.compiler.tests.IInterface* %4, "
  "%systems.vos.wisey.compiler.tests.IInterface** %2\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(OwnerFieldVariableTest, setToNullTest) {
  mOwnerFieldVariable->setToNull(mContext);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = load %systems.vos.wisey.compiler.tests.MObject*, "
  "%systems.vos.wisey.compiler.tests.MObject** null"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.MObject, "
  "%systems.vos.wisey.compiler.tests.MObject* %0, i32 0, i32 0"
  "\n  store %systems.vos.wisey.compiler.tests.MModel* null, "
  "%systems.vos.wisey.compiler.tests.MModel** %1\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(OwnerFieldVariableTest, existsInOuterScopeTest) {
  EXPECT_TRUE(mOwnerFieldVariable->existsInOuterScope());
}

TEST_F(TestFileSampleRunner, objectFieldVariableSetToNullTest) {
  expectFailCompile("tests/samples/test_object_field_variable_set_to_null.yz",
                    1,
                    "Error: Undeclared variable 'mModel'");
}

TEST_F(TestFileSampleRunner, objectFieldVariableSetToAnotherTest) {
  runFile("tests/samples/test_object_field_variable_set_to_another.yz", "7");
}

TEST_F(TestFileSampleRunner, objectFieldVariableNullComplicatedTest) {
  compileAndRunFile("tests/samples/test_object_field_variable_null_complicated.yz", 11);
}
