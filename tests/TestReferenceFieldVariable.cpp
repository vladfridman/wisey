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
  Node* mObject;
  Node* mNode;
  Interface* mInterface;
  BasicBlock* mBasicBlock;
  Value* mReferenceFieldValue;
  ReferenceFieldVariable* mReferenceFieldVariable;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  ReferenceFieldVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    string interfaceFullName = "systems.vos.wisey.compiler.tests.IInterface";
    StructType* interfaceStructType = StructType::create(mLLVMContext, interfaceFullName);
    vector<InterfaceTypeSpecifier*> parentInterfaces;
    vector<MethodSignatureDeclaration*> interfaceMethods;
    mInterface = Interface::newInterface(interfaceFullName,
                                         interfaceStructType,
                                         parentInterfaces,
                                         interfaceMethods);
    vector<Interface*> interfaces;
    interfaces.push_back(mInterface);

    string nodeFullName = "systems.vos.wisey.compiler.tests.NNode";
    StructType* nodeStructType = StructType::create(mLLVMContext, nodeFullName);
    mNode = Node::newNode(nodeFullName, nodeStructType);
    mNode->setInterfaces(interfaces);
  
    vector<Type*> types;
    types.push_back(mNode->getLLVMType(mLLVMContext)->getPointerElementType());
    types.push_back(mInterface->getLLVMType(mLLVMContext)->getPointerElementType());
    string objectFullName = "systems.vos.wisey.compiler.tests.NObject";
    StructType* objectStructType = StructType::create(mLLVMContext, objectFullName);
    objectStructType->setBody(types);
    vector<Field*> fields;
    ExpressionList fieldArguments;
    fields.push_back(new Field(STATE_FIELD, mNode->getOwner(), "foo", fieldArguments));
    fields.push_back(new Field(STATE_FIELD, mInterface->getOwner(), "bar", fieldArguments));
    fields.back()->setIndex(1u);
    mObject = Node::newNode(objectFullName, objectStructType);
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
   
    mReferenceFieldValue = ConstantPointerNull::get(mNode->getLLVMType(mLLVMContext));
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
  EXPECT_EQ(mReferenceFieldVariable->getType(), mNode->getOwner());
  EXPECT_EQ(mReferenceFieldVariable->getValue(), mReferenceFieldValue);
}

TEST_F(ReferenceFieldVariableTest, referenceFieldVariableGenerateIdentifierIRTest) {
  mReferenceFieldVariable->generateIdentifierIR(mContext, "test");

  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = load %systems.vos.wisey.compiler.tests.NObject*, "
  "%systems.vos.wisey.compiler.tests.NObject** null"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.NObject, "
  "%systems.vos.wisey.compiler.tests.NObject* %0, i32 0, i32 0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ReferenceFieldVariableTest, referenceFieldVariableGenerateAssignmentIRTest) {
  NiceMock<MockExpression> assignToExpression;
  
  PointerType* llvmType = (PointerType*) mNode->getLLVMType(mLLVMContext);
  Value* assignToValue = ConstantPointerNull::get(llvmType);
  ON_CALL(assignToExpression, getType(_)).WillByDefault(Return(mNode->getOwner()));
  ON_CALL(assignToExpression, generateIR(_)).WillByDefault(Return(assignToValue));
  
  mReferenceFieldVariable->generateAssignmentIR(mContext, &assignToExpression);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = load %systems.vos.wisey.compiler.tests.NObject*, "
  "%systems.vos.wisey.compiler.tests.NObject** null"
  "\n  %1 = getelementptr %systems.vos.wisey.compiler.tests.NObject, "
  "%systems.vos.wisey.compiler.tests.NObject* %0, i32 0, i32 0"
  "\n  %2 = load %systems.vos.wisey.compiler.tests.NNode*, "
  "%systems.vos.wisey.compiler.tests.NNode** null"
  "\n  store %systems.vos.wisey.compiler.tests.NNode* %2, "
  "%systems.vos.wisey.compiler.tests.NNode** %1\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ReferenceFieldVariableTest, referenceFieldVariableGenerateAssignmentWithCastIRTest) {
  NiceMock<MockExpression> assignToExpression;
  
  PointerType* llvmType = mNode->getLLVMType(mLLVMContext);
  Value* assignToValue = ConstantPointerNull::get(llvmType);
  ON_CALL(assignToExpression, getType(_)).WillByDefault(Return(mNode->getOwner()));
  ON_CALL(assignToExpression, generateIR(_)).WillByDefault(Return(assignToValue));
  
  Value* referenceFieldValue = ConstantPointerNull::get(mInterface->getLLVMType(mLLVMContext));
  ReferenceFieldVariable* referenceFieldVariable =
    new ReferenceFieldVariable("bar", referenceFieldValue, mObject);
  referenceFieldVariable->generateAssignmentIR(mContext, &assignToExpression);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.NNode** null "
  "to %systems.vos.wisey.compiler.tests.IInterface**"
  "\n  %1 = load %systems.vos.wisey.compiler.tests.NObject*, "
  "%systems.vos.wisey.compiler.tests.NObject** null"
  "\n  %2 = getelementptr %systems.vos.wisey.compiler.tests.NObject, "
  "%systems.vos.wisey.compiler.tests.NObject* %1, i32 0, i32 1"
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
