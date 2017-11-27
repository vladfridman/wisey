//
//  TestFieldReferenceVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link FieldReferenceVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileSampleRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/FieldReferenceVariable.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/ParameterReferenceVariable.hpp"
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

struct FieldReferenceVariableTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Controller* mObject;
  Node* mNode;
  Interface* mInterface;
  BasicBlock* mBasicBlock;
  FieldReferenceVariable* mFieldReferenceVariable;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  FieldReferenceVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    
    string interfaceFullName = "systems.vos.wisey.compiler.tests.IInterface";
    StructType* interfaceStructType = StructType::create(mLLVMContext, interfaceFullName);
    vector<InterfaceTypeSpecifier*> parentInterfaces;
    vector<IObjectElementDeclaration*> interfaceElements;
    mInterface = Interface::newInterface(interfaceFullName,
                                         interfaceStructType,
                                         parentInterfaces,
                                         interfaceElements);
    vector<Interface*> interfaces;
    interfaces.push_back(mInterface);

    string nodeFullName = "systems.vos.wisey.compiler.tests.NNode";
    StructType* nodeStructType = StructType::create(mLLVMContext, nodeFullName);
    mNode = Node::newNode(nodeFullName, nodeStructType);
    mNode->setInterfaces(interfaces);
  
    vector<Type*> types;
    types.push_back(Type::getInt64Ty(mLLVMContext));
    types.push_back(mNode->getLLVMType(mLLVMContext));
    types.push_back(mInterface->getLLVMType(mLLVMContext));
    string objectFullName = "systems.vos.wisey.compiler.tests.NObject";
    StructType* objectStructType = StructType::create(mLLVMContext, objectFullName);
    objectStructType->setBody(types);
    vector<Field*> fields;
    ExpressionList fieldArguments;
    fields.push_back(new Field(STATE_FIELD, mNode, "foo", fieldArguments));
    fields.push_back(new Field(STATE_FIELD, mInterface, "bar", fieldArguments));
    mObject = Controller::newController(objectFullName, objectStructType);
    mObject->setFields(fields, 1u);
    
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
    IVariable* thisVariable = new ParameterReferenceVariable("this", mObject, thisPointer);
    mContext.getScopes().setVariable(thisVariable);
   
    mFieldReferenceVariable = new FieldReferenceVariable("foo", mObject);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~FieldReferenceVariableTest() {
    delete mStringStream;
    delete mObject;
  }
};

TEST_F(FieldReferenceVariableTest, basicFieldsTest) {
  EXPECT_STREQ(mFieldReferenceVariable->getName().c_str(), "foo");
  EXPECT_EQ(mFieldReferenceVariable->getType(), mNode);
}

TEST_F(FieldReferenceVariableTest, referenceFieldVariableGenerateIdentifierIRTest) {
  mFieldReferenceVariable->generateIdentifierIR(mContext);

  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.NObject, "
  "%systems.vos.wisey.compiler.tests.NObject* null, i32 0, i32 1"
  "\n  %referenceFieldIdentifier = load %systems.vos.wisey.compiler.tests.NNode*, "
  "%systems.vos.wisey.compiler.tests.NNode** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldReferenceVariableTest, referenceFieldVariableGenerateAssignmentIRTest) {
  NiceMock<MockExpression> assignToExpression;
  
  PointerType* llvmType = (PointerType*) mNode->getLLVMType(mLLVMContext);
  Value* assignToValue = ConstantPointerNull::get(llvmType);
  ON_CALL(assignToExpression, getType(_)).WillByDefault(Return(mNode->getOwner()));
  ON_CALL(assignToExpression, generateIR(_)).WillByDefault(Return(assignToValue));
  
  mFieldReferenceVariable->generateAssignmentIR(mContext, &assignToExpression);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.NObject, "
  "%systems.vos.wisey.compiler.tests.NObject* null, i32 0, i32 1"
  "\n  %1 = load %systems.vos.wisey.compiler.tests.NNode*, "
  "%systems.vos.wisey.compiler.tests.NNode** %0"
  "\n  %2 = bitcast %systems.vos.wisey.compiler.tests.NNode* %1 to i64*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectUnsafely(i64* %2, i64 -1)"
  "\n  %3 = bitcast %systems.vos.wisey.compiler.tests.NNode* null to i64*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectUnsafely(i64* %3, i64 1)"
  "\n  store %systems.vos.wisey.compiler.tests.NNode* null, "
  "%systems.vos.wisey.compiler.tests.NNode** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldReferenceVariableTest, referenceFieldVariableGenerateAssignmentWithCastIRTest) {
  NiceMock<MockExpression> assignToExpression;
  
  PointerType* llvmType = mNode->getLLVMType(mLLVMContext);
  Value* assignToValue = ConstantPointerNull::get(llvmType);
  ON_CALL(assignToExpression, getType(_)).WillByDefault(Return(mNode->getOwner()));
  ON_CALL(assignToExpression, generateIR(_)).WillByDefault(Return(assignToValue));
  
  FieldReferenceVariable* referenceFieldVariable = new FieldReferenceVariable("bar", mObject);
  referenceFieldVariable->generateAssignmentIR(mContext, &assignToExpression);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.NNode* null to i8*"
  "\n  %1 = getelementptr i8, i8* %0, i64 8"
  "\n  %2 = bitcast i8* %1 to %systems.vos.wisey.compiler.tests.IInterface*"
  "\n  %3 = getelementptr %systems.vos.wisey.compiler.tests.NObject, "
  "%systems.vos.wisey.compiler.tests.NObject* null, i32 0, i32 2"
  "\n  %4 = load %systems.vos.wisey.compiler.tests.IInterface*, "
  "%systems.vos.wisey.compiler.tests.IInterface** %3"
  "\n  %5 = bitcast %systems.vos.wisey.compiler.tests.IInterface* %4 to i8*"
  "\n  call void @__adjustReferenceCounterForInterface(i8* %5, i64 -1)"
  "\n  %6 = bitcast %systems.vos.wisey.compiler.tests.IInterface* %2 to i8*"
  "\n  call void @__adjustReferenceCounterForInterface(i8* %6, i64 1)"
  "\n  store %systems.vos.wisey.compiler.tests.IInterface* %2, "
  "%systems.vos.wisey.compiler.tests.IInterface** %3\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldReferenceVariableTest, decrementReferenceCounterTest) {
  mFieldReferenceVariable->decrementReferenceCounter(mContext);
  
  EXPECT_EQ(mBasicBlock->getInstList().size(), 0u);
}

TEST_F(FieldReferenceVariableTest, existsInOuterScopeTest) {
  EXPECT_TRUE(mFieldReferenceVariable->existsInOuterScope());
}

TEST_F(TestFileSampleRunner, compareTwoNullsRunTest) {
  runFile("tests/samples/test_compare_two_nulls.yz", "1");
}

TEST_F(TestFileSampleRunner, compareToNullRunTest) {
  runFile("tests/samples/test_compare_to_null.yz", "1");
}

TEST_F(TestFileSampleRunner, assignFieldReferenceToParameterOwnerCompileTest) {
  compileFile("tests/samples/test_assign_field_reference_to_parameter_owner.yz");
}

TEST_F(TestFileSampleRunner, assignFieldReferenceToNullCompileTest) {
  compileFile("tests/samples/test_assign_field_reference_to_null.yz");
}

TEST_F(TestFileSampleRunner, assignFieldReferenceToParameterReferenceCompileTest) {
  compileFile("tests/samples/test_assign_field_reference_to_parameter_reference.yz");
}
