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
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/FieldReferenceVariable.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IInterfaceTypeSpecifier.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/ParameterReferenceVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/StateField.hpp"

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
     
    string interfaceFullName = "systems.vos.wisey.compiler.tests.IInterface";
    StructType* interfaceStructType = StructType::create(mLLVMContext, interfaceFullName);
    vector<IInterfaceTypeSpecifier*> parentInterfaces;
    vector<IObjectElementDefinition*> interfaceElements;
    mInterface = Interface::newInterface(AccessLevel::PUBLIC_ACCESS,
                                         interfaceFullName,
                                         interfaceStructType,
                                         parentInterfaces,
                                         interfaceElements,
                                         mContext.getImportProfile(),
                                         0);
    vector<Interface*> interfaces;
    interfaces.push_back(mInterface);

    string nodeFullName = "systems.vos.wisey.compiler.tests.NNode";
    StructType* nodeStructType = StructType::create(mLLVMContext, nodeFullName);
    mNode = Node::newNode(AccessLevel::PUBLIC_ACCESS,
                          nodeFullName,
                          nodeStructType,
                          mContext.getImportProfile(),
                          0);
    mNode->setInterfaces(interfaces);
  
    vector<Type*> types;
    types.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                    ->getPointerTo()->getPointerTo());
    types.push_back(mNode->getLLVMType(mContext));
    types.push_back(mInterface->getLLVMType(mContext));
    string objectFullName = "systems.vos.wisey.compiler.tests.CController";
    StructType* objectStructType = StructType::create(mLLVMContext, objectFullName);
    objectStructType->setBody(types);
    vector<IField*> fields;
    fields.push_back(new StateField(mNode, "foo", 0));
    fields.push_back(new StateField(mInterface, "bar", 0));
    mObject = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                        objectFullName,
                                        objectStructType,
                                        mContext.getImportProfile(),
                                        0);
    mObject->setFields(mContext, fields, 1u);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    Value* thisPointer = ConstantPointerNull::get(mObject->getLLVMType(mContext));
    IVariable* thisVariable = new ParameterReferenceVariable(IObjectType::THIS,
                                                             mObject,
                                                             thisPointer,
                                                             0);
    mContext.getScopes().setVariable(mContext, thisVariable);
   
    mFieldReferenceVariable = new FieldReferenceVariable("foo", mObject, 0);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~FieldReferenceVariableTest() {
    delete mStringStream;
    delete mObject;
  }
};

TEST_F(FieldReferenceVariableTest, basicFieldsTest) {
  EXPECT_STREQ("foo", mFieldReferenceVariable->getName().c_str());
  EXPECT_EQ(mNode, mFieldReferenceVariable->getType());
  EXPECT_TRUE(mFieldReferenceVariable->isField());
  EXPECT_FALSE(mFieldReferenceVariable->isSystem());
}

TEST_F(FieldReferenceVariableTest, generateIdentifierIRTest) {
  mFieldReferenceVariable->generateIdentifierIR(mContext, 0);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CController, %systems.vos.wisey.compiler.tests.CController* null, i32 0, i32 1"
  "\n  %foo = load %systems.vos.wisey.compiler.tests.NNode*, %systems.vos.wisey.compiler.tests.NNode** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldReferenceVariableTest, generateIdentifierreferenceIRTest) {
  mFieldReferenceVariable->generateIdentifierReferenceIR(mContext, 0);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CController, %systems.vos.wisey.compiler.tests.CController* null, i32 0, i32 1\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldReferenceVariableTest, generateAssignmentIRTest) {
  NiceMock<MockExpression> assignToExpression;
  
  llvm::PointerType* llvmType = (llvm::PointerType*) mNode->getLLVMType(mContext);
  Value* assignToValue = ConstantPointerNull::get(llvmType);
  ON_CALL(assignToExpression, getType(_)).WillByDefault(Return(mNode->getOwner()));
  ON_CALL(assignToExpression, generateIR(_, _)).WillByDefault(Return(assignToValue));
  vector<const IExpression*> arrayIndices;

  mFieldReferenceVariable->generateAssignmentIR(mContext, &assignToExpression, arrayIndices, 0);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CController, %systems.vos.wisey.compiler.tests.CController* null, i32 0, i32 1"
  "\n  %1 = load %systems.vos.wisey.compiler.tests.NNode*, %systems.vos.wisey.compiler.tests.NNode** %0"
  "\n  %2 = bitcast %systems.vos.wisey.compiler.tests.NNode* %1 to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectUnsafely(i8* %2, i64 -1)"
  "\n  %3 = bitcast %systems.vos.wisey.compiler.tests.NNode* null to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectUnsafely(i8* %3, i64 1)"
  "\n  store %systems.vos.wisey.compiler.tests.NNode* null, "
  "%systems.vos.wisey.compiler.tests.NNode** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldReferenceVariableTest, generateAssignmentWithCastIRTest) {
  NiceMock<MockExpression> assignToExpression;
  
  llvm::PointerType* llvmType = mNode->getLLVMType(mContext);
  Value* assignToValue = ConstantPointerNull::get(llvmType);
  ON_CALL(assignToExpression, getType(_)).WillByDefault(Return(mNode->getOwner()));
  ON_CALL(assignToExpression, generateIR(_, _)).WillByDefault(Return(assignToValue));
  vector<const IExpression*> arrayIndices;

  FieldReferenceVariable* referenceFieldVariable = new FieldReferenceVariable("bar", mObject, 0);
  referenceFieldVariable->generateAssignmentIR(mContext, &assignToExpression, arrayIndices, 0);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = bitcast %systems.vos.wisey.compiler.tests.NNode* null to i8*"
  "\n  %1 = getelementptr i8, i8* %0, i64 0"
  "\n  %2 = bitcast i8* %1 to %systems.vos.wisey.compiler.tests.IInterface*"
  "\n  %3 = getelementptr %systems.vos.wisey.compiler.tests.CController, %systems.vos.wisey.compiler.tests.CController* null, i32 0, i32 2"
  "\n  %4 = load %systems.vos.wisey.compiler.tests.IInterface*, %systems.vos.wisey.compiler.tests.IInterface** %3"
  "\n  %5 = bitcast %systems.vos.wisey.compiler.tests.IInterface* %4 to i8*"
  "\n  call void @__adjustReferenceCounter(i8* %5, i64 -1)"
  "\n  %6 = bitcast %systems.vos.wisey.compiler.tests.IInterface* %2 to i8*"
  "\n  call void @__adjustReferenceCounter(i8* %6, i64 1)"
  "\n  store %systems.vos.wisey.compiler.tests.IInterface* %2, "
  "%systems.vos.wisey.compiler.tests.IInterface** %3\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldReferenceVariableTest, decrementReferenceCounterTest) {
  mFieldReferenceVariable->decrementReferenceCounter(mContext);
  
  EXPECT_EQ(mBasicBlock->getInstList().size(), 0u);
}

TEST_F(TestFileRunner, compareTwoNullsRunTest) {
  runFile("tests/samples/test_compare_two_nulls.yz", "1");
}

TEST_F(TestFileRunner, compareToNullRunTest) {
  runFile("tests/samples/test_compare_to_null.yz", "1");
}

TEST_F(TestFileRunner, assignFieldReferenceToParameterOwnerCompileTest) {
  compileFile("tests/samples/test_assign_field_reference_to_parameter_owner.yz");
}

TEST_F(TestFileRunner, assignFieldReferenceToNullCompileTest) {
  compileFile("tests/samples/test_assign_field_reference_to_null.yz");
}

TEST_F(TestFileRunner, assignFieldReferenceToParameterReferenceCompileTest) {
  compileFile("tests/samples/test_assign_field_reference_to_parameter_reference.yz");
}
