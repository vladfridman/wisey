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
#include "FieldReferenceVariable.hpp"
#include "IExpression.hpp"
#include "IInterfaceTypeSpecifier.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "ParameterReferenceVariable.hpp"
#include "PrimitiveTypes.hpp"
#include "StateField.hpp"

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
  Function* mFunction;
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
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "main",
                                 mContext.getModule());
    BasicBlock* declareBlock = BasicBlock::Create(mLLVMContext, "declare", mFunction);
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setDeclarationsBlock(declareBlock);
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
  EXPECT_FALSE(mFieldReferenceVariable->isStatic());
}

TEST_F(FieldReferenceVariableTest, generateIdentifierIRTest) {
  mFieldReferenceVariable->generateIdentifierIR(mContext, 0);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:                                            ; No predecessors!" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CController, %systems.vos.wisey.compiler.tests.CController* null, i32 0, i32 1"
  "\n  %foo = load %systems.vos.wisey.compiler.tests.NNode*, %systems.vos.wisey.compiler.tests.NNode** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldReferenceVariableTest, generateIdentifierreferenceIRTest) {
  mFieldReferenceVariable->generateIdentifierReferenceIR(mContext, 0);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:                                            ; No predecessors!" +
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
  
  *mStringStream << *mFunction;
  string expected = string() +
  "\ndefine internal i32 @main() {"
  "\ndeclare:"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CController, %systems.vos.wisey.compiler.tests.CController* null, i32 0, i32 1"
  "\n  %1 = load %systems.vos.wisey.compiler.tests.NNode*, %systems.vos.wisey.compiler.tests.NNode** %0"
  "\n  %2 = icmp eq %systems.vos.wisey.compiler.tests.NNode* %1, null"
  "\n  br i1 %2, label %if.end, label %if.notnull"
  "\n"
  "\nif.end:                                           ; preds = %if.notnull, %entry"
  "\n  %3 = icmp eq %systems.vos.wisey.compiler.tests.NNode* null, null"
  "\n  br i1 %3, label %if.end1, label %if.notnull2"
  "\n"
  "\nif.notnull:                                       ; preds = %entry"
  "\n  %4 = bitcast %systems.vos.wisey.compiler.tests.NNode* %1 to i64*"
  "\n  %5 = getelementptr i64, i64* %4, i64 -1"
  "\n  %count = load i64, i64* %5"
  "\n  %6 = add i64 %count, -1"
  "\n  store i64 %6, i64* %5"
  "\n  br label %if.end"
  "\n"
  "\nif.end1:                                          ; preds = %if.notnull2, %if.end"
  "\n  store %systems.vos.wisey.compiler.tests.NNode* null, %systems.vos.wisey.compiler.tests.NNode** %0"
  "\n"
  "\nif.notnull2:                                      ; preds = %if.end"
  "\n  %7 = bitcast %systems.vos.wisey.compiler.tests.NNode* null to i64*"
  "\n  %8 = getelementptr i64, i64* %7, i64 -1"
  "\n  %count3 = load i64, i64* %8"
  "\n  %9 = add i64 %count3, 1"
  "\n  store i64 %9, i64* %8"
  "\n  br label %if.end1"
  "\n}\n";

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
  "\nentry:                                            ; No predecessors!" +
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
  runFile("tests/samples/test_compare_two_nulls.yz", 1);
}

TEST_F(TestFileRunner, compareToNullRunTest) {
  runFile("tests/samples/test_compare_to_null.yz", 1);
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
